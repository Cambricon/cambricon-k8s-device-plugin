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
	"sort"
	"testing"

	"github.com/Cambricon/cambricon-k8s-device-plugin/device-plugin/pkg/cndev"
	"github.com/stretchr/testify/assert"
	v1 "k8s.io/api/core/v1"
	metav1 "k8s.io/apimachinery/pkg/apis/meta/v1"
	fakeclient "k8s.io/client-go/kubernetes/fake"
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
				assert.Equal(t, 11, len(devices))
				assert.Equal(t, mluMonitorDeviceName, devices[0].HostPath)
				assert.Equal(t, mluMsgqDeviceName+":0", devices[1].HostPath)
				assert.Equal(t, mluRPCDeviceName+":0", devices[2].HostPath)
				assert.Equal(t, mluCmsgDeviceName+"0", devices[3].HostPath)
				assert.Equal(t, mluCommuDeviceName+"0", devices[4].HostPath)
				assert.Equal(t, mluDeviceName+"0", devices[5].HostPath)
				assert.Equal(t, mluMsgqDeviceName+":1", devices[6].HostPath)
				assert.Equal(t, mluRPCDeviceName+":1", devices[7].HostPath)
				assert.Equal(t, mluCmsgDeviceName+"1", devices[8].HostPath)
				assert.Equal(t, mluCommuDeviceName+"1", devices[9].HostPath)
				assert.Equal(t, mluDeviceName+"1", devices[10].HostPath)
			case 1:
				assert.Equal(t, 4, len(devices))
				assert.Equal(t, mluMonitorDeviceName, devices[0].HostPath)
				assert.Equal(t, mluCommuDeviceName+"1", devices[1].HostPath)
				assert.Equal(t, mluUARTConsoleDeviceName+"1", devices[2].HostPath)
				assert.Equal(t, mluDeviceName+"1", devices[3].HostPath)
			}
		})
	}
}

func TestCambriconDevicePluginAllocateSriov(t *testing.T) {
	devs := []*pluginapi.Device{
		{
			ID: "MLU-testdevice-sn0001--fake--1",
		},
		{
			ID: "MLU-testdevice-sn0001--fake--2",
		},
	}
	devsInfo := map[string]*cndev.Device{
		"MLU-testdevice-sn0001--fake--1": {
			UUID: "MLU-testdevice-sn0001--fake--1",
			Path: "/dev/cambricon_dev0vf1",
		},
		"MLU-testdevice-sn0001--fake--2": {
			UUID: "MLU-testdevice-sn0001--fake--2",
			Path: "/dev/cambricon_dev0vf2",
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
					Mode:              sriov,
					VirtualizationNum: 2,
				},
				deviceList: &deviceList{
					hasCtrlDev:  true,
					hasMsgqDev:  true,
					hasRPCDev:   true,
					hasCmsgDev:  true,
					hasCommuDev: true,
				},
			},
			req: &pluginapi.AllocateRequest{
				ContainerRequests: []*pluginapi.ContainerAllocateRequest{
					{
						DevicesIDs: []string{
							"MLU-testdevice-sn0001--fake--1",
							"MLU-testdevice-sn0001--fake--2",
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
					Mode:              sriov,
					VirtualizationNum: 2,
				},
				deviceList: &deviceList{
					hasCtrlDev:  true,
					hasCommuDev: true,
				},
			},
			req: &pluginapi.AllocateRequest{
				ContainerRequests: []*pluginapi.ContainerAllocateRequest{
					{
						DevicesIDs: []string{
							"MLU-testdevice-sn0001--fake--2",
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
				assert.Equal(t, 5, len(devices))
				assert.Equal(t, mluMonitorDeviceName, devices[0].HostPath)
				assert.Equal(t, mluCommuDeviceName+"0vf1", devices[1].HostPath)
				assert.Equal(t, mluDeviceName+"0vf1", devices[2].HostPath)
				assert.Equal(t, mluCommuDeviceName+"0vf2", devices[3].HostPath)
				assert.Equal(t, mluDeviceName+"0vf2", devices[4].HostPath)
			case 1:
				assert.Equal(t, 3, len(devices))
				assert.Equal(t, mluMonitorDeviceName, devices[0].HostPath)
				assert.Equal(t, mluCommuDeviceName+"0vf2", devices[1].HostPath)
				assert.Equal(t, mluDeviceName+"0vf2", devices[2].HostPath)
			}
		})
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
					Mode:              envShare,
					VirtualizationNum: 2,
				},
				deviceList: &deviceList{
					hasCtrlDev:  true,
					hasMsgqDev:  true,
					hasRPCDev:   true,
					hasCmsgDev:  true,
					hasCommuDev: true,
				},
			},
			req: req,
		},
		{
			m: &CambriconDevicePlugin{
				devs:     devs,
				devsInfo: devsInfo,
				options: Options{
					Mode:              envShare,
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
				assert.Equal(t, 6, len(devices))
				assert.Equal(t, mluMonitorDeviceName, devices[0].HostPath)
				assert.Equal(t, mluMsgqDeviceName+":0", devices[1].HostPath)
				assert.Equal(t, mluRPCDeviceName+":0", devices[2].HostPath)
				assert.Equal(t, mluCmsgDeviceName+"0", devices[3].HostPath)
				assert.Equal(t, mluCommuDeviceName+"0", devices[4].HostPath)
				assert.Equal(t, mluDeviceName+"0", devices[5].HostPath)

			case 1:
				assert.Equal(t, 3, len(devices))
				assert.Equal(t, mluMonitorDeviceName, devices[0].HostPath)
				assert.Equal(t, mluCommuDeviceName+"0", devices[1].HostPath)
				assert.Equal(t, mluDeviceName+"0", devices[2].HostPath)
			}
		})
	}
}

