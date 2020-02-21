/*************************************************************************
 * Copyright (C) [2019] by Cambricon, Inc. All rights reserved
 *
 *  Licensed under the Apache License, Version 2.0 (the "License");
 *  you may not use this file except in compliance with the License.
 *  You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS
 * OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL
 * THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 *************************************************************************/

package main

import (
	"fmt"
	"github.com/cambricon/cambricon-k8s-device-plugin/pkg/cndev"
	"golang.org/x/net/context"
	"google.golang.org/grpc"
	pluginapi "k8s.io/kubernetes/pkg/kubelet/apis/deviceplugin/v1beta1"
	"log"
	"net"
	"os"
	"path"
	"strings"
	"time"
)

const (
	resourceName           = "cambricon.com/mlu"
	serverSock             = pluginapi.DevicePluginPath + "cambricon.sock"
	envDisableHealthChecks = "DP_DISABLE_HEALTHCHECKS"
)

// CambriconDevicePlugin implements the Kubernetes device plugin API
type CambriconDevicePlugin struct {
	devs    []*pluginapi.Device
	socket  string
	devsMap []*cndev.Device
	stop    chan interface{}
	health  chan *pluginapi.Device

	server *grpc.Server
}

// NewCambriconDevicePlugin returns an initialized CambriconDevicePlugin
func NewCambriconDevicePlugin() *CambriconDevicePlugin {
	devs, devsMap := getDevicesAndMap()
	return &CambriconDevicePlugin{
		devs:    devs,
		devsMap: devsMap,
		socket:  serverSock,

		stop:   make(chan interface{}),
		health: make(chan *pluginapi.Device),
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

	// Wait for server to start by launching a blocking connexion
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

	car := pluginapi.ContainerAllocateResponse{}
	//check if card shared device like cnmon has been added into responses
	hasMLU100ShareDevice := false
	hasMLU270ShareDevice := false

	devpaths := m.uuidToPath(req.DevicesIDs)

	for _, devpath := range devpaths {
		if isMLU270Device(devpath) {
			if !hasMLU270ShareDevice {
				addDevice(&car, mlu270MonitorDeviceName)
				hasMLU270ShareDevice = true
			}

			var index int
			_, err := fmt.Sscanf(devpath, mlu270DeviceName+"%d", &index)

			if err != nil {
				log.Printf("Failed to get device index for device path %v", err)
			} else {
				addDevice(&car, fmt.Sprintf(mlu270MSGQDeviceName+":%d", index))
				addDevice(&car, fmt.Sprintf(mlu270RPCDeviceName+":%d", index))
				addDevice(&car, fmt.Sprintf(mlu270CmsgDeviceName+"%d", index))
				addDevice(&car, fmt.Sprintf(mlu270COMMUDeviceName+"%d", index))
			}

		} else {
			if !hasMLU100ShareDevice {
				addDevice(&car, mlu100MonitorDeviceName)
				_, err := os.Stat(mlu100CodecDeviceName)
				if err == nil {
					addDevice(&car, mlu100CodecDeviceName)
				}

				hasMLU100ShareDevice = true
			}
		}
		addDevice(&car, devpath)
	}
	return car
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
		for _, dev := range m.devsMap {
			if dev.UUID == uuid {
				paths = append(paths, dev.Path)
			}
		}
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
	disableHealthChecks := strings.ToLower(os.Getenv(envDisableHealthChecks))

	ctx, cancel := context.WithCancel(context.Background())

	var unhealthy chan *pluginapi.Device
	if !strings.Contains(disableHealthChecks, "all") {
		unhealthy = make(chan *pluginapi.Device)
		go watchUnhealthy(ctx, m.devs, m.devsMap, unhealthy)
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

func addDevice(car *pluginapi.ContainerAllocateResponse, deviceName string) {
	dev := new(pluginapi.DeviceSpec)
	dev.HostPath = deviceName
	dev.ContainerPath = deviceName
	dev.Permissions = "rw"
	car.Devices = append(car.Devices, dev)
}
