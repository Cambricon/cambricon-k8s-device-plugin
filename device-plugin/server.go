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
	"errors"
	"fmt"
	"log"
	"net"
	"os"
	"path"
	"sort"
	"strconv"
	"strings"
	"time"

	"github.com/Cambricon/cambricon-k8s-device-plugin/device-plugin/pkg/cndev"
	"google.golang.org/grpc"
	metav1 "k8s.io/apimachinery/pkg/apis/meta/v1"
	"k8s.io/client-go/kubernetes"
	"k8s.io/client-go/rest"
	pluginapi "k8s.io/kubelet/pkg/apis/deviceplugin/v1beta1"
)

const (
	serverSock               = pluginapi.DevicePluginPath + "cambricon.sock"
	mluLinkPolicyUnsatisfied = "mluLinkPolicyUnsatisfied"
	nodeUpdateRetry          = 5
)

var errMLULinkPolicyUnsatisfied = errors.New("mluLink policy unsatisfied")

// CambriconDevicePlugin implements the Kubernetes device plugin API
type CambriconDevicePlugin struct {
	devs         []*pluginapi.Device
	devsInfo     map[string]*cndev.Device
	socket       string
	stop         chan interface{}
	health       chan *pluginapi.Device
	server       *grpc.Server
	deviceList   *deviceList
	topology     map[int]string
	nodeHostname string
	clientset    kubernetes.Interface
	options      Options
}

// NewCambriconDevicePlugin returns an initialized CambriconDevicePlugin
func NewCambriconDevicePlugin(o Options) *CambriconDevicePlugin {
	devs, devsInfo := getDevices(o.Mode, int(o.VirtualizationNum))
	var c kubernetes.Interface
	if o.Mode == topologyAware {
		c = initClientSet()
	}
	return &CambriconDevicePlugin{
		devs:         devs,
		devsInfo:     devsInfo,
		socket:       serverSock,
		stop:         make(chan interface{}),
		health:       make(chan *pluginapi.Device),
		deviceList:   newDeviceList(),
		nodeHostname: o.NodeName,
		clientset:    c,
		options:      o,
	}
}