func TestConstructTopologyMap(t *testing.T) {
	tests := []struct {
		m *CambriconDevicePlugin
	}{
		{
			m: &CambriconDevicePlugin{
				options: Options{
					Mode: topologyAware,
				},
				devsInfo: map[string]*cndev.Device{
					"MLU-1": {
						UUID:        "MLU-1",
						MotherBoard: "mb-0",
						MLULinkDevs: map[string]int{
							"MLU-0":            1,
							"MLU-2":            1,
							"MLU-3":            2,
							"MLU-5":            1,
							"another-machine2": 1,
						},
					},
					"MLU-6": {
						UUID:        "MLU-6",
						MotherBoard: "mb-1",
						MLULinkDevs: map[string]int{
							"MLU-2":            1,
							"MLU-4":            2,
							"MLU-5":            1,
							"MLU-7":            1,
							"another-machine7": 1,
						},
					},
					"MLU-0": {
						UUID:        "MLU-0",
						MotherBoard: "mb-0",
						MLULinkDevs: map[string]int{
							"MLU-1":            1,
							"MLU-2":            2,
							"MLU-3":            1,
							"MLU-4":            1,
							"another-machine1": 1,
						},
					},
					"MLU-4": {
						UUID:        "MLU-4",
						MotherBoard: "mb-1",
						MLULinkDevs: map[string]int{
							"MLU-0":            1,
							"MLU-5":            1,
							"MLU-6":            2,
							"MLU-7":            1,
							"another-machine5": 1,
						},
					},
					"MLU-7": {
						UUID:        "MLU-7",
						MotherBoard: "mb-1",
						MLULinkDevs: map[string]int{
							"MLU-3":            1,
							"MLU-4":            1,
							"MLU-5":            2,
							"MLU-6":            1,
							"another-machine8": 1,
						},
					},
					"MLU-2": {
						UUID:        "MLU-2",
						MotherBoard: "mb-0",
						MLULinkDevs: map[string]int{
							"MLU-0":            2,
							"MLU-1":            1,
							"MLU-3":            1,
							"MLU-6":            1,
							"another-machine3": 1,
						},
					},
					"MLU-5": {
						UUID:        "MLU-5",
						MotherBoard: "mb-1",
						MLULinkDevs: map[string]int{
							"MLU-1":            1,
							"MLU-4":            1,
							"MLU-6":            1,
							"MLU-7":            2,
							"another-machine6": 1,
						},
					},
					"MLU-3": {
						UUID:        "MLU-3",
						MotherBoard: "mb-0",
						MLULinkDevs: map[string]int{
							"MLU-0":            1,
							"MLU-1":            2,
							"MLU-2":            1,
							"MLU-7":            1,
							"another-machine4": 1,
						},
					},
				},
			},
		},
		{
			m: &CambriconDevicePlugin{
				options: Options{
					Mode: topologyAware,
				},
				devsInfo: map[string]*cndev.Device{
					"MLU-1": {
						UUID:        "MLU-1",
						MotherBoard: "mb-0",
						MLULinkDevs: map[string]int{
							"MLU-0": 1,
							"MLU-2": 1,
							"MLU-3": 2,
							"MLU-5": 2,
						},
					},
					"MLU-6": {
						UUID:        "MLU-6",
						MotherBoard: "mb-1",
						MLULinkDevs: map[string]int{
							"MLU-2": 2,
							"MLU-4": 2,
							"MLU-5": 1,
							"MLU-7": 1,
						},
					},
					"MLU-0": {
						UUID:        "MLU-0",
						MotherBoard: "mb-0",
						MLULinkDevs: map[string]int{
							"MLU-1": 1,
							"MLU-2": 2,
							"MLU-3": 1,
							"MLU-4": 2,
						},
					},
					"MLU-4": {
						UUID:        "MLU-4",
						MotherBoard: "mb-1",
						MLULinkDevs: map[string]int{
							"MLU-0": 2,
							"MLU-5": 1,
							"MLU-6": 2,
							"MLU-7": 1,
						},
					},
					"MLU-7": {
						UUID:        "MLU-7",
						MotherBoard: "mb-1",
						MLULinkDevs: map[string]int{
							"MLU-3": 2,
							"MLU-4": 1,
							"MLU-5": 2,
							"MLU-6": 1,
						},
					},
					"MLU-2": {
						UUID:        "MLU-2",
						MotherBoard: "mb-0",
						MLULinkDevs: map[string]int{
							"MLU-0": 2,
							"MLU-1": 1,
							"MLU-3": 1,
							"MLU-6": 2,
						},
					},
					"MLU-5": {
						UUID:        "MLU-5",
						MotherBoard: "mb-1",
						MLULinkDevs: map[string]int{
							"MLU-1": 2,
							"MLU-4": 1,
							"MLU-6": 1,
							"MLU-7": 2,
						},
					},
					"MLU-3": {
						UUID:        "MLU-3",
						MotherBoard: "mb-0",
						MLULinkDevs: map[string]int{
							"MLU-0": 1,
							"MLU-1": 2,
							"MLU-2": 1,
							"MLU-7": 2,
						},
					},
				},
			},
		},
		{
			m: &CambriconDevicePlugin{
				options: Options{
					Mode: topologyAware,
				},
				devsInfo: map[string]*cndev.Device{
					"MLU-0": {
						UUID:        "MLU-0",
						MotherBoard: "mb-0",
						MLULinkDevs: map[string]int{
							"MLU-1": 1,
							"MLU-2": 2,
							"MLU-3": 1,
							"MLU-4": 2,
						},
					},
					"MLU-4": {
						UUID:        "MLU-4",
						MotherBoard: "mb-1",
						MLULinkDevs: map[string]int{
							"MLU-0": 2,
							"MLU-5": 1,
							"MLU-6": 2,
							"MLU-7": 1,
						},
					},
					"MLU-7": {
						UUID:        "MLU-7",
						MotherBoard: "mb-1",
						MLULinkDevs: map[string]int{
							"MLU-3": 2,
							"MLU-4": 1,
							"MLU-5": 2,
							"MLU-6": 1,
						},
					},
				},
			},
		},
	}
	for i, tt := range tests {
		t.Run(fmt.Sprintf("test %d", i), func(t *testing.T) {
			defer func() {
				if err := recover(); err != nil {
					assert.Equal(t, err, fmt.Errorf("the number of known devices is %d, should be 8", len(tt.m.devsInfo)))
				}
			}()
			tt.m.constructTopologyMap()
			assert.Equal(t, tt.m.topology, map[int]string{
				0: "MLU-0",
				1: "MLU-1",
				2: "MLU-2",
				3: "MLU-3",
				4: "MLU-4",
				5: "MLU-5",
				6: "MLU-6",
				7: "MLU-7",
			})
		})
	}
}

