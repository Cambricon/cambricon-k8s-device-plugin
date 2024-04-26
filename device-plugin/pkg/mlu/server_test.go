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
	"testing"

	"github.com/Cambricon/cambricon-k8s-device-plugin/device-plugin/pkg/cndev"
	"github.com/stretchr/testify/assert"
	v1 "k8s.io/api/core/v1"
	"k8s.io/apimachinery/pkg/api/resource"
	metav1 "k8s.io/apimachinery/pkg/apis/meta/v1"
	"k8s.io/client-go/kubernetes/fake"
	pluginapi "k8s.io/kubelet/pkg/apis/deviceplugin/v1beta1"
)

func TestCambriconDevicePluginAllocate(t *testing.T) {
	devs := []*pluginapi.Device{
		{
			ID: "MLU-testdevice-sn0001",
		},
		{
			ID: "MLU-testdevice-sn0002",
		},
	}
	devsInfo := map[string]*cndev.Device{
		"MLU-testdevice-sn0001": {
			UUID: "MLU-testdevice-sn0001",
			Path: "/dev/cambricon_dev0",
		},
		"MLU-testdevice-sn0002": {
			UUID: "MLU-testdevice-sn0002",
			Path: "/dev/cambricon_dev1",
		},
	}

	tests := []struct {
		m   *CambriconDevicePlugin
		req *pluginapi.AllocateRequest
	}{
		{
			m: &CambriconDevicePlugin{
				devs:     devs,
				devsInfo: devsInfo,
				options: Options{
					Mode: "default",
				},
				deviceList: &deviceList{
					hasCtrlDev:  true,
					hasMsgqDev:  true,
					hasRPCDev:   true,
					hasCmsgDev:  true,
					hasCommuDev: true,
					hasIpcmDev:  true,
				},
			},
			req: &pluginapi.AllocateRequest{
				ContainerRequests: []*pluginapi.ContainerAllocateRequest{
					{
						DevicesIDs: []string{
							"MLU-testdevice-sn0001",
							"MLU-testdevice-sn0002",
						},
					},
				},
			},
		},
		{
			m: &CambriconDevicePlugin{
				devs:     devs,
				devsInfo: devsInfo,
				options: Options{
					Mode:          "default",
					EnableConsole: true,
				},
				deviceList: &deviceList{
					hasCtrlDev:        true,
					hasGdrDev:         true,
					hasCommuDev:       true,
					hasUARTConsoleDev: true,
				},
			},
			req: &pluginapi.AllocateRequest{
				ContainerRequests: []*pluginapi.ContainerAllocateRequest{
					{
						DevicesIDs: []string{
							"MLU-testdevice-sn0002",
						},
					},
				},
			},
		},
	}

	for i, tt := range tests {
		t.Run("", func(t *testing.T) {
			ctx := context.TODO()
			resp, err := tt.m.Allocate(ctx, tt.req)
			assert.NoError(t, err)
			assert.Equal(t, 1, len(resp.ContainerResponses))
			devices := resp.ContainerResponses[0].Devices
			assert.Equal(t, mluMonitorDeviceName, devices[0].HostPath)
			switch i {
			case 0:
				assert.Equal(t, 13, len(devices))
				assert.Equal(t, mluMsgqDeviceName+":0", devices[1].HostPath)
				assert.Equal(t, mluRPCDeviceName+":0", devices[2].HostPath)
				assert.Equal(t, mluCmsgDeviceName+"0", devices[3].HostPath)
				assert.Equal(t, mluCommuDeviceName+"0", devices[4].HostPath)
				assert.Equal(t, mluIpcmDeviceName+"0", devices[5].HostPath)
				assert.Equal(t, mluDeviceName+"0", devices[6].HostPath)
				assert.Equal(t, mluMsgqDeviceName+":1", devices[7].HostPath)
				assert.Equal(t, mluRPCDeviceName+":1", devices[8].HostPath)
				assert.Equal(t, mluCmsgDeviceName+"1", devices[9].HostPath)
				assert.Equal(t, mluCommuDeviceName+"1", devices[10].HostPath)
				assert.Equal(t, mluIpcmDeviceName+"1", devices[11].HostPath)
				assert.Equal(t, mluDeviceName+"1", devices[12].HostPath)
			case 1:
				assert.Equal(t, 5, len(devices))
				assert.Equal(t, mluGdrDeviceName, devices[1].HostPath)
				assert.Equal(t, mluCommuDeviceName+"1", devices[2].HostPath)
				assert.Equal(t, mluUARTConsoleDeviceName+"1", devices[3].HostPath)
				assert.Equal(t, mluDeviceName+"1", devices[4].HostPath)
			}
		})
	}
}

