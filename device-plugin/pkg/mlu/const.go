// Copyright 2021 Cambricon, Inc.
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

import pluginapi "k8s.io/kubelet/pkg/apis/deviceplugin/v1beta1"

type pluginMode string

const (
	cambriconVisibleDevices = "CAMBRICON_VISIBLE_DEVICES"

	DsmluLockTime           = "cambricon.com/dsmlu.lock"
	DsmluProfile            = "CAMBRICON_DSMLU_PROFILE"
	DsmluProfileAndInstance = "CAMBRICON_DSMLU_PROFILE_INSTANCE"
	DsmluResourceAssigned   = "CAMBRICON_DSMLU_ASSIGHED"

	mluLinkPolicyUnsatisfied = "mluLinkPolicyUnsatisfied"

	normalMlu  = "mlu"
	realCounts = "real-mlu-counts"
	retries    = 5
	serverSock = pluginapi.DevicePluginPath + "cambricon.sock"
)

const (
	Default       pluginMode = "default"
	DynamicSmlu   pluginMode = "dynamic-smlu"
	EnvShare      pluginMode = "env-share"
	Mim           pluginMode = "mim"
	TopologyAware pluginMode = "topology-aware"
)

const (
	mluCmsgDeviceName        = "/dev/cmsg_ctrl"
	mluCommuDeviceName       = "/dev/commu"
	mluDeviceName            = "/dev/cambricon_dev"
	mluGdrDeviceName         = "/dev/cambricon_gdr"
	mluIpcmDeviceName        = "/dev/cambricon_ipcm"
	mluMonitorDeviceName     = "/dev/cambricon_ctl"
	mluMsgqDeviceName        = "/dev/cambr-msgq"
	mluRPCDeviceName         = "/dev/cambr-rpc"
	mluRPMsgDir              = "/dev/cambricon/"
	mluSplitDeviceName       = "/dev/cambricon-split"
	mluUARTConsoleDeviceName = "/dev/ttyMS"
)

const (
	bestEffort string = "best-effort"
	restricted string = "restricted"
	guaranteed string = "guaranteed"
)
