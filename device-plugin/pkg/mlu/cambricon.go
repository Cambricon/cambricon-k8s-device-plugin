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
	"fmt"
	"path/filepath"
	"strings"
	"time"

	"github.com/Cambricon/cambricon-k8s-device-plugin/device-plugin/pkg/cndev"
	log "github.com/sirupsen/logrus"
	pluginapi "k8s.io/kubelet/pkg/apis/deviceplugin/v1beta1"
)

type deviceList struct {
	hasCmsgDev        bool
	hasCommuDev       bool
	hasCtrlDev        bool
	hasGdrDev         bool
	hasIpcmDev        bool
	hasMsgqDev        bool
	hasRPCDev         bool
	hasSplitDev       bool
	hasUARTConsoleDev bool
}

func newDeviceList() *deviceList {
	return &deviceList{
		hasCmsgDev:        hostDeviceExistsWithPrefix(mluCmsgDeviceName),
		hasCommuDev:       hostDeviceExistsWithPrefix(mluCommuDeviceName),
		hasCtrlDev:        hostDeviceExistsWithPrefix(mluMonitorDeviceName),
		hasGdrDev:         hostDeviceExistsWithPrefix(mluGdrDeviceName),
		hasIpcmDev:        hostDeviceExistsWithPrefix(mluIpcmDeviceName),
		hasMsgqDev:        hostDeviceExistsWithPrefix(mluMsgqDeviceName),
		hasRPCDev:         hostDeviceExistsWithPrefix(mluRPCDeviceName),
		hasSplitDev:       hostDeviceExistsWithPrefix(mluSplitDeviceName),
		hasUARTConsoleDev: hostDeviceExistsWithPrefix(mluUARTConsoleDeviceName),
	}
}

func hostDeviceExistsWithPrefix(prefix string) bool {
	matches, err := filepath.Glob(prefix + "*")
	if err != nil {
		log.Printf("failed to know if host device with prefix exists, err: %v", err)
		return false
	}
	return len(matches) > 0
}

func check(err error) {
	if err != nil {
		log.Panicln("Fatal:", err)
	}
}

func generateFakeDevs(origin *cndev.Device, num int, mode pluginMode) ([]*pluginapi.Device, map[string]*cndev.Device) {
	devs := []*pluginapi.Device{}
	devsInfo := make(map[string]*cndev.Device)
	var uuid string
	path := origin.Path
	for i := 0; i < num; i++ {
		switch mode {
		case EnvShare:
			uuid = fmt.Sprintf("%s-_-%d", origin.UUID, i+1)
		case DynamicSmlu:
			uuid = fmt.Sprintf("%s-%s-%d", origin.UUID, origin.Profile, i+1)
		}
		devsInfo[uuid] = &cndev.Device{
			Slot:    origin.Slot,
			UUID:    uuid,
			Path:    path,
			Profile: origin.Profile,
		}
		devs = append(devs, &pluginapi.Device{
			ID:     uuid,
			Health: pluginapi.Healthy,
		})
	}

	log.Debugf("generate devices devs: %+v\n devsinfo: %+v", devs, devsInfo)
	return devs, devsInfo
}

func scanMimDevs(origin *cndev.Device, idx uint) ([]*pluginapi.Device, map[string]*cndev.Device) {
	devs := []*pluginapi.Device{}
	devsInfo := make(map[string]*cndev.Device)
	infos, err := cndev.GetAllMluInstanceInfo(idx)
	check(err)
	for _, info := range infos {
		uid := origin.UUID + "-mim-" + info.UUID
		devsInfo[uid] = &cndev.Device{
			Slot:    origin.Slot,
			UUID:    uid,
			Path:    fmt.Sprintf("%s%d", mluDeviceName, origin.Slot) + "," + info.IpcmDevNodeName + "," + info.DevNodeName, // device name should never contain ","
			Profile: strings.ReplaceAll(info.Name, "+", "-"),
		}
		devs = append(devs, &pluginapi.Device{
			ID:     uid,
			Health: pluginapi.Healthy,
		})
	}

	log.Debugf("scanMimDevs devs: %+v\n devsinfo: %+v", devs, devsInfo)
	return devs, devsInfo
}

