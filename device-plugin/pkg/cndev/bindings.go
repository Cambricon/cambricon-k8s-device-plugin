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
	"log"
	"time"
	"unsafe"
)

const version = 5

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
	cardInfos.version = C.int(version)
	r := C.cndevGetDeviceCount(&cardInfos)
	return uint(cardInfos.number), errorString(r)
}

func GetDeviceModel(idx uint) string {
	return C.GoString(C.getCardNameStringByDevId(C.int(idx)))
}

func getDeviceInfo(idx uint) (string, string, string, error) {
	var cardName C.cndevCardName_t
	var cardSN C.cndevCardSN_t
	var uuidInfo C.cndevUUID_t

	cardName.version = C.int(version)
	r := C.cndevGetCardName(&cardName, C.int(idx))
	err := errorString(r)
	if err != nil {
		return "", "", "", err
	}

	if cardName.id == C.MLU100 {
		log.Panicln("MLU100 detected, there is no way to be here.")
	}

	cardSN.version = C.int(version)
	r = C.cndevGetCardSN(&cardSN, C.int(idx))
	err = errorString(r)
	if err != nil {
		return "", "", "", err
	}

	uuidInfo.version = C.int(version)
	r = C.cndevGetUUID(&uuidInfo, C.int(idx))
	err = errorString(r)
	if err != nil {
		return "", "", "", err
	}
	uuid := *(*[C.UUID_SIZE]C.uchar)(unsafe.Pointer(&uuidInfo.uuid))

	return fmt.Sprintf("MLU-%s", uuid), fmt.Sprintf("%x", int(cardSN.motherBoardSn)), fmt.Sprintf("/dev/cambricon_dev%d", idx), nil
}

func getDeviceMLULinkDevs(idx uint) (map[string]int, error) {
	devs := make(map[string]int)
	portNum := C.cndevGetMLULinkPortNumber(C.int(idx))
	for i := 0; i < int(portNum); i++ {
		var status C.cndevMLULinkStatus_t
		status.version = C.int(version)
		r := C.cndevGetMLULinkStatus(&status, C.int(idx), C.int(i))
		err := errorString(r)
		if err != nil {
			return nil, err
		}
		if status.isActive == C.CNDEV_FEATURE_DISABLED {
			log.Printf("MLU %v port %v disabled", idx, i)
			continue
		}
		var remoteinfo C.cndevMLULinkRemoteInfo_t
		remoteinfo.version = C.int(version)
		r = C.cndevGetMLULinkRemoteInfo(&remoteinfo, C.int(idx), C.int(i))
		err = errorString(r)
		if err != nil {
			return nil, err
		}
		uuid := fmt.Sprintf("MLU-%s", *(*[C.UUID_SIZE]C.uchar)(unsafe.Pointer(&remoteinfo.uuid)))
		devs[uuid]++
	}
	return devs, nil
}

func getDeviceHealthState(idx uint, delayTime int) (int, error) {
	var ret C.cndevRet_t
	var cardHealthState C.cndevCardHealthState_t
	var healthCode int
	cardHealthState.version = C.int(version)
	// sleep for some seconds
	time.Sleep(time.Duration(delayTime) * time.Second)
	ret = C.cndevGetCardHealthState(&cardHealthState, C.int(idx))
	healthCode = int(cardHealthState.health)
	return healthCode, errorString(ret)
}

func getDevicePCIeInfo(idx uint) (*pcie, error) {
	var pcieInfo C.cndevPCIeInfo_t
	pcieInfo.version = C.int(version)
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