func TestGetPreferredEnvShareDeviceID(t *testing.T) {
	reqs := []pluginapi.ContainerPreferredAllocationRequest{
		{
			AvailableDeviceIDs: []string{"MLU-testdevice-sn0001-_-12", "MLU-testdevice-sn0001-_-4", "MLU-testdevice-sn0001-_-1", "MLU-testdevice-sn0001-_-2", "MLU-testdevice-sn0003-_-1"},
			AllocationSize:     1,
		},
		{
			AvailableDeviceIDs: []string{"MLU-testdevice-sn0001-_-12", "MLU-testdevice-sn0001-_-4", "MLU-testdevice-sn0001-_-2", "MLU-testdevice-sn0003-_-1"},
			AllocationSize:     2,
		},
		{
			AvailableDeviceIDs: []string{"MLU-testdevice-sn0001-_-12", "MLU-testdevice-sn0001-_-4"},
			AllocationSize:     2,
		},
	}

	expects := [][]string{
		{"MLU-testdevice-sn0001-_-1"},
		{"MLU-testdevice-sn0001-_-2", "MLU-testdevice-sn0003-_-1"},
		nil,
	}

	for i, req := range reqs {
		ids, err := getPreferredEnvShareDeviceID(&req)
		if i != 2 {
			assert.NoError(t, err)
			assert.Equal(t, ids, expects[i])
			continue
		}
		assert.EqualError(t, err, "can not get preferred devices since available is shorter than required, required:2, available:1")
	}
}

func TestCambriconDevicePluginAllocateEnvShare(t *testing.T) {
	devs := []*pluginapi.Device{
		{
			ID: "MLU-testdevice-sn0001-_-1",
		},
		{
			ID: "MLU-testdevice-sn0001-_-2",
		},
	}
	devsInfo := map[string]*cndev.Device{
		"MLU-testdevice-sn0001-_-1": {
			UUID: "MLU-testdevice-sn0001-_-1",
			Path: "/dev/cambricon_dev0",
		},
		"MLU-testdevice-sn0001-_-2": {
			UUID: "MLU-testdevice-sn0001-_-2",
			Path: "/dev/cambricon_dev0",
		},
	}
	req := &pluginapi.AllocateRequest{
		ContainerRequests: []*pluginapi.ContainerAllocateRequest{
			{
				DevicesIDs: []string{
					"MLU-testdevice-sn0001-_-2",
				},
			},
		},
	}
	tests := []struct {
		m   *CambriconDevicePlugin
		req *pluginapi.AllocateRequest
	}{
		{
			m: &CambriconDevicePlugin{
				devs:     devs,
				devsInfo: devsInfo,
				options: Options{
					Mode:              EnvShare,
					VirtualizationNum: 2,
				},
				deviceList: &deviceList{
					hasCtrlDev:  true,
					hasMsgqDev:  true,
					hasRPCDev:   true,
					hasCmsgDev:  true,
					hasCommuDev: true,
					hasIpcmDev:  true,
				},
			},
			req: req,
		},
		{
			m: &CambriconDevicePlugin{
				devs:     devs,
				devsInfo: devsInfo,
				options: Options{
					Mode:              EnvShare,
					VirtualizationNum: 2,
				},
				deviceList: &deviceList{
					hasCtrlDev:  true,
					hasCommuDev: true,
				},
			},
			req: req,
		},
	}

	for i, tt := range tests {
		t.Run("", func(t *testing.T) {
			ctx := context.TODO()
			resp, err := tt.m.Allocate(ctx, tt.req)
			assert.NoError(t, err)
			assert.Equal(t, 1, len(resp.ContainerResponses))
			devices := resp.ContainerResponses[0].Devices
			assert.Equal(t, mluMonitorDeviceName, devices[0].HostPath)
			switch i {
			case 0:
				assert.Equal(t, 7, len(devices))
				assert.Equal(t, mluMsgqDeviceName+":0", devices[1].HostPath)
				assert.Equal(t, mluRPCDeviceName+":0", devices[2].HostPath)
				assert.Equal(t, mluCmsgDeviceName+"0", devices[3].HostPath)
				assert.Equal(t, mluCommuDeviceName+"0", devices[4].HostPath)
				assert.Equal(t, mluIpcmDeviceName+"0", devices[5].HostPath)
				assert.Equal(t, mluDeviceName+"0", devices[6].HostPath)
			case 1:
				assert.Equal(t, 3, len(devices))
				assert.Equal(t, mluCommuDeviceName+"0", devices[1].HostPath)
				assert.Equal(t, mluDeviceName+"0", devices[2].HostPath)
			}
		})
	}
}