func TestCambriconDevicePluginGetPreferredAllocatedDeviceUUIDs(t *testing.T) {
	plugins := []*CambriconDevicePlugin{
		{
			options: Options{
				Mode: topologyAware,
			},
			topology: map[int]string{
				0: "MLU-0",
				1: "MLU-1",
				2: "MLU-2",
				3: "MLU-3",
				4: "MLU-4",
				5: "MLU-5",
				6: "MLU-6",
				7: "MLU-7",
			},
			devsInfo: map[string]*cndev.Device{
				"MLU-0": {
					UUID:        "MLU-0",
					Slot:        0,
					MotherBoard: "mb-0",
					MLULinkDevs: map[string]int{
						"MLU-1":            1,
						"MLU-2":            2,
						"MLU-3":            1,
						"MLU-4":            1,
						"another-machine1": 1,
					},
				},
				"MLU-1": {
					UUID:        "MLU-1",
					Slot:        1,
					MotherBoard: "mb-0",
					MLULinkDevs: map[string]int{
						"MLU-0":            1,
						"MLU-2":            1,
						"MLU-3":            2,
						"MLU-5":            1,
						"another-machine2": 1,
					},
				},
				"MLU-2": {
					UUID:        "MLU-2",
					Slot:        2,
					MotherBoard: "mb-0",
					MLULinkDevs: map[string]int{
						"MLU-0":            2,
						"MLU-1":            1,
						"MLU-3":            1,
						"MLU-6":            1,
						"another-machine3": 1,
					},
				},
				"MLU-3": {
					UUID:        "MLU-3",
					Slot:        3,
					MotherBoard: "mb-0",
					MLULinkDevs: map[string]int{
						"MLU-0":            1,
						"MLU-1":            2,
						"MLU-2":            1,
						"MLU-7":            1,
						"another-machine4": 1,
					},
				},
				"MLU-4": {
					UUID:        "MLU-4",
					Slot:        4,
					MotherBoard: "mb-1",
					MLULinkDevs: map[string]int{
						"MLU-0":            1,
						"MLU-5":            1,
						"MLU-6":            2,
						"MLU-7":            1,
						"another-machine5": 1,
					},
				},
				"MLU-5": {
					UUID:        "MLU-5",
					Slot:        5,
					MotherBoard: "mb-1",
					MLULinkDevs: map[string]int{
						"MLU-1":            1,
						"MLU-4":            1,
						"MLU-6":            1,
						"MLU-7":            2,
						"another-machine6": 1,
					},
				},
				"MLU-6": {
					UUID:        "MLU-6",
					Slot:        6,
					MotherBoard: "mb-1",
					MLULinkDevs: map[string]int{
						"MLU-2":            1,
						"MLU-4":            2,
						"MLU-5":            1,
						"MLU-7":            1,
						"another-machine7": 1,
					},
				},
				"MLU-7": {
					UUID:        "MLU-7",
					Slot:        7,
					MotherBoard: "mb-1",
					MLULinkDevs: map[string]int{
						"MLU-3":            1,
						"MLU-4":            1,
						"MLU-5":            2,
						"MLU-6":            1,
						"another-machine8": 1,
					},
				},
			},
		},
		{
			options: Options{
				Mode: topologyAware,
			},
			topology: map[int]string{
				0: "MLU-0",
				1: "MLU-1",
				2: "MLU-2",
				3: "MLU-3",
				4: "MLU-4",
				5: "MLU-5",
				6: "MLU-6",
				7: "MLU-7",
			},
			devsInfo: map[string]*cndev.Device{
				"MLU-1": {
					UUID:        "MLU-1",
					Slot:        1,
					MotherBoard: "mb-0",
					MLULinkDevs: map[string]int{
						"MLU-0": 1,
						"MLU-2": 1,
						"MLU-3": 2,
						"MLU-5": 2,
					},
				},
				"MLU-6": {
					UUID:        "MLU-6",
					Slot:        6,
					MotherBoard: "mb-1",
					MLULinkDevs: map[string]int{
						"MLU-2": 2,
						"MLU-4": 2,
						"MLU-5": 1,
						"MLU-7": 1,
					},
				},
				"MLU-0": {
					UUID:        "MLU-0",
					Slot:        0,
					MotherBoard: "mb-0",
					MLULinkDevs: map[string]int{
						"MLU-1": 1,
						"MLU-2": 2,
						"MLU-3": 1,
						"MLU-4": 2,
					},
				},
				"MLU-4": {
					UUID:        "MLU-4",
					Slot:        4,
					MotherBoard: "mb-1",
					MLULinkDevs: map[string]int{
						"MLU-0": 2,
						"MLU-5": 1,
						"MLU-6": 2,
						"MLU-7": 1,
					},
				},
				"MLU-7": {
					UUID:        "MLU-7",
					Slot:        7,
					MotherBoard: "mb-1",
					MLULinkDevs: map[string]int{
						"MLU-3": 2,
						"MLU-4": 1,
						"MLU-5": 2,
						"MLU-6": 1,
					},
				},
				"MLU-2": {
					UUID:        "MLU-2",
					Slot:        2,
					MotherBoard: "mb-0",
					MLULinkDevs: map[string]int{
						"MLU-0": 2,
						"MLU-1": 1,
						"MLU-3": 1,
						"MLU-6": 2,
					},
				},
				"MLU-5": {
					UUID:        "MLU-5",
					Slot:        5,
					MotherBoard: "mb-1",
					MLULinkDevs: map[string]int{
						"MLU-1": 2,
						"MLU-4": 1,
						"MLU-6": 1,
						"MLU-7": 2,
					},
				},
				"MLU-3": {
					UUID:        "MLU-3",
					Slot:        3,
					MotherBoard: "mb-0",
					MLULinkDevs: map[string]int{
						"MLU-0": 1,
						"MLU-1": 2,
						"MLU-2": 1,
						"MLU-7": 2,
					},
				},
			},
		},
	}
	tests := map[string][]struct {
		available []string
		size      int
		out       []string
	}{
		bestEffort: {
			{
				available: []string{"MLU-0", "MLU-1", "MLU-5"},
				size:      1,
				out:       []string{"MLU-5"},
			},
			{
				available: []string{"MLU-0", "MLU-1", "MLU-4"},
				size:      2,
				out:       []string{"MLU-0", "MLU-1"},
			},
			{
				available: []string{"MLU-0", "MLU-1", "MLU-2", "MLU-3", "MLU-4", "MLU-6"},
				size:      2,
				out:       []string{"MLU-4", "MLU-6"},
			},
			{
				available: []string{"MLU-0", "MLU-1", "MLU-4", "MLU-6"},
				size:      2,
				out:       []string{"MLU-4", "MLU-6"},
			},
			{
				available: []string{"MLU-0", "MLU-5"},
				size:      2,
				out:       []string{"MLU-0", "MLU-5"},
			},
			{
				available: []string{"MLU-0", "MLU-1", "MLU-4", "MLU-2"},
				size:      3,
				out:       []string{"MLU-0", "MLU-1", "MLU-2"},
			},
			{
				available: []string{"MLU-0", "MLU-4", "MLU-5", "MLU-1", "MLU-6", "MLU-7", "MLU-3"},
				size:      4,
				out:       []string{"MLU-4", "MLU-5", "MLU-6", "MLU-7"},
			},
			{
				available: []string{"MLU-4", "MLU-0", "MLU-3", "MLU-5", "MLU-6"},
				size:      4,
				out:       []string{"MLU-0", "MLU-4", "MLU-5", "MLU-6"},
			},
			{
				available: []string{"MLU-4", "MLU-0", "MLU-3", "MLU-7", "MLU-2"},
				size:      4,
				out:       []string{"MLU-0", "MLU-3", "MLU-4", "MLU-7"},
			},
			{
				available: []string{"MLU-1", "MLU-0", "MLU-3", "MLU-6"},
				size:      4,
				out:       []string{"MLU-0", "MLU-1", "MLU-3", "MLU-6"},
			},
			{
				available: []string{"MLU-1", "MLU-5", "MLU-3", "MLU-7", "MLU-0", "MLU-4"},
				size:      4,
				out:       []string{"MLU-1", "MLU-3", "MLU-5", "MLU-7"},
			},
		},
		guaranteed: {
			{
				available: []string{"MLU-1", "MLU-5", "MLU-3", "MLU-7", "MLU-0", "MLU-4"},
				size:      4,
				out:       []string{"MLU-1", "MLU-3", "MLU-5", "MLU-7"},
			},
			{
				available: []string{"MLU-1", "MLU-0", "MLU-3", "MLU-6"},
				size:      4,
			},
			{
				available: []string{"MLU-0", "MLU-1", "MLU-4", "MLU-2"},
				size:      3,
				out:       []string{"MLU-0", "MLU-1", "MLU-2"},
			},
			{
				available: []string{"MLU-0", "MLU-1", "MLU-4"},
				size:      2,
				out:       []string{"MLU-0", "MLU-1"},
			},
			{
				available: []string{"MLU-0", "MLU-5"},
				size:      2,
			},
			{
				available: []string{"MLU-0", "MLU-1", "MLU-2", "MLU-3", "MLU-4", "MLU-6"},
				size:      2,
				out:       []string{"MLU-4", "MLU-6"},
			},
			{
				available: []string{"MLU-0", "MLU-1", "MLU-4", "MLU-6"},
				size:      2,
				out:       []string{"MLU-4", "MLU-6"},
			},
		},
		restricted: {
			{
				available: []string{"MLU-0", "MLU-1", "MLU-2"},
				size:      2,
				out:       []string{"MLU-0", "MLU-2"},
			},
			{
				available: []string{"MLU-0", "MLU-4", "MLU-3"},
				size:      2,
			},
			{
				available: []string{"MLU-0", "MLU-1", "MLU-2", "MLU-3", "MLU-4", "MLU-6"},
				size:      2,
				out:       []string{"MLU-4", "MLU-6"},
			},
			{
				available: []string{"MLU-0", "MLU-2", "MLU-4", "MLU-5"},
				size:      2,
				out:       []string{"MLU-0", "MLU-2"},
			},
			{
				available: []string{"MLU-0", "MLU-1", "MLU-4", "MLU-2"},
				size:      3,
				out:       []string{"MLU-0", "MLU-1", "MLU-2"},
			},
			{
				available: []string{"MLU-0", "MLU-4", "MLU-5", "MLU-1", "MLU-6", "MLU-7", "MLU-3"},
				size:      4,
				out:       []string{"MLU-4", "MLU-5", "MLU-6", "MLU-7"},
			},
			{
				available: []string{"MLU-4", "MLU-0", "MLU-3", "MLU-7", "MLU-2"},
				size:      4,
			},
			{
				available: []string{"MLU-0", "MLU-2", "MLU-4", "MLU-6"},
				size:      4,
			},
		},
	}
	for i, plugin := range plugins {
		plugin.nodeHostname = "fakeNode"
		plugin.clientset = fakeclient.NewSimpleClientset(
			&v1.Node{
				ObjectMeta: metav1.ObjectMeta{
					Name: plugin.nodeHostname,
				},
				Spec: v1.NodeSpec{},
			},
		)
		for policy, cases := range tests {
			plugin.options.MLULinkPolicy = policy
			for index, test := range cases {
				t.Run(fmt.Sprintf("plugin %d, policy %s, test: %d", i, policy, index), func(t *testing.T) {
					res, err := plugin.GetPreferredAllocatedDeviceUUIDs(test.available, nil, test.size)
					sort.Slice(res, func(i int, j int) bool {
						return res[i] < res[j]
					})
					node, _ := plugin.clientset.CoreV1().Nodes().Get(context.TODO(), plugin.nodeHostname, metav1.GetOptions{})
					_, ok := node.Annotations[mluLinkPolicyUnsatisfied]
					if test.out != nil {
						assert.Equal(t, test.out, res)
						assert.Equal(t, false, ok)
					} else {
						assert.Equal(t, errMLULinkPolicyUnsatisfied, errors.Unwrap(err))
						assert.Equal(t, true, ok)
						delete(node.Annotations, mluLinkPolicyUnsatisfied)
						plugin.clientset.CoreV1().Nodes().Update(context.TODO(), node, metav1.UpdateOptions{})
					}
				})
			}
		}
	}
}
