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

package cndev

// #cgo LDFLAGS: -ldl
// #include "include/cndev.h"
// #include "cndev_dl.h"
import "C"

import (
	"errors"
	"fmt"
	"time"
)

func errorString(ret C.cndevRet_t) error {
	if ret == C.CNDEV_SUCCESS {
		return nil
	}
	err := C.GoString(C.cndevGetErrorString(ret))
	return fmt.Errorf("cndev: %v", err)
}

func Init() error {
	r := C.cndevInit_dl()
	if r == C.CNDEV_ERROR_UNINITIALIZED {
		return errors.New("could not load CNDEV library")
	}
	return errorString(r)
}

func Release() error {
	r := C.cndevRelease_dl()
	return errorString(r)
}

func GetDeviceCount() (uint, error) {
	var cardInfos C.cndevCardInfo_t
	cardInfos.version = C.int(3)
	r := C.cndevGetDeviceCount(&cardInfos)
	return uint(cardInfos.Number), errorString(r)
}

func getDeviceInfo(idx uint) (string, string, error) {
	var cardName C.cndevCardName_t
	var cardSN C.cndevCardSN_t
	var path string

	cardSN.version = C.int(3)
	r := C.cndevGetCardSN(&cardSN, C.int(idx))
	err := errorString(r)
	if err != nil {
		return "", "", err
	}
	uuid := fmt.Sprintf("MLU-%x", int(cardSN.sn))

	cardName.version = C.int(3)
	r = C.cndevGetCardName(&cardName, C.int(idx))
	err = errorString(r)
	if err != nil {
		return "", "", err
	}
	if cardName.id == C.MLU100 {
		path = fmt.Sprintf("/dev/cambricon_c10Dev%d", idx)
	} else {
		path = fmt.Sprintf("/dev/cambricon_dev%d", idx)
	}
	return uuid, path, nil
}

func getDeviceHealthState(idx uint, delayTime int) (int, error) {
	var ret C.cndevRet_t
	var cardHealthState C.cndevCardHealthState_t
	var healthCode int
	cardHealthState.version = C.int(3)
	// sleep for some seconds
	time.Sleep(time.Duration(delayTime) * time.Second)
	ret = C.cndevGetCardHealthState(&cardHealthState, C.int(idx))
	healthCode = int(cardHealthState.health)
	return healthCode, errorString(ret)
}

func getDevicePCIeInfo(idx uint) (*pcie, error) {
	var pcieInfo C.cndevPCIeInfo_t
	pcieInfo.version = C.int(3)
	r := C.cndevGetPCIeInfo(&pcieInfo, C.int(idx))
	if err := errorString(r); err != nil {
		return nil, err
	}
	return &pcie{
		domain:   int(pcieInfo.domain),
		bus:      int(pcieInfo.bus),
		device:   int(pcieInfo.device),
		function: int(pcieInfo.function),
	}, nil
}
