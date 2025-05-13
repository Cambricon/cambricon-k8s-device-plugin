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

package mlu

import (
	"context"
	"encoding/json"
	"errors"
	"fmt"
	"math/rand"
	"net"
	"os"
	"path"
	"sort"
	"strconv"
	"strings"
	"sync"
	"syscall"
	"time"

	"github.com/Cambricon/cambricon-k8s-device-plugin/device-plugin/pkg/allocator"
	"github.com/Cambricon/cambricon-k8s-device-plugin/device-plugin/pkg/cndev"
	log "github.com/sirupsen/logrus"
	"google.golang.org/grpc"
	"google.golang.org/grpc/credentials/insecure"
	metav1 "k8s.io/apimachinery/pkg/apis/meta/v1"
	"k8s.io/apimachinery/pkg/types"
	"k8s.io/client-go/kubernetes"
	"k8s.io/client-go/rest"
	pluginapi "k8s.io/kubelet/pkg/apis/deviceplugin/v1beta1"
)

// CambriconDevicePlugin implements the Kubernetes device plugin API
type CambriconDevicePlugin struct {
	allocator    allocator.Allocator
	clientset    kubernetes.Interface
	deviceList   *deviceList
	devs         []*pluginapi.Device
	devsInfo     map[string]*cndev.Device
	health       chan *pluginapi.Device
	nodeHostname string
	options      Options
	profile      string
	server       *grpc.Server
	socket       string
	stop         chan interface{}
	sync.RWMutex
}

// Use global variables to synchronize information between goroutines, should always add lock and clean when finished
var dynamicSmlu map[string]*pluginapi.AllocateResponse
var profileAndInstance map[string]string

// NewCambriconDevicePlugin returns an initialized CambriconDevicePlugin
func NewCambriconDevicePlugin(o Options, profile string, devs []*pluginapi.Device, devsInfo map[string]*cndev.Device) *CambriconDevicePlugin {
	sock := serverSock
	if profile != normalMlu {
		sock = pluginapi.DevicePluginPath + profile + ".sock"
	}
	return &CambriconDevicePlugin{
		devs:         devs,
		devsInfo:     devsInfo,
		socket:       sock,
		stop:         make(chan interface{}),
		health:       make(chan *pluginapi.Device),
		deviceList:   newDeviceList(),
		nodeHostname: o.NodeName,
		options:      o,
		profile:      profile,
	}
}

func (m *CambriconDevicePlugin) GetDevicePluginOptions(context.Context, *pluginapi.Empty) (*pluginapi.DevicePluginOptions, error) {
	return &pluginapi.DevicePluginOptions{
		GetPreferredAllocationAvailable: m.options.Mode == TopologyAware || m.options.Mode == EnvShare,
	}, nil
}

// dial establishes the gRPC communication with the registered device plugin.
func dial(socket string, timeout time.Duration) (*grpc.ClientConn, error) {
	ctx, cancel := context.WithTimeout(context.Background(), timeout)
	defer cancel()

	conn, err := grpc.DialContext(ctx, socket, grpc.WithTransportCredentials(insecure.NewCredentials()), grpc.WithContextDialer(func(ctx context.Context, addr string) (net.Conn, error) {
		dialer := net.Dialer{
			Timeout: timeout,
		}
		return dialer.DialContext(ctx, "unix", addr)
	}), grpc.WithBlock())
	if err != nil {
		return nil, fmt.Errorf("failure connecting to %s: %v", socket, err)
	}
	return conn, nil
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

	if !m.options.DisableHealthCheck {
		go m.healthcheck()
	}

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
			GetPreferredAllocationAvailable: m.options.Mode == TopologyAware || m.options.Mode == EnvShare,
		},
	}

	_, err = client.Register(context.Background(), reqt)
	if err != nil {
		return err
	}
	return nil
}