func (m *CambriconDevicePlugin) GetDevicePluginOptions(context.Context, *pluginapi.Empty) (*pluginapi.DevicePluginOptions, error) {
	return &pluginapi.DevicePluginOptions{
		GetPreferredAllocationAvailable: m.options.Mode == topologyAware,
	}, nil
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
		Options: &pluginapi.DevicePluginOptions{
			GetPreferredAllocationAvailable: m.options.Mode == topologyAware,
		},
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

	if m.options.CnmonPath != "" {
		resp.Mounts = []*pluginapi.Mount{
			{
				ContainerPath: m.options.CnmonPath,
				HostPath:      m.options.CnmonPath,
				ReadOnly:      true,
			},
		}
	}

	devpaths := m.uuidToPath(req.DevicesIDs)

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
		if m.deviceList.hasIpcmDev {
			addDevice(&resp, fmt.Sprintf(mluIpcmDeviceName+"%d", index), fmt.Sprintf(mluIpcmDeviceName+"%d", id))
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
	if m.options.CnmonPath != "" && !path.IsAbs(m.options.CnmonPath) {
		log.Panicf("invalid cnmon path: %s", m.options.CnmonPath)
	}

	if m.options.Mode == topologyAware {
		m.constructTopologyMap()
		log.Printf("topology index info %v \n", m.topology)

		if m.options.MLULinkPolicy != bestEffort {
			if err := m.updateNodeMLULinkAnnotation(0); err != nil {
				return err
			}
		}
	}

	if err := m.Start(); err != nil {
		return fmt.Errorf("start device plugin err: %v", err)
	}

	log.Printf("Starting to serve on socket %v", m.socket)

	resourceName := "cambricon.com/mlu"
	if m.options.EnableDeviceType {
		model := cndev.GetDeviceModel(uint(0))
		if model == "" {
			m.Stop()
			return errors.New("device type enabled, but got empty device model from cndev")
		}
		if strings.EqualFold(model, "MLU270-X5K") {
			resourceName = "cambricon.com/" + strings.ToLower(model)
		} else {
			resourceName = "cambricon.com/" + strings.Split(strings.ToLower(model), "-")[0]
		}
	}
	if err := m.Register(pluginapi.KubeletSocket, resourceName); err != nil {
		m.Stop()
		return fmt.Errorf("register resource %s err: %v", resourceName, err)
	}
	log.Printf("Registered resource %s", resourceName)
	return nil
}

func (m *CambriconDevicePlugin) GetPreferredAllocation(ctx context.Context, r *pluginapi.PreferredAllocationRequest) (*pluginapi.PreferredAllocationResponse, error) {
	response := &pluginapi.PreferredAllocationResponse{}
	for _, req := range r.ContainerRequests {
		allocated, err := m.GetPreferredAllocatedDeviceUUIDs(req.AvailableDeviceIDs, req.MustIncludeDeviceIDs, int(req.AllocationSize))
		if err != nil {
			log.Printf("failed to get preferred allocated devices, err: %v \n", err)
			return response, err
		}
		resp := &pluginapi.ContainerPreferredAllocationResponse{
			DeviceIDs: allocated,
		}
		response.ContainerResponses = append(response.ContainerResponses, resp)
	}
	return response, nil
}

func (m *CambriconDevicePlugin) GetPreferredAllocatedDeviceUUIDs(available []string, required []string, size int) ([]string, error) {

	// todo: consider required list for init containers and numa. ignore it for now.
	if len(required) != 0 {
		log.Printf("required device slice not empty, ignore it. %v \n", required)
	}

	if size == 4 {
		policy := map[string][][]int{
			bestEffort: {{0, 1, 2, 3}, {4, 5, 6, 7}, {0, 2, 4, 6}, {1, 3, 5, 7}, {0, 1, 4, 5}, {0, 3, 4, 7}, {2, 3, 6, 7}, {1, 2, 5, 6}},
			guaranteed: {{0, 1, 2, 3}, {4, 5, 6, 7}, {0, 2, 4, 6}, {1, 3, 5, 7}, {0, 1, 4, 5}, {0, 3, 4, 7}, {2, 3, 6, 7}, {1, 2, 5, 6}},
			restricted: {{0, 1, 2, 3}, {4, 5, 6, 7}},
		}
		preferred := policy[m.options.MLULinkPolicy]
		for _, set := range preferred {
			ids := m.getMLUIDsByTopo(set)
			if mluSetContainsAll(available, ids) {
				log.Printf("use preferred topo %v, devices %v, slots: %v\n", set, ids, m.getSlots(ids))
				return ids, nil
			}
		}
		if m.options.MLULinkPolicy != bestEffort {
			if err := m.updateNodeMLULinkAnnotation(size); err != nil {
				log.Printf("update node annotation err: %v", err)
			}
			return nil, fmt.Errorf("%v %w for required size %d, available: %v", m.options.MLULinkPolicy, errMLULinkPolicyUnsatisfied, size, available)
		}
	}
	needed := size
	allocated := []string{}

	allocateRemainingFrom := func(devices []string) bool {
		for _, device := range devices {
			if mluSetContains(allocated, device) {
				continue
			}
			allocated = append(allocated, device)
			needed--
			if needed == 0 {
				return true
			}
		}
		return false
	}

	motherBoards := make(map[string][]string)
	for _, dev := range available {
		raw := m.devsInfo[dev]
		motherBoards[raw.MotherBoard] = append(motherBoards[raw.MotherBoard], dev)
	}
	log.Printf("available devices seperated by mother board %v, request size: %d\n", motherBoards, size)

	mbs := [][]string{}
	for mb, v := range motherBoards {
		mbs = append(mbs, v)
		log.Printf("available devices slots for motherboard %s: %v", mb, m.getSlots(v))
	}
	sort.Slice(mbs, func(i int, j int) bool {
		return len(mbs[i]) < len(mbs[j])
	})

	if size == 2 {
		preferred := [][]int{{0, 2}, {1, 3}, {4, 6}, {5, 7}}
		for _, mb := range mbs {
			for _, set := range preferred {
				ids := m.getMLUIDsByTopo(set)
				if mluSetContainsAll(mb, ids) {
					return ids, nil
				}
			}
		}
		if m.options.MLULinkPolicy == restricted {
			if err := m.updateNodeMLULinkAnnotation(size); err != nil {
				log.Printf("update node annotation err: %v", err)
			}
			return nil, fmt.Errorf("%v %w for required size %d, available: %v", m.options.MLULinkPolicy, errMLULinkPolicyUnsatisfied, size, available)
		}
	}

	if len(mbs[0]) >= size {
		log.Printf("allocate from devices %v \n", mbs[0])
		allocateRemainingFrom(mbs[0])
		return allocated, nil
	}

	if allocateRemainingFrom(mbs[1]) {
		log.Printf("allocate from devices %v \n", mbs[1])
		return allocated, nil
	}

	log.Printf("allocated %v, allocate from another motherboard with linked devices\n", allocated)

	for _, dev := range mbs[1] {
		raw := m.devsInfo[dev]
		for k := range raw.MLULinkDevs {
			if !mluSetContains(available, k) {
				continue
			}
			if allocateRemainingFrom([]string{k}) {
				log.Printf("allocate linked device %s of device %s \n", k, dev)
				return allocated, nil
			}
		}
	}

	log.Printf("allocated %v, allocate from another motherboard with unlinked devices\n", allocated)

	if size == 2 && m.options.MLULinkPolicy == guaranteed {
		if err := m.updateNodeMLULinkAnnotation(size); err != nil {
			log.Printf("update node annotation err: %v", err)
		}
		return nil, fmt.Errorf("%v %w for required size %d, available: %v", m.options.MLULinkPolicy, errMLULinkPolicyUnsatisfied, size, available)
	}

	allocateRemainingFrom(mbs[0])
	return allocated, nil
}

func (m *CambriconDevicePlugin) constructTopologyMap() {
	if len(m.devsInfo) != 8 {
		err := fmt.Errorf("the number of known devices is %d, should be 8", len(m.devsInfo))
		panic(err)
	}
	motherBoards := make(map[string][]*cndev.Device)
	for _, dev := range m.devsInfo {
		motherBoards[dev.MotherBoard] = append(motherBoards[dev.MotherBoard], dev)
	}
	boards := []string{}
	for mb := range motherBoards {
		boards = append(boards, mb)
	}
	sort.Slice(boards, func(i int, j int) bool {
		return boards[i] < boards[j]
	})
	var dev0 *cndev.Device
	for _, dev := range motherBoards[boards[0]] {
		if dev0 == nil || dev0.UUID > dev.UUID {
			dev0 = dev
		}
	}
	topo := make(map[int]string)
	topo[0] = dev0.UUID
	for d, num := range dev0.MLULinkDevs {
		raw, ok := m.devsInfo[d]
		if !ok || raw.MotherBoard != dev0.MotherBoard {
			continue
		}
		if num == 2 {
			topo[2] = raw.UUID
			continue
		}
		dev1, ok := topo[1]
		if !ok {
			topo[1] = raw.UUID
			continue
		}
		if dev1 > raw.UUID {
			topo[1] = raw.UUID
			topo[3] = dev1
			continue
		}
		topo[3] = raw.UUID
	}

	for index := 0; index < 4; index++ {
		device := topo[index]
		for d := range m.devsInfo[device].MLULinkDevs {
			raw, ok := m.devsInfo[d]
			if !ok || raw.MotherBoard == m.devsInfo[device].MotherBoard {
				continue
			}
			topo[index+4] = raw.UUID
		}
	}

	m.topology = topo
}

func (m *CambriconDevicePlugin) getMLUIDsByTopo(topo []int) []string {
	res := []string{}
	for _, t := range topo {
		res = append(res, m.topology[t])
	}
	return res
}

func (m *CambriconDevicePlugin) createAnnotationWithTimestamp(size int) error {
	node, err := m.clientset.CoreV1().Nodes().Get(context.TODO(), m.nodeHostname, metav1.GetOptions{})
	if err != nil {
		return fmt.Errorf("get node err %v", err)
	}
	if size == 0 {
		delete(node.Annotations, mluLinkPolicyUnsatisfied)
	} else {
		timeStamp := strconv.FormatInt(time.Now().Unix(), 10)
		if len(node.Annotations) == 0 {
			node.Annotations = make(map[string]string)
		}
		node.Annotations[mluLinkPolicyUnsatisfied] = fmt.Sprintf("%d-%s-%s", size, m.options.MLULinkPolicy, timeStamp)
	}
	_, err = m.clientset.CoreV1().Nodes().Update(context.TODO(), node, metav1.UpdateOptions{})
	if err != nil {
		return fmt.Errorf("update node err: %v", err)
	}
	return nil
}

func (m *CambriconDevicePlugin) updateNodeMLULinkAnnotation(size int) error {
	for i := 0; i < nodeUpdateRetry; i++ {
		if err := m.createAnnotationWithTimestamp(size); err != nil {
			log.Printf("createAnnotationWithTimestamp err: %v, retried times: %d", err, i)
			time.Sleep(100 * time.Millisecond)
		} else {
			return nil
		}
	}
	return fmt.Errorf("update node annotation with size %d exceeds retry count %d", size, nodeUpdateRetry)
}

func (m *CambriconDevicePlugin) getSlots(ids []string) []uint {
	slots := []uint{}
	for _, id := range ids {
		mlu := m.devsInfo[id]
		slots = append(slots, mlu.Slot)
	}
	return slots
}

func mluSetContains(set []string, dev string) bool {
	for i := range set {
		if set[i] == dev {
			return true
		}
	}
	return false
}

func mluSetContainsAll(set []string, devs []string) bool {
	for _, dev := range devs {
		if !mluSetContains(set, dev) {
			return false
		}
	}
	return true
}

func addDevice(car *pluginapi.ContainerAllocateResponse, hostPath string, containerPath string) {
	dev := new(pluginapi.DeviceSpec)
	dev.HostPath = hostPath
	dev.ContainerPath = containerPath
	dev.Permissions = "rw"
	car.Devices = append(car.Devices, dev)
}

func initClientSet() kubernetes.Interface {
	config, err := rest.InClusterConfig()
	if err != nil {
		log.Printf("Failed to get in cluser config, err: %v", err)
	}
	clientset, err := kubernetes.NewForConfig(config)
	if err != nil {
		log.Printf("Failed to init clientset, err: %v", err)
	}
	return clientset
}
