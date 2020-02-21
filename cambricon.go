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
	"golang.org/x/net/context"
	pluginapi "k8s.io/kubernetes/pkg/kubelet/apis/deviceplugin/v1beta1"
	"log"
	"strings"
	"time"
)

const (
	mlu100MonitorDeviceName = "/dev/cnmon_dev"
	mlu100CodecDeviceName   = "/dev/cncodec_dev"
	mlu100DeviceName        = "/dev/cambricon_c10Dev"

	mlu270MonitorDeviceName = "/dev/cambricon_ctl"
	mlu270MSGQDeviceName    = "/dev/cambr-msgq"
	mlu270RPCDeviceName     = "/dev/cambr-rpc"
	mlu270CmsgDeviceName    = "/dev/cmsg_ctrl"
	mlu270DeviceName        = "/dev/cambricon_dev"
	mlu270COMMUDeviceName   = "/dev/commu"
)

func check(err error) {
	if err != nil {
		log.Panicln("Fatal:", err)
	}
}

func getDevices() []*pluginapi.Device {
	n, err := cndev.GetDeviceCount()
	check(err)

	var devs []*pluginapi.Device
	for i := uint(0); i < n; i++ {
		d, err := cndev.NewDeviceLite(i)
		check(err)
		devs = append(devs, &pluginapi.Device{
			ID:     d.UUID,
			Health: pluginapi.Healthy,
		})
	}

	return devs
}

func getDevicesAndMap() ([]*pluginapi.Device, []*cndev.Device) {
	n, err := cndev.GetDeviceCount()
	check(err)
	var devsMap []*cndev.Device
	var devs []*pluginapi.Device
	for i := uint(0); i < n; i++ {
		d, err := cndev.NewDeviceLite(i)
		check(err)
		devsMap = append(devsMap, d)
		devs = append(devs, &pluginapi.Device{
			ID:     d.UUID,
			Health: pluginapi.Healthy,
		})
	}

	return devs, devsMap
}

func deviceExists(devs []*pluginapi.Device, id string) bool {
	for _, d := range devs {
		if d.ID == id {
			return true
		}
	}
	return false
}

func isMLU270Device(devPath string) bool {
	return strings.HasPrefix(devPath, mlu270DeviceName)
}

func watchUnhealthy(ctx context.Context, devs []*pluginapi.Device, devsMap []*cndev.Device, unhealthy chan<- *pluginapi.Device) {
	var ifSupportHealthCheck = true
	for {
		select {
		case <-ctx.Done():
			return
		default:
		}
		if ifSupportHealthCheck == true {
			for _, dm := range devsMap {

				ret, err := dm.DeviceHealthCheckState(1)
				if err != nil {
					ifSupportHealthCheck = false
					log.Printf("Failed to get Device %s healthy status, stop the healthy check", dm.UUID)
				}
				if ret == 0 {
					for _, d := range devs {
						if d.ID == dm.UUID {
							log.Printf("Unhealthy device :%s\n", d.ID)
							unhealthy <- d
							continue
						}
					}
				}
			}
		}
		//Sleep 1 second between two health checks
		time.Sleep(time.Duration(int(1)) * time.Second)
	}
}