// ListAndWatch lists devices and update that list according to the health status
func (m *CambriconDevicePlugin) ListAndWatch(_ *pluginapi.Empty, s pluginapi.DevicePlugin_ListAndWatchServer) error {
	if err := s.Send(&pluginapi.ListAndWatchResponse{Devices: m.devs}); err != nil {
		log.Errorf("Failed send list and watch response via sock %s with %v", m.socket, err)
		syscall.Kill(os.Getpid(), syscall.SIGHUP)
		return err
	}

	for {
		select {
		case <-m.stop:
			return nil
		case d := <-m.health:
			for i, dev := range m.devs {
				if dev.ID == d.ID {
					m.devs[i].Health = d.Health
					break
				}
			}
			if err := s.Send(&pluginapi.ListAndWatchResponse{Devices: m.devs}); err != nil {
				log.Errorf("Failed send list and watch response via sock %s with %v", m.socket, err)
				syscall.Kill(os.Getpid(), syscall.SIGHUP)
				return err
			}
		}
	}
}

func (m *CambriconDevicePlugin) PrepareResponse(uuids []string) pluginapi.ContainerAllocateResponse {
	resp := pluginapi.ContainerAllocateResponse{}

	devPaths := m.uuidToPath(uuids)
	log.Debugf("Prepare response device paths %v", devPaths)

	if m.options.UseRuntime {
		resp.Envs = make(map[string]string)
		if (m.options.Mode == Mim || m.options.Mode == DynamicSmlu) && m.profile != normalMlu {
			resp.Envs[virtualDevices] = strings.Join(devPaths, ";")
			return resp
		}
		var slots []string
		for _, uuid := range uuids {
			slots = append(slots, strconv.Itoa(int(m.devsInfo[uuid].Slot)))
		}
		resp.Envs[cambriconVisibleDevices] = strings.Join(slots, ",")
		return resp
	}

	if m.options.MountRPMsg {
		resp.Mounts = append(resp.Mounts, &pluginapi.Mount{
			ContainerPath: mluRPMsgDir,
			HostPath:      mluRPMsgDir,
		})
	}

	if m.options.CnmonPath != "" {
		resp.Mounts = append(resp.Mounts, &pluginapi.Mount{
			ContainerPath: m.options.CnmonPath,
			HostPath:      m.options.CnmonPath,
			ReadOnly:      true,
		})
	}

	if m.deviceList.hasCtrlDev {
		addDevice(&resp, mluMonitorDeviceName)
	}

	if m.deviceList.hasGdrDev {
		addDevice(&resp, mluGdrDeviceName)
	}

	omitDup := map[string]struct{}{}
	for _, devPath := range devPaths {
		if (m.options.Mode == Mim || m.options.Mode == DynamicSmlu) && m.profile != normalMlu {
			// devpath is like "/dev/cambricon_dev0,/dev/cambricon_ipcm0,/dev/cambricon-caps/cap_dev0_mi1"
			pathSets := strings.Split(devPath, ",")
			if len(pathSets) != 3 {
				log.Printf("Invalid devPath %s", devPath)
				continue
			}
			if _, ok := omitDup[pathSets[0]]; !ok {
				addDevice(&resp, pathSets[0])
				if m.deviceList.hasIpcmDev {
					addDevice(&resp, pathSets[1])
				}
				omitDup[pathSets[0]] = struct{}{}
			}
			addDevice(&resp, pathSets[2])
			continue
		}

		var index int
		_, err := fmt.Sscanf(devPath, mluDeviceName+"%d", &index)
		if err != nil {
			log.Printf("Failed to get device index for device path %v", err)
			continue
		}
		if m.deviceList.hasMsgqDev {
			addDevice(&resp, fmt.Sprintf(mluMsgqDeviceName+":%d", index))
		}
		if m.deviceList.hasRPCDev {
			addDevice(&resp, fmt.Sprintf(mluRPCDeviceName+":%d", index))
		}
		if m.deviceList.hasCmsgDev {
			addDevice(&resp, fmt.Sprintf(mluCmsgDeviceName+"%d", index))
		}
		if m.deviceList.hasCommuDev {
			addDevice(&resp, fmt.Sprintf(mluCommuDeviceName+"%d", index))
		}
		if m.deviceList.hasIpcmDev {
			addDevice(&resp, fmt.Sprintf(mluIpcmDeviceName+"%d", index))
		}
		if m.deviceList.hasUARTConsoleDev && m.options.EnableConsole {
			addDevice(&resp, fmt.Sprintf(mluUARTConsoleDeviceName+"%d", index))
		}
		addDevice(&resp, devPath)
	}
	return resp
}

