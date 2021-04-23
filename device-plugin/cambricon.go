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
	"path/filepath"
	"time"

	"github.com/Cambricon/cambricon-k8s-device-plugin/device-plugin/pkg/cndev"
	pluginapi "k8s.io/kubelet/pkg/apis/deviceplugin/v1beta1"
)

const (
	mlu100MonitorDeviceName = "/dev/cnmon_dev"
	mlu100CodecDeviceName   = "/dev/cncodec_dev"
	mlu100DeviceName        = "/dev/cambricon_c10Dev"

	mluMonitorDeviceName     = "/dev/cambricon_ctl"
	mluMsgqDeviceName        = "/dev/cambr-msgq"
	mluRPCDeviceName         = "/dev/cambr-rpc"
	mluCmsgDeviceName        = "/dev/cmsg_ctrl"
	mluDeviceName            = "/dev/cambricon_dev"
	mluCommuDeviceName       = "/dev/commu"
	mluUARTConsoleDeviceName = "/dev/ttyMS"
)

type deviceList struct {
	hasCnmonDev bool
	hasCodecDev bool
	hasC10Dev   bool

	hasCtrlDev        bool
	hasMsgqDev        bool
	hasRPCDev         bool
	hasCmsgDev        bool
	hasCommuDev       bool
	hasUARTConsoleDev bool
}

func newDeviceList() *deviceList {
	return &deviceList{
		hasCnmonDev:       hostDeviceExistsWithPrefix(mlu100MonitorDeviceName),
		hasCodecDev:       hostDeviceExistsWithPrefix(mlu100CodecDeviceName),
		hasC10Dev:         hostDeviceExistsWithPrefix(mlu100DeviceName),
		hasCtrlDev:        hostDeviceExistsWithPrefix(mluMonitorDeviceName),
		hasMsgqDev:        hostDeviceExistsWithPrefix(mluMsgqDeviceName),
		hasRPCDev:         hostDeviceExistsWithPrefix(mluRPCDeviceName),
		hasCmsgDev:        hostDeviceExistsWithPrefix(mluCmsgDeviceName),
		hasCommuDev:       hostDeviceExistsWithPrefix(mluCommuDeviceName),
		hasUARTConsoleDev: hostDeviceExistsWithPrefix(mluUARTConsoleDeviceName),
	}
}

func hostDeviceExistsWithPrefix(prefix string) bool {
	matches, err := filepath.Glob(prefix + "*")
	if err != nil {
		log.Printf("failed to know if host device with prefix exists, err: %v \n", err)
		return false
	}
	return len(matches) > 0
}

func check(err error) {
	if err != nil {
		log.Panicln("Fatal:", err)
	}
}

func generateFakeDevs(origin *cndev.Device, num int, sriovEnabled bool) ([]*pluginapi.Device, map[string]*cndev.Device) {
	devs := []*pluginapi.Device{}
	devsInfo := make(map[string]*cndev.Device)
	var uuid string
	path := origin.Path
	for i := 0; i < num; i++ {
		if sriovEnabled {
			path = fmt.Sprintf("%svf%d", origin.Path, i+1)
			uuid = fmt.Sprintf("%s--fake--%d", origin.UUID, i+1)
		} else {
			uuid = fmt.Sprintf("%s-_-%d", origin.UUID, i+1)
		}
		devsInfo[uuid] = &cndev.Device{
			Slot: origin.Slot,
			UUID: uuid,
			Path: path,
		}
		devs = append(devs, &pluginapi.Device{
			ID:     uuid,
			Health: pluginapi.Healthy,
		})
	}
	return devs, devsInfo
}

func getDevices(mode string, fakeNum int) ([]*pluginapi.Device, map[string]*cndev.Device) {
	devs := []*pluginapi.Device{}
	devsInfo := make(map[string]*cndev.Device)

	num, err := cndev.GetDeviceCount()
	check(err)

	for i := uint(0); i < num; i++ {
		d, err := cndev.NewDeviceLite(i, mode == sriov)
		check(err)
		switch mode {
		case envShare:
			if fakeNum < 1 {
				check(fmt.Errorf("invalid env-share number %d", fakeNum))
			}
			devices, infos := generateFakeDevs(d, fakeNum, false)
			devs = append(devs, devices...)
			for k, v := range infos {
				devsInfo[k] = v
			}
		case sriov:
			err = d.EnableSriov(fakeNum)
			check(err)
			devices, infos := generateFakeDevs(d, fakeNum, true)
			devs = append(devs, devices...)
			for k, v := range infos {
				devsInfo[k] = v
			}
		default:
			devsInfo[d.UUID] = d
			devs = append(devs, &pluginapi.Device{
				ID:     d.UUID,
				Health: pluginapi.Healthy,
			})
		}
	}
	return devs, devsInfo
}

func deviceExists(devs []*pluginapi.Device, id string) bool {
	for _, d := range devs {
		if d.ID == id {
			return true
		}
	}
	return false
}

func watchUnhealthy(ctx context.Context, devs []*pluginapi.Device, devsInfo map[string]*cndev.Device, unhealthy chan<- *pluginapi.Device) {
	healthCheck := true
	for {
		select {
		case <-ctx.Done():
			return
		default:
		}
		if healthCheck {
			for _, dm := range devsInfo {
				ret, err := dm.GetGeviceHealthState(1)
				if err != nil {
					healthCheck = false
					log.Printf("Failed to get Device %s healthy status, stop the healthy check", dm.UUID)
				}
				if ret == 0 {
					for _, d := range devs {
						if d.ID == dm.UUID {
							log.Printf("Unhealthy device :%s\n", d.ID)
							unhealthy <- d
							break
						}
					}
				}
			}
		}
		//Sleep 1 second between two health checks
		time.Sleep(time.Second)
	}
}
