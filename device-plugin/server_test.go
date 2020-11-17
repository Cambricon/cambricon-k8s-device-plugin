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
	"testing"

	"github.com/cambricon/cambricon-k8s-device-plugin/device-plugin/pkg/cndev"
	"github.com/stretchr/testify/assert"
	"golang.org/x/net/context"
	pluginapi "k8s.io/kubelet/pkg/apis/deviceplugin/v1beta1"
)

func TestCambriconDevicePluginAllocateMLU100(t *testing.T) {
	devs := []*pluginapi.Device{
		{
			ID: "MLU-testdevice-sn0001",
		},
		{
			ID: "MLU-testdevice-sn0002",
		},
	}
	devsInfo := []*cndev.Device{
		{
			UUID: "MLU-testdevice-sn0001",
			Path: "/dev/cambricon_c10Dev0",
		},
		{
			UUID: "MLU-testdevice-sn0002",
			Path: "/dev/cambricon_c10Dev1",
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
				feature:  0,
				deviceList: &deviceList{
					hasCnmonDev: true,
					hasCodecDev: true,
					hasC10Dev:   true,
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
		{
			m: &CambriconDevicePlugin{
				devs:     devs,
				devsInfo: devsInfo,
				feature:  0,
				deviceList: &deviceList{
					hasCnmonDev: true,
					hasC10Dev:   true,
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
		{
			m: &CambriconDevicePlugin{
				devs:     devs,
				devsInfo: devsInfo,
				feature:  0,
				deviceList: &deviceList{
					hasCnmonDev: true,
					hasC10Dev:   true,
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
	}

	for i, tt := range tests {
		t.Run("", func(t *testing.T) {
			ctx := context.TODO()
			resp, err := tt.m.Allocate(ctx, tt.req)
			assert.NoError(t, err)
			assert.Equal(t, 1, len(resp.ContainerResponses))
			devices := resp.ContainerResponses[0].Devices
			assert.Equal(t, mlu100MonitorDeviceName, devices[0].HostPath)
			switch i {
			case 0:
				assert.Equal(t, 3, len(devices))
				assert.Equal(t, mlu100CodecDeviceName, devices[1].HostPath)
				assert.Equal(t, mlu100DeviceName+"1", devices[2].HostPath)
			case 1:
				assert.Equal(t, 2, len(devices))
				assert.Equal(t, mlu100DeviceName+"1", devices[1].HostPath)
			case 3:
				assert.Equal(t, 3, len(devices))
				assert.Equal(t, mlu100DeviceName+"0", devices[1].HostPath)
				assert.Equal(t, mlu100DeviceName+"1", devices[2].HostPath)
			}
		})
	}
}

func TestCambriconDevicePluginAllocate(t *testing.T) {
	devs := []*pluginapi.Device{
		{
			ID: "MLU-testdevice-sn0001",
		},
		{
			ID: "MLU-testdevice-sn0002",
		},
	}
	devsInfo := []*cndev.Device{
		{
			UUID: "MLU-testdevice-sn0001",
			Path: "/dev/cambricon_dev0",
		},
		{
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
				feature:  0,
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
				feature:  0,
				deviceList: &deviceList{
					hasCtrlDev:  true,
					hasCommuDev: true,
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
				assert.Equal(t, 3, len(devices))
				assert.Equal(t, mluMonitorDeviceName, devices[0].HostPath)
				assert.Equal(t, mluCommuDeviceName+"1", devices[1].HostPath)
				assert.Equal(t, mluDeviceName+"1", devices[2].HostPath)
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
	devsInfo := []*cndev.Device{
		{
			UUID: "MLU-testdevice-sn0001--fake--1",
			Path: "/dev/cambricon_dev0vf1",
		},
		{
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
				feature:  sriovShare,
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
				feature:  sriovShare,
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
	devsInfo := []*cndev.Device{
		{
			UUID: "MLU-testdevice-sn0001-_-1",
			Path: "/dev/cambricon_dev0",
		},
		{
			UUID: "MLU-testdevice-sn0001-_-2",
			Path: "/dev/cambricon_dev0",
		},
	}
	req := &pluginapi.AllocateRequest{
		ContainerRequests: []*pluginapi.ContainerAllocateRequest{
			{
				DevicesIDs: []string{
					"MLU-testdevice-sn0001-_-1",
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
				feature:  envShare,
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
				feature:  envShare,
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
