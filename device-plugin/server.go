// Copyright 2020 Cambricon, Inc.
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//      http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

package main

import (
	"context"
	"fmt"
	"log"
	"net"
	"os"
	"path"
	"strconv"
	"strings"
	"time"

	"github.com/cambricon/cambricon-k8s-device-plugin/device-plugin/pkg/cndev"
	"google.golang.org/grpc"

	pluginapi "k8s.io/kubelet/pkg/apis/deviceplugin/v1beta1"
)

const (
	resourceName = "cambricon.com/mlu"
	serverSock   = pluginapi.DevicePluginPath + "cambricon.sock"
)

// CambriconDevicePlugin implements the Kubernetes device plugin API
type CambriconDevicePlugin struct {
	devs       []*pluginapi.Device
	devsInfo   map[string]*cndev.Device
	socket     string
	stop       chan interface{}
	health     chan *pluginapi.Device
	server     *grpc.Server
	deviceList *deviceList
	options    Options
}

// NewCambriconDevicePlugin returns an initialized CambriconDevicePlugin
func NewCambriconDevicePlugin(o Options) *CambriconDevicePlugin {
	devs, devsInfo := getDevices(o.Mode, int(o.VirtualizationNum))
	return &CambriconDevicePlugin{
		devs:       devs,
		devsInfo:   devsInfo,
		socket:     serverSock,
		stop:       make(chan interface{}),
		health:     make(chan *pluginapi.Device),
		deviceList: newDeviceList(),
		options:    o,
	}
}

func (m *CambriconDevicePlugin) GetDevicePluginOptions(context.Context, *pluginapi.Empty) (*pluginapi.DevicePluginOptions, error) {
	return &pluginapi.DevicePluginOptions{}, nil
}

// dial establishes the gRPC communication with the registered device plugin.
func dial(unixSocketPath string, timeout time.Duration) (*grpc.ClientConn, error) {
	c, err := grpc.Dial(unixSocketPath, grpc.WithInsecure(), grpc.WithBlock(),
		grpc.WithTimeout(timeout),
		grpc.WithDialer(func(addr string, timeout time.Duration) (net.Conn, error) {
			return net.DialTimeout("unix", addr, timeout)
		}),
	)

	if err != nil {
		return nil, err
	}

	return c, nil
}

// Start starts the gRPC server of the device plugin
func (m *CambriconDevicePlugin) Start() error {
	err := m.cleanup()
	if err != nil {
		return err
	}

	sock, err := net.Listen("unix", m.socket)
	if err != nil {
		return err
	}

	m.server = grpc.NewServer([]grpc.ServerOption{}...)
	pluginapi.RegisterDevicePluginServer(m.server, m)

	go m.server.Serve(sock)

	// Wait for server to start by launching a blocking connection
	conn, err := dial(m.socket, 5*time.Second)
	if err != nil {
		return err
	}
	conn.Close()

	go m.healthcheck()

	return nil
}

// Stop stops the gRPC server
func (m *CambriconDevicePlugin) Stop() error {
	if m.server == nil {
		return nil
	}

	m.server.Stop()
	m.server = nil
	close(m.stop)

	return m.cleanup()
}

// Register registers the device plugin for the given resourceName with Kubelet.
func (m *CambriconDevicePlugin) Register(kubeletEndpoint, resourceName string) error {
	conn, err := dial(kubeletEndpoint, 5*time.Second)
	if err != nil {
		return err
	}
	defer conn.Close()

	client := pluginapi.NewRegistrationClient(conn)
	reqt := &pluginapi.RegisterRequest{
		Version:      pluginapi.Version,
		Endpoint:     path.Base(m.socket),
		ResourceName: resourceName,
	}

	_, err = client.Register(context.Background(), reqt)
	if err != nil {
		return err
	}
	return nil
}

// ListAndWatch lists devices and update that list according to the health status
func (m *CambriconDevicePlugin) ListAndWatch(e *pluginapi.Empty, s pluginapi.DevicePlugin_ListAndWatchServer) error {
	s.Send(&pluginapi.ListAndWatchResponse{Devices: m.devs})

	for {
		select {
		case <-m.stop:
			return nil
		case d := <-m.health:
			// FIXME: there is no way to recover from the Unhealthy state.
			d.Health = pluginapi.Unhealthy
			s.Send(&pluginapi.ListAndWatchResponse{Devices: m.devs})
		}
	}
}

func (m *CambriconDevicePlugin) unhealthy(dev *pluginapi.Device) {
	m.health <- dev
}