func (m *CambriconDevicePlugin) GetDeviceUUIDByIndex(index uint) (uuid string, found bool) {
	for uuid, info := range m.devsInfo {
		if info.Slot == index {
			return uuid, true
		}
	}
	return "", false
}

func (m *CambriconDevicePlugin) allocateDynamicSmlu(ctx context.Context) (*pluginapi.AllocateResponse, error) {

	m.Lock()
	defer m.Unlock()

	pod, err := m.getDynamicSmluCandidatePod(ctx)
	if err != nil {
		log.Errorf("Failed to get dynamic smlu candidate pods, err %v", err)
		cleanDsmluRecord()
		return nil, fmt.Errorf("failed to get dynamic smlu candidate pods, err %v", err)
	}
	log.Debugf("Handling pod %s allocation", pod.Name)

	if resp, ok := dynamicSmlu[pod.Name]; ok {
		log.Debugf("Creating container in pod %s", pod.Name)
		defer cleanDsmluRecord()

		patchedAnnotation, err := json.Marshal(
			map[string]interface{}{
				"metadata": map[string]map[string]string{"annotations": {
					DsmluResourceAssigned:   "true",
					DsmluProfileAndInstance: profileAndInstance[pod.Name],
				}}})
		if err != nil {
			log.Errorf("Failed to patch pod annotation. err: %v", err)
			return nil, fmt.Errorf("patchPodAnnotation %v", err)
		}
		_, err = m.clientset.CoreV1().Pods(pod.Namespace).Patch(ctx, pod.Name, types.StrategicMergePatchType, patchedAnnotation, metav1.PatchOptions{})
		for i := 0; i < retries && err != nil; i++ {
			log.Warnf("PatchPodAnnotation err: %v, retried times: %d", err, i)
			time.Sleep(time.Duration(rand.Intn(i)) * 10 * time.Millisecond)
			_, err = m.clientset.CoreV1().Pods(pod.Namespace).Patch(ctx, pod.Name, types.StrategicMergePatchType, patchedAnnotation, metav1.PatchOptions{})
		}
		if err != nil {
			return nil, fmt.Errorf("patchPodAnnotation exceeds retry count %d", retries)
		}
		err = m.releaseNodeLock()
		for i := 0; i < retries && err != nil; i++ {
			log.Printf("Failed to release node lock, err %v, retried %d times", err, i)
			time.Sleep(time.Duration(rand.Intn(i)) * 10 * time.Millisecond)
			err = m.releaseNodeLock()
		}
		if err != nil {
			log.Printf("ReleaseNodeLock exceeds retry count %d", retries)
		}
		return resp, nil
	}

	pl, err := GetProfileFromAnnotation(pod)
	if err != nil {
		log.Errorf("Failed to get vcore and vmemory from annotation, err %v", err)
		return nil, fmt.Errorf("get profile from annotation %v", err)
	}
	log.Debugf("Get profile %v from pod %s", pl, pod.Name)

	var profileID int
	var dsmluInfo cndev.SmluInfo
	memUnit := m.options.MinDsmluUnit
	if m.options.MinDsmluUnit <= 0 {
		mem, err := cndev.GetDeviceMemory(uint(pl.Slot))
		check(err)
		memUnit = int(mem) / 100
	}
	if info, ok := cndev.GetExistProfile(pl, memUnit); ok {
		if info.Remain < 1 {
			log.Errorf("Found exist profile %d for device %d but its remain %d is invaild", info.ProfileID, pl.Slot, info.Remain)
			return nil, fmt.Errorf("found exist profile %d for device %d but its remain %d is invaild", info.ProfileID, pl.Slot, info.Remain)
		}
		log.Debugf("Get exist profile %v", info.ProfileID)
		profileID = info.ProfileID
	} else {
		prof, err := cndev.CreateSmluProfile(pl, memUnit)
		if err != nil {
			log.Errorf("Failed to create smlu profile %v err %v", pl, err)
			return nil, fmt.Errorf("failed to create smlu profile %v err %v", pl, err)
		}
		log.Debugf("Created smlu profile %d", prof)
		profileID = int(prof)
	}

	mluIntance, err := cndev.CreateSmluProfileInstance(uint(profileID), uint(pl.Slot))
	if err != nil {
		log.Errorf("Failed to create smlu profile %d for device %d err %v", profileID, pl.Slot, err)
		return nil, fmt.Errorf("failed to create smlu profile %d for device %d err %v", profileID, pl.Slot, err)
	}

	infos, err := cndev.GetAllSmluInfo(uint(pl.Slot))
	if err == nil {
		err = fmt.Errorf("allSmluInfos %v", infos)
		for _, info := range infos {
			if (info.InstanceID<<8 | pl.Slot) == mluIntance {
				dsmluInfo = info
				err = nil
				break
			}
		}
	}
	if err != nil {
		log.Errorf("Failed to get smlu info for slot %d to match instance handle %d err %v", pl.Slot, mluIntance, err)
		if err := cndev.DestroySmlu(mluIntance); err != nil {
			log.Errorf("Failed to destroy smlu with instance handle %d err %v", mluIntance, err)
		}
		return nil, fmt.Errorf("failed to get smlu info for slot %d to match instance handle %d", pl.Slot, mluIntance)
	}

	log.Debugf("Created profile %d instance %d for pod %s", profileID, mluIntance, pod.Name)

	uuid, ok := m.GetDeviceUUIDByIndex(uint(pl.Slot))
	if !ok {
		log.Errorf("Failed to get uuid by index %d", pl.Slot)
		return nil, fmt.Errorf("failed GetDeviceUUIDByIndex %d", pl.Slot)
	}

	uid := uuid + "-dsmlu-" + dsmluInfo.UUID
	m.devsInfo[uid] = &cndev.Device{
		Slot:    uint(pl.Slot),
		UUID:    uid,
		Path:    fmt.Sprintf("%s%d", mluDeviceName, pl.Slot) + "," + fmt.Sprintf("%s%d", mluIpcmDeviceName, pl.Slot) + "," + dsmluInfo.DevNodeName, // device name should never contain ","
		Profile: strings.ReplaceAll(dsmluInfo.Name, "+", "-"),
	}

	responses := pluginapi.AllocateResponse{}
	resp := m.PrepareResponse([]string{uid})
	responses.ContainerResponses = append(responses.ContainerResponses, &resp)

	dynamicSmlu = make(map[string]*pluginapi.AllocateResponse)
	dynamicSmlu[pod.Name] = &responses
	profileAndInstance = make(map[string]string)
	profileAndInstance[pod.Name] = fmt.Sprintf("%d_%d_%d_%d", profileID, mluIntance, pl.Slot, dsmluInfo.InstanceID)

	log.Debugf("Store in dynamicSmlu Map with key %s, value %v", pod.Name, responses)

	return &responses, nil
}