func TestCambriconDevicePluginAllocateMim(t *testing.T) {
	devs := []*pluginapi.Device{
		{
			ID: "MLU-testdevice-sn0001",
		},
		{
			ID: "MLU-testdevice-sn0002",
		},
	}
	devsInfo := map[string]*cndev.Device{
		"MLU-testdevice-sn0001": {
			UUID:    "MLU-testdevice-sn0001",
			Path:    "/dev/cambricon_dev0,/dev/cambricon_ipcm0,/dev/cambricon-caps/cap_dev0_mi1",
			Profile: "2m.16gb",
		},
		"MLU-testdevice-sn0002": {
			UUID:    "MLU-testdevice-sn0002",
			Path:    "/dev/cambricon_dev0,/dev/cambricon_ipcm0,/dev/cambricon-caps/cap_dev0_mi2",
			Profile: "2m.16gb",
		},
	}
	m := &CambriconDevicePlugin{
		devs:     devs,
		devsInfo: devsInfo,
		options: Options{
			Mode: Mim,
		},
		deviceList: &deviceList{
			hasCtrlDev: true,
			hasIpcmDev: true,
		},
		profile: "2m.16gb",
	}
	req := &pluginapi.AllocateRequest{
		ContainerRequests: []*pluginapi.ContainerAllocateRequest{
			{
				DevicesIDs: []string{
					"MLU-testdevice-sn0001",
					"MLU-testdevice-sn0002",
				},
			},
		},
	}

	t.Run("", func(t *testing.T) {
		ctx := context.TODO()
		resp, err := m.Allocate(ctx, req)
		assert.NoError(t, err)
		assert.Equal(t, 1, len(resp.ContainerResponses))
		devices := resp.ContainerResponses[0].Devices
		assert.Equal(t, 5, len(devices))
		assert.Equal(t, mluMonitorDeviceName, devices[0].HostPath)
		assert.Equal(t, mluDeviceName+"0", devices[1].HostPath)
		assert.Equal(t, mluIpcmDeviceName+"0", devices[2].HostPath)
		assert.Equal(t, "/dev/cambricon-caps/cap_dev0_mi1", devices[3].HostPath)
		assert.Equal(t, "/dev/cambricon-caps/cap_dev0_mi2", devices[4].HostPath)
	})
}