func (m *CambriconDevicePlugin) PrepareResponse(req *pluginapi.ContainerAllocateRequest) pluginapi.ContainerAllocateResponse {

	resp := pluginapi.ContainerAllocateResponse{}
	devpaths := m.uuidToPath(req.DevicesIDs)

	if m.deviceList.hasC10Dev {
		if m.deviceList.hasCnmonDev {
			addDevice(&resp, mlu100MonitorDeviceName, mlu100MonitorDeviceName)
		}
		if m.deviceList.hasCodecDev {
			addDevice(&resp, mlu100CodecDeviceName, mlu100CodecDeviceName)
		}
		for id, devpath := range devpaths {
			addDevice(&resp, devpath, mlu100DeviceName+strconv.Itoa(id))
		}
		return resp
	}

	if m.deviceList.hasCtrlDev {
		addDevice(&resp, mluMonitorDeviceName, mluMonitorDeviceName)
	}

	for id, devpath := range devpaths {
		if m.options.Mode == sriov {
			vfid := strings.Split(devpath, mluDeviceName)[1]
			if m.deviceList.hasCommuDev {
				addDevice(&resp, mluCommuDeviceName+vfid, mluCommuDeviceName+strconv.Itoa(id))
			}
			addDevice(&resp, devpath, mluDeviceName+strconv.Itoa(id))
			continue
		}

		var index int
		_, err := fmt.Sscanf(devpath, mluDeviceName+"%d", &index)
		if err != nil {
			log.Printf("Failed to get device index for device path %v", err)
			continue
		}
		if m.deviceList.hasMsgqDev {
			addDevice(&resp, fmt.Sprintf(mluMsgqDeviceName+":%d", index), fmt.Sprintf(mluMsgqDeviceName+":%d", id))
		}
		if m.deviceList.hasRPCDev {
			addDevice(&resp, fmt.Sprintf(mluRPCDeviceName+":%d", index), fmt.Sprintf(mluRPCDeviceName+":%d", id))
		}
		if m.deviceList.hasCmsgDev {
			addDevice(&resp, fmt.Sprintf(mluCmsgDeviceName+"%d", index), fmt.Sprintf(mluCmsgDeviceName+"%d", id))
		}
		if m.deviceList.hasCommuDev {
			addDevice(&resp, fmt.Sprintf(mluCommuDeviceName+"%d", index), fmt.Sprintf(mluCommuDeviceName+"%d", id))
		}
		if m.deviceList.hasUARTConsoleDev && m.options.EnableConsole {
			addDevice(&resp, fmt.Sprintf(mluUARTConsoleDeviceName+"%d", index), fmt.Sprintf(mluUARTConsoleDeviceName+"%d", id))
		}
		addDevice(&resp, devpath, mluDeviceName+strconv.Itoa(id))
	}
	return resp
}

// Allocate which return list of devices.
func (m *CambriconDevicePlugin) Allocate(ctx context.Context, reqs *pluginapi.AllocateRequest) (*pluginapi.AllocateResponse, error) {
	devs := m.devs
	responses := pluginapi.AllocateResponse{}

	for _, req := range reqs.ContainerRequests {

		car := m.PrepareResponse(req)
		responses.ContainerResponses = append(responses.ContainerResponses, &car)
		for _, id := range req.DevicesIDs {
			if !deviceExists(devs, id) {
				return nil, fmt.Errorf("invalid allocation request: unknown device: %s", id)
			}
		}
	}
	return &responses, nil
}

func (m *CambriconDevicePlugin) uuidToPath(uuids []string) []string {
	var paths []string
	for _, uuid := range uuids {
		dev := m.devsInfo[uuid]
		paths = append(paths, dev.Path)
	}
	return paths
}

func (m *CambriconDevicePlugin) PreStartContainer(context.Context, *pluginapi.PreStartContainerRequest) (*pluginapi.PreStartContainerResponse, error) {
	return &pluginapi.PreStartContainerResponse{}, nil
}

func (m *CambriconDevicePlugin) cleanup() error {
	if err := os.Remove(m.socket); err != nil && !os.IsNotExist(err) {
		return err
	}
	return nil
}

func (m *CambriconDevicePlugin) healthcheck() {
	ctx, cancel := context.WithCancel(context.Background())
	var unhealthy chan *pluginapi.Device
	if !m.options.DisableHealthCheck {
		unhealthy = make(chan *pluginapi.Device)
		go watchUnhealthy(ctx, m.devs, m.devsInfo, unhealthy)
	}

	for {
		select {
		case <-m.stop:
			cancel()
			return
		case dev := <-unhealthy:
			m.unhealthy(dev)
		}
	}
}

// Serve starts the gRPC server and register the device plugin to Kubelet
func (m *CambriconDevicePlugin) Serve() error {
	err := m.Start()
	if err != nil {
		log.Printf("Could not start device plugin: %s", err)
		return err
	}
	log.Println("Starting to serve on", m.socket)

	err = m.Register(pluginapi.KubeletSocket, resourceName)
	if err != nil {
		log.Printf("Could not register device plugin: %s", err)
		m.Stop()
		return err
	}
	log.Println("Registered device plugin with Kubelet")

	return nil
}

func (m *CambriconDevicePlugin) GetPreferredAllocation(ctx context.Context, r *pluginapi.PreferredAllocationRequest) (*pluginapi.PreferredAllocationResponse, error) {
	response := &pluginapi.PreferredAllocationResponse{}
	return response, nil
}

func addDevice(car *pluginapi.ContainerAllocateResponse, hostPath string, containerPath string) {
	dev := new(pluginapi.DeviceSpec)
	dev.HostPath = hostPath
	dev.ContainerPath = containerPath
	dev.Permissions = "rw"
	car.Devices = append(car.Devices, dev)
}