// Allocate which return list of devices.
func (m *CambriconDevicePlugin) Allocate(ctx context.Context, reqs *pluginapi.AllocateRequest) (*pluginapi.AllocateResponse, error) {
	ta := time.Now()
	log.Debugf("Receive allocate requesets %v in time %s", reqs, ta)

	if m.options.Mode == DynamicSmlu {
		return m.allocateDynamicSmlu(ctx)
	}

	responses := pluginapi.AllocateResponse{}
	for _, req := range reqs.ContainerRequests {
		for _, id := range req.DevicesIDs {
			if !deviceExists(m.devs, id) {
				return nil, fmt.Errorf("invalid allocation request: unknown device: %s", id)
			}
		}
		car := m.PrepareResponse(req.DevicesIDs)
		responses.ContainerResponses = append(responses.ContainerResponses, &car)
	}

	tf := time.Now()
	log.Debugf("Receive allocate response for reqs %v in time %s, diff is %s", reqs, tf, tf.Sub(ta))
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
	health := make(chan *pluginapi.Device)

	go watchUnhealthy(ctx, m.devsInfo, health)

	for {
		select {
		case <-m.stop:
			cancel()
			return
		case dev := <-health:
			m.health <- dev
		}
	}
}

// Serve starts the gRPC server and register the device plugin to Kubelet
func (m *CambriconDevicePlugin) Serve() error {
	if m.options.CnmonPath != "" && !path.IsAbs(m.options.CnmonPath) {
		log.Panicf("Invalid cnmon path: %s", m.options.CnmonPath)
	}

	if m.options.Mode == TopologyAware {
		m.allocator = allocator.New(m.options.MLULinkPolicy, m.devsInfo)
		m.clientset = initClientSet()

		if m.options.MLULinkPolicy != bestEffort {
			if err := m.updateNodeMLULinkAnnotation(0); err != nil {
				return err
			}
		}
	}

	if m.options.Mode == DynamicSmlu {
		m.clientset = initClientSet()
		if err := m.releaseNodeLock(); err != nil {
			return err
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
	switch m.options.Mode {
	case DynamicSmlu:
		if m.profile != normalMlu {
			resourceName = resourceName + ".smlu." + m.profile
		}
	case EnvShare:
		resourceName = resourceName + ".share"
	case Mim:
		if m.profile != normalMlu {
			resourceName = resourceName + ".mim-" + m.profile
		}
	}
	if m.profile == realCounts {
		resourceName = "cambricon.com/" + realCounts
	}
	if err := m.Register(pluginapi.KubeletSocket, resourceName); err != nil {
		m.Stop()
		return fmt.Errorf("register resource %s err: %v", resourceName, err)
	}
	log.Printf("Registered resource %s", resourceName)
	return nil
}

func (m *CambriconDevicePlugin) GetPreferredAllocation(_ context.Context, r *pluginapi.PreferredAllocationRequest) (*pluginapi.PreferredAllocationResponse, error) {
	response := &pluginapi.PreferredAllocationResponse{}
	for _, req := range r.ContainerRequests {
		var allocated []string
		var err error
		switch m.options.Mode {
		case EnvShare:
			allocated, err = getPreferredEnvShareDeviceID(req)
			if err != nil {
				log.Errorf("Failed to get preferred allocated devices for env share mode, requests %d, err: %v", req.AllocationSize, err)
				return response, err
			}
		case TopologyAware:
			available := m.getSlots(req.AvailableDeviceIDs)
			required := m.getSlots(req.MustIncludeDeviceIDs)
			allocated, err = m.getPreferredAllocatedDeviceUUIDs(available, required, int(req.AllocationSize))
			if err != nil {
				log.Errorf("Failed to get preferred allocated devices, available: %v, size: %d, err: %v", available, req.AllocationSize, err)
				return response, err
			}
		default:
			log.Errorf("Not supported mode %s", m.options.Mode)
			return response, fmt.Errorf("not supported mode %s", m.options.Mode)
		}
		resp := &pluginapi.ContainerPreferredAllocationResponse{
			DeviceIDs: allocated,
		}
		response.ContainerResponses = append(response.ContainerResponses, resp)
	}
	return response, nil
}

func (m *CambriconDevicePlugin) getPreferredAllocatedDeviceUUIDs(available []uint, required []uint, size int) ([]string, error) {
	// todo: consider required list for init containers and numa. ignore it for now.
	if len(required) != 0 {
		log.Printf("Required device slice not empty, ignore it. %v", required)
	}

	log.Println("=== Start getPreferredAllocatedDeviceUUIDs ===")
	log.Printf("Available devs: %v, size %d", available, size)

	var devs []uint
	if m.options.MLULinkPolicy == bestEffort && len(available) == size {
		devs = available
	} else {
		var err error
		devs, err = m.allocator.Allocate(available, required, size)
		if err != nil {
			if e := m.updateNodeMLULinkAnnotation(size); e != nil {
				log.Errorf("UpdateNodeMLULinkAnnotation err: %v", e)
			}
			return nil, err
		}
	}

	log.Printf("Preferred devices %v", devs)

	uuids := []string{}
	for _, dev := range devs {
		uuid, found := m.GetDeviceUUIDByIndex(dev)
		if !found {
			return nil, fmt.Errorf("uuid not found for dev %d", dev)
		}
		uuids = append(uuids, uuid)
	}

	log.Println("=== Finish getPreferredAllocatedDeviceUUIDs ===")
	return uuids, nil
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
	err := m.createAnnotationWithTimestamp(size)
	for i := 0; i < retries && err != nil; i++ {
		log.Printf("CreateAnnotationWithTimestamp err: %v, retried times: %d", err, i+1)
		time.Sleep(time.Duration(rand.Intn(i)) * 10 * time.Millisecond)
		err = m.createAnnotationWithTimestamp(size)
	}
	return err
}

func (m *CambriconDevicePlugin) getSlots(ids []string) []uint {
	slots := []uint{}
	for _, id := range ids {
		mlu := m.devsInfo[id]
		slots = append(slots, mlu.Slot)
	}
	return slots
}

func cleanDsmluRecord() {
	dynamicSmlu = nil
	profileAndInstance = nil
}

func addDevice(car *pluginapi.ContainerAllocateResponse, devPath string) {
	dev := new(pluginapi.DeviceSpec)
	dev.HostPath = devPath
	dev.ContainerPath = devPath
	dev.Permissions = "rw"
	car.Devices = append(car.Devices, dev)
}

func initClientSet() kubernetes.Interface {
	config, err := rest.InClusterConfig()
	if err != nil {
		log.Errorf("Failed to get in cluser config, err: %v", err)
	}
	clientset, err := kubernetes.NewForConfig(config)
	if err != nil {
		log.Errorf("Failed to init clientset, err: %v", err)
	}
	return clientset
}

func getPreferredEnvShareDeviceID(req *pluginapi.ContainerPreferredAllocationRequest) ([]string, error) {
	dMap := map[string][]int{}
	for _, id := range req.AvailableDeviceIDs {
		ids := strings.Split(id, "-_-")
		uuid := ids[0]
		vf, err := strconv.Atoi(ids[len(ids)-1])
		if err != nil {
			log.Errorf("Convert value to int, error %v, value %s", err, ids[len(ids)-1])
			return nil, err
		}
		dMap[uuid] = append(dMap[uuid], vf)
	}

	type envShareDeviceID struct {
		uuid string
		vfs  []int
	}
	es := []envShareDeviceID{}
	for k, v := range dMap {
		sort.Ints(v)
		es = append(es, envShareDeviceID{
			uuid: k,
			vfs:  v,
		})
	}
	sort.Slice(es, func(i int, j int) bool {
		return len(es[i].vfs) > len(es[j].vfs)
	})

	if req.AllocationSize == 1 {
		return []string{fmt.Sprintf("%s-_-%d", es[0].uuid, es[0].vfs[0])}, nil
	}

	if int(req.AllocationSize) > len(es) {
		return nil, fmt.Errorf("can not get preferred devices since available is shorter than required, required:%d, available:%d", int(req.AllocationSize), len(es))
	}

	res := []string{}
	for i := 0; i < int(req.AllocationSize); i++ {
		res = append(res, fmt.Sprintf("%s-_-%d", es[i].uuid, es[i].vfs[0]))
	}
	return res, nil
}
