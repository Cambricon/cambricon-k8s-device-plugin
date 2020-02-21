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
	"github.com/cambricon/cambricon-k8s-device-plugin/pkg/cndev"
	"github.com/stretchr/testify/assert"
	"golang.org/x/net/context"
	pluginapi "k8s.io/kubernetes/pkg/kubelet/apis/deviceplugin/v1beta1"
	"testing"
)

func TestCambriconDevicePlugin_Allocate(t *testing.T) {
	m := &CambriconDevicePlugin{
		devsMap: []*cndev.Device{
			{
				UUID: "MLU-testdevice-sn0001",
				Path: "/dev/cambricon_c10Dev0",
			},
			{
				UUID: "MLU-testdevice-sn0002",
				Path: "/dev/cambricon_c10Dev1",
			},
		},
		devs: []*pluginapi.Device{
			{
				ID: "MLU-testdevice-sn0001",
			},
			{
				ID: "MLU-testdevice-sn0002",
			},
		},
	}
	ctx := context.TODO()
	req := &pluginapi.AllocateRequest{
		ContainerRequests: []*pluginapi.ContainerAllocateRequest{
			&pluginapi.ContainerAllocateRequest{
				DevicesIDs: []string{
					"MLU-testdevice-sn0001",
				},
			},
		},
	}

	rsp, err := m.Allocate(ctx, req)
	assert.NoError(t, err)
	assert.Equal(t, 1, len(rsp.ContainerResponses))

	devices := rsp.ContainerResponses[0].Devices
	result := false
	if len(devices) == 2 || len(devices) == 3 {
		result = true
	}
	assert.True(t, result)

	assert.Equal(t, mlu100MonitorDeviceName, devices[0].HostPath)
	if len(devices) == 2 {
		assert.Equal(t, mlu100DeviceName+"0", devices[1].HostPath)
	} else {
		assert.Equal(t, mlu100CodecDeviceName, devices[1].HostPath)
		assert.Equal(t, mlu100DeviceName+"0", devices[2].HostPath)
	}

	ctx = context.TODO()
	req = &pluginapi.AllocateRequest{
		ContainerRequests: []*pluginapi.ContainerAllocateRequest{
			&pluginapi.ContainerAllocateRequest{
				DevicesIDs: []string{
					"MLU-testdevice-sn0001",
					"MLU-testdevice-sn0002",
				},
			},
		},
	}

	rsp, err = m.Allocate(ctx, req)
	assert.NoError(t, err)
	assert.Equal(t, 1, len(rsp.ContainerResponses))

	devices = rsp.ContainerResponses[0].Devices
	result = false
	if len(devices) == 3 || len(devices) == 4 {
		result = true
	}
	assert.True(t, result)

	assert.Equal(t, mlu100MonitorDeviceName, devices[0].HostPath)
	if len(devices) == 3 {
		assert.Equal(t, mlu100DeviceName+"0", devices[1].HostPath)
		assert.Equal(t, mlu100DeviceName+"1", devices[2].HostPath)
	} else {
		assert.Equal(t, mlu100CodecDeviceName, devices[1].HostPath)
		assert.Equal(t, mlu100DeviceName+"0", devices[2].HostPath)
		assert.Equal(t, mlu100DeviceName+"1", devices[3].HostPath)
	}

	m = &CambriconDevicePlugin{
		devsMap: []*cndev.Device{
			{
				UUID: "MLU-testdevice-sn0001",
				Path: "/dev/cambricon_dev0",
			},
			{
				UUID: "MLU-testdevice-sn0002",
				Path: "/dev/cambricon_dev1",
			},
		},
		devs: []*pluginapi.Device{
			{
				ID: "MLU-testdevice-sn0001",
			},
			{
				ID: "MLU-testdevice-sn0002",
			},
		},
	}

	ctx = context.TODO()
	req = &pluginapi.AllocateRequest{
		ContainerRequests: []*pluginapi.ContainerAllocateRequest{
			&pluginapi.ContainerAllocateRequest{
				DevicesIDs: []string{
					"MLU-testdevice-sn0002",
				},
			},
		},
	}

	rsp, err = m.Allocate(ctx, req)
	assert.NoError(t, err)
	assert.Equal(t, 1, len(rsp.ContainerResponses))
	assert.Equal(t, 6, len(rsp.ContainerResponses[0].Devices))

	assert.Equal(t, mlu270MonitorDeviceName, rsp.ContainerResponses[0].Devices[0].HostPath)
	assert.Equal(t, mlu270MSGQDeviceName+":1", rsp.ContainerResponses[0].Devices[1].HostPath)
	assert.Equal(t, mlu270RPCDeviceName+":1", rsp.ContainerResponses[0].Devices[2].HostPath)
	assert.Equal(t, mlu270CmsgDeviceName+"1", rsp.ContainerResponses[0].Devices[3].HostPath)
	assert.Equal(t, mlu270COMMUDeviceName+"1", rsp.ContainerResponses[0].Devices[4].HostPath)
	assert.Equal(t, mlu270DeviceName+"1", rsp.ContainerResponses[0].Devices[5].HostPath)

	req = &pluginapi.AllocateRequest{
		ContainerRequests: []*pluginapi.ContainerAllocateRequest{
			&pluginapi.ContainerAllocateRequest{
				DevicesIDs: []string{
					"MLU-testdevice-sn0002",
					"MLU-testdevice-sn0001",
				},
			},
		},
	}

	rsp, err = m.Allocate(ctx, req)
	assert.NoError(t, err)
	assert.Equal(t, 1, len(rsp.ContainerResponses))
	assert.Equal(t, 11, len(rsp.ContainerResponses[0].Devices))

	assert.Equal(t, mlu270MonitorDeviceName, rsp.ContainerResponses[0].Devices[0].HostPath)

	assert.Equal(t, mlu270MSGQDeviceName+":1", rsp.ContainerResponses[0].Devices[1].HostPath)
	assert.Equal(t, mlu270RPCDeviceName+":1", rsp.ContainerResponses[0].Devices[2].HostPath)
	assert.Equal(t, mlu270CmsgDeviceName+"1", rsp.ContainerResponses[0].Devices[3].HostPath)
	assert.Equal(t, mlu270COMMUDeviceName+"1", rsp.ContainerResponses[0].Devices[4].HostPath)
	assert.Equal(t, mlu270DeviceName+"1", rsp.ContainerResponses[0].Devices[5].HostPath)

	assert.Equal(t, mlu270MSGQDeviceName+":0", rsp.ContainerResponses[0].Devices[6].HostPath)
	assert.Equal(t, mlu270RPCDeviceName+":0", rsp.ContainerResponses[0].Devices[7].HostPath)
	assert.Equal(t, mlu270CmsgDeviceName+"0", rsp.ContainerResponses[0].Devices[8].HostPath)
	assert.Equal(t, mlu270COMMUDeviceName+"0", rsp.ContainerResponses[0].Devices[9].HostPath)
	assert.Equal(t, mlu270DeviceName+"0", rsp.ContainerResponses[0].Devices[10].HostPath)
}