func GetDevices(o Options) (map[string][]*pluginapi.Device, map[string]map[string]*cndev.Device) {
	devs := []*pluginapi.Device{}
	devsInfo := make(map[string]*cndev.Device)
	num, err := cndev.GetDeviceCount()
	check(err)

	for i := uint(0); i < num; i++ {
		d, err := cndev.NewDeviceLite(i)
		check(err)

		realCountDevice := *d
		realCountDevice.Profile = realCounts
		devsInfo[realCounts+"-"+realCountDevice.UUID] = &realCountDevice
		devs = append(devs, &pluginapi.Device{
			ID:     realCounts + "-" + realCountDevice.UUID,
			Health: pluginapi.Healthy,
		})

		switch o.Mode {
		case EnvShare:
			if o.VirtualizationNum < 1 {
				check(fmt.Errorf("invalid env-share number %d", o.VirtualizationNum))
			}
			devices, infos := generateFakeDevs(d, o.VirtualizationNum, o.Mode)
			devs = append(devs, devices...)
			for k, v := range infos {
				devsInfo[k] = v
			}
		case DynamicSmlu:
			dev := *d
			// fake ipu uuid
			dev.Profile = "vcore"
			num := 100
			devices, infos := generateFakeDevs(&dev, num, o.Mode)
			devs = append(devs, devices...)
			for k, v := range infos {
				devsInfo[k] = v
			}
			// fake memory uuid
			dev.Profile = "vmemory"
			if o.MinDsmluUnit > 0 {
				mem, err := cndev.GetDeviceMemory(i)
				check(err)
				num = int(mem) / o.MinDsmluUnit
			}
			devices, infos = generateFakeDevs(&dev, num, o.Mode)
			devs = append(devs, devices...)
			for k, v := range infos {
				devsInfo[k] = v
			}
		case Mim:
			enabled, err := cndev.DeviceMimModeEnabled(i)
			check(err)
			if enabled {
				devices, infos := scanMimDevs(d, i)
				devs = append(devs, devices...)
				for k, v := range infos {
					devsInfo[k] = v
				}
				break
			}
			fallthrough
		default:
			devsInfo[d.UUID] = d
			devs = append(devs, &pluginapi.Device{
				ID:     d.UUID,
				Health: pluginapi.Healthy,
				Topology: &pluginapi.TopologyInfo{
					Nodes: []*pluginapi.NUMANode{
						{
							ID: int64(d.Numa),
						},
					},
				},
			})
		}
	}
	log.Debugf("GetDevices devs: %+v\n devsinfo: %+v", devs, devsInfo)
	return classifyByProfile(devs, devsInfo)
}

func deviceExists(devs []*pluginapi.Device, id string) bool {
	for _, d := range devs {
		if d.ID == id {
			return true
		}
	}
	return false
}

func watchUnhealthy(ctx context.Context, devsInfo map[string]*cndev.Device, health chan<- *pluginapi.Device) {
	unhealthy := make(map[string]bool)
	var getDeviceComputeModeDisabled bool
	for {
		select {
		case <-ctx.Done():
			return
		default:
		}

		omitDupCall := map[uint]int{}
		for _, dm := range devsInfo {
			if _, ok := omitDupCall[dm.Slot]; !ok {
				ret, _, _, err := cndev.GetDeviceHealthState(dm.Slot, 1)
				if err != nil {
					log.Warnf("Failed to get Device %s healthy status with err %v, set it as unhealthy", dm.UUID, err)
					ret = 0
				}
				if ret == 1 && !getDeviceComputeModeDisabled {
					computeMode, err := cndev.GetDeviceComputeMode(dm.Slot, 1)
					if err != nil {
						getDeviceComputeModeDisabled = true
						log.Warnf("Failed to get Device %s compute mode with err %v, ignore compute mode", dm.UUID, err)
					} else if computeMode {
						ret = 0
						log.Debugf("Device %s is in compute mode, set it as unhealthy", dm.UUID)
					}
				}
				omitDupCall[dm.Slot] = ret
			}

			if omitDupCall[dm.Slot] == 0 {
				if unhealthy[dm.UUID] {
					continue
				}
				unhealthy[dm.UUID] = true
				dev := pluginapi.Device{
					ID:     dm.UUID,
					Health: pluginapi.Unhealthy,
				}
				log.Debugf("Device %s health state changes from health to unhealth in time %s", dm.UUID, time.Now())
				health <- &dev
			} else if unhealthy[dm.UUID] {
				delete(unhealthy, dm.UUID)
				dev := pluginapi.Device{
					ID:     dm.UUID,
					Health: pluginapi.Healthy,
				}
				log.Debugf("Device %s health state changes from health to unhealth in time %s", dm.UUID, time.Now())
				health <- &dev
			}
		}

		//Sleep 1 second between two health checks
		time.Sleep(time.Second)
	}
}

func classifyByProfile(devs []*pluginapi.Device, devsInfo map[string]*cndev.Device) (
	map[string][]*pluginapi.Device, map[string]map[string]*cndev.Device) {
	devsM := map[string][]*pluginapi.Device{}
	devsInfoM := map[string]map[string]*cndev.Device{}
	for _, d := range devs {
		profile := devsInfo[d.ID].Profile
		if profile == "" {
			profile = normalMlu
		}
		devsM[profile] = append(devsM[profile], d)
		if _, ok := devsInfoM[profile]; !ok {
			devsInfoM[profile] = map[string]*cndev.Device{}
		}
		devsInfoM[profile][d.ID] = devsInfo[d.ID]
	}

	log.Debugf("classifyByProfile devs: %+v\n devsinfo: %+v", devsM, devsInfoM)
	return devsM, devsInfoM
}