func TestCambriconDevicePluginAllocateDsmlu(t *testing.T) {
	node := &v1.Node{
		ObjectMeta: metav1.ObjectMeta{
			Name:        "testnode",
			Annotations: map[string]string{DsmluLockTime: "1111"},
		},
		Spec: v1.NodeSpec{},
	}

	pod := &v1.Pod{
		ObjectMeta: metav1.ObjectMeta{
			Name:      "test-pod",
			Namespace: "ns",
			Annotations: map[string]string{
				DsmluResourceAssigned: "false",
				DsmluProfile:          "0_1_1",
			},
		},
		Spec: v1.PodSpec{
			NodeName: "testnode",
			Containers: []v1.Container{
				{
					Name:  "test-container1",
					Image: "test-image1",
					Resources: v1.ResourceRequirements{
						Limits: v1.ResourceList{
							v1.ResourceName("cambricon.com/mlu.vcore"):   *resource.NewQuantity(1, resource.DecimalSI),
							v1.ResourceName("cambricon.com/mlu.vmemory"): *resource.NewQuantity(1, resource.DecimalSI),
						},
					},
				},
			},
		},
		Status: v1.PodStatus{
			Phase: v1.PodPending,
		},
	}

	fakeClient := fake.NewSimpleClientset(node, pod)

	devs := []*pluginapi.Device{
		{
			ID: "sn0001-vcore-1",
		},
		{
			ID: "sn0001-vmemory-1",
		},
	}
	devsInfo := map[string]*cndev.Device{
		"sn0001-vcore-1": {
			UUID:    "MLU-testdevice-sn0001",
			Path:    "/dev/cambricon_dev0",
			Profile: "vcore",
		},
		"sn0001-vmemory-1": {
			UUID:    "MLU-testdevice-sn0002",
			Path:    "/dev/cambricon_dev0",
			Profile: "vmemory",
		},
	}
	ms := []*CambriconDevicePlugin{
		{
			devs:     devs,
			devsInfo: devsInfo,
			options: Options{
				Mode: DynamicSmlu,
			},
			deviceList: &deviceList{
				hasCtrlDev: true,
				hasIpcmDev: true,
			},
			profile:   "vcore",
			clientset: fakeClient,
		},
		{
			devs:     devs,
			devsInfo: devsInfo,
			options: Options{
				Mode: DynamicSmlu,
			},
			deviceList: &deviceList{
				hasCtrlDev: true,
				hasIpcmDev: true,
			},
			profile:      "vmemory",
			clientset:    fakeClient,
			nodeHostname: "testnode",
		},
	}
	reqs := []*pluginapi.AllocateRequest{
		{
			ContainerRequests: []*pluginapi.ContainerAllocateRequest{
				{
					DevicesIDs: []string{
						"sn0001-vcore-1",
					},
				},
			},
		},
		{
			ContainerRequests: []*pluginapi.ContainerAllocateRequest{
				{
					DevicesIDs: []string{
						"sn0001-vmemory-1",
					},
				},
			},
		},
	}

	t.Run("", func(t *testing.T) {
		ctx := context.TODO()
		for i, req := range reqs {
			resp, err := ms[i].Allocate(ctx, req)
			assert.NoError(t, err)
			assert.Equal(t, 1, len(resp.ContainerResponses))
			devices := resp.ContainerResponses[0].Devices
			assert.Equal(t, 4, len(devices))
			assert.Equal(t, mluMonitorDeviceName, devices[0].HostPath)
			assert.Equal(t, mluDeviceName+"0", devices[1].HostPath)
			assert.Equal(t, mluIpcmDeviceName+"0", devices[2].HostPath)
			assert.Equal(t, "/dev/cambricon-caps/cap_dev0_mi1", devices[3].HostPath)
		}
		pod, err := fakeClient.CoreV1().Pods(pod.Namespace).Get(ctx, "test-pod", metav1.GetOptions{})
		assert.NoError(t, err)
		assert.Equal(t, pod.Annotations[DsmluResourceAssigned], "true")
		assert.Equal(t, pod.Annotations[DsmluProfileAndInstance], "0_256_0_1")
	})
}
