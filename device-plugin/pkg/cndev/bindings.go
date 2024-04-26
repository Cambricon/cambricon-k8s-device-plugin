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

// #cgo LDFLAGS: -ldl -Wl,--unresolved-symbols=ignore-in-object-files
// #include "include/cndev.h"
import "C"

import (
	"fmt"
	"time"
	"unsafe"

	log "github.com/sirupsen/logrus"
)

var (
	cndevHandleMap map[uint]C.cndevDevice_t
	xidErrorsList  []string
)

type Device struct {
	MotherBoard string
	Path        string
	Profile     string
	Slot        uint
	SN          string
	UUID        string
}

type DsmluProfile struct {
	Slot    int
	Vcore   int
	Vmemory int
}

type DsmluProfileInfo struct {
	Memory    uint64
	Name      string
	Quota     uint
	ProfileID int
	Remain    int
	Total     int
}

type MimInfo struct {
	DevNodeName     string
	IpcmDevNodeName string
	Name            string
	UUID            string
}

type SmluInfo struct {
	DevNodeName string
	InstanceID  int
	Name        string
	ProfileID   int
	UUID        string
}

func Init(healthCheck bool, list []string) error {
	r := dl.cndevInit()
	if err := errorString(r); err != nil || healthCheck {
		return err
	}
	xidErrorsList = list
	if len(xidErrorsList) == 0 {
		xidErrorsList = []string{
			"CNDEV_XID_DBE_ECC_ERROR",
			"CNDEV_XID_FALLEN_OFF_ERROR",
			"CNDEV_XID_HBM_ERROR",
			"CNDEV_XID_HEARTBEAT_ERROR",
			"CNDEV_XID_IPU_RESET_ERROR",
			"CNDEV_XID_MCU_ERROR",
			"CNDEV_XID_MLULINK_FATAL_ERROR",
			"CNDEV_XID_OVER_TEMP_ERROR",
			"CNDEV_XID_PAGE_RETIREMENT_ERROR",
			"CNDEV_XID_PCIE_DMA_ERROR",
			"CNDEV_XID_RPC_ERROR",
		}
	}
	return generateDeviceHandleMap()
}

func Release() error {
	return errorString(dl.cndevRelease())
}

func CreateSmluProfile(pl *DsmluProfile, memUnit int) (uint, error) {
	var profileID C.int
	var profile C.cndevSMluSet_t
	profile.version = C.CNDEV_VERSION_5
	profile.mluQuota = C.uint(pl.Vcore)
	profile.memorySize = C.ulong(pl.Vmemory * memUnit * 1024 * 1024)
	r := C.cndevCreateSMluProfileInfo(&profile, &profileID, cndevHandleMap[uint(pl.Slot)])
	return uint(profileID), errorString(r)
}

func CreateSmluProfileInstance(profileID, index uint) (int, error) {
	var instance C.cndevMluInstance_t
	name := C.CString("")
	defer C.free(unsafe.Pointer(name))
	r := C.cndevCreateSMluInstanceByProfileId(&instance, C.uint(profileID), cndevHandleMap[index], name)
	return int(instance), errorString(r)
}

func DestroySmlu(instanceHandle int) error {
	return errorString(C.cndevDestroySMluInstanceByHandle(C.int(instanceHandle)))
}

func DestroySmluProfile(profileID, index uint) error {
	return errorString(C.cndevDestroySMluProfileInfo(C.int(profileID), cndevHandleMap[index]))
}

func DeviceMimModeEnabled(idx uint) (bool, error) {
	var mode C.cndevMimMode_t
	mode.version = C.CNDEV_VERSION_5
	r := C.cndevGetMimMode(&mode, cndevHandleMap[idx])
	return mode.mimMode == C.CNDEV_FEATURE_ENABLED, errorString(r)
}

func DeviceSmluModeEnabled(idx uint) (bool, error) {
	var mode C.cndevSMLUMode_t
	mode.version = C.CNDEV_VERSION_5
	r := C.cndevGetSMLUMode(&mode, cndevHandleMap[idx])
	return mode.smluMode == C.CNDEV_FEATURE_ENABLED, errorString(r)
}

func GetAllMluInstanceInfo(idx uint) ([]MimInfo, error) {
	var miInfos []MimInfo
	miCount := C.int(10)
	var miInfo C.cndevMluInstanceInfo_t
	infs := (*C.cndevMluInstanceInfo_t)(C.malloc(C.size_t(miCount) * C.size_t(unsafe.Sizeof(miInfo))))
	defer C.free(unsafe.Pointer(infs))
	infs.version = C.CNDEV_VERSION_6
	r := C.cndevGetAllMluInstanceInfo(&miCount, infs, cndevHandleMap[idx])
	if errorString(r) != nil {
		return miInfos, errorString(r)
	}
	infos := (*[10]C.cndevMluInstanceInfo_t)(unsafe.Pointer(infs))[:miCount]
	for i := 0; i < int(miCount); i++ {
		info := infos[i]
		miInfos = append(miInfos,
			MimInfo{
				DevNodeName:     C.GoString((*C.char)(unsafe.Pointer(&info.devNodeName))),
				IpcmDevNodeName: C.GoString((*C.char)(unsafe.Pointer(&info.ipcmDevNodeName))),
				Name:            C.GoString((*C.char)(unsafe.Pointer(&info.profileName))),
				UUID:            fmt.Sprintf("MLU-%s", C.GoString((*C.char)(unsafe.Pointer(&info.uuid)))),
			})
	}
	log.Debugf("Mim infos for device %d are %+v", idx, miInfos)
	return miInfos, nil
}

func GetAllSmluInfo(idx uint) ([]SmluInfo, error) {
	var smluInfos []SmluInfo
	smluCount := C.int(100)
	var smluInfo C.cndevSMluInfo_t
	infs := (*C.cndevSMluInfo_t)(C.malloc(C.size_t(smluCount) * C.size_t(unsafe.Sizeof(smluInfo))))
	defer C.free(unsafe.Pointer(infs))
	infs.version = C.CNDEV_VERSION_6
	r := C.cndevGetAllSMluInstanceInfo(&smluCount, infs, cndevHandleMap[idx])
	if errorString(r) != nil {
		return smluInfos, errorString(r)
	}
	infos := (*[100]C.cndevSMluInfo_t)(unsafe.Pointer(infs))[:smluCount]
	for i := 0; i < int(smluCount); i++ {
		info := infos[i]
		smluInfos = append(smluInfos,
			SmluInfo{
				DevNodeName: C.GoString((*C.char)(unsafe.Pointer(&info.devNodeName))),
				InstanceID:  int(info.instanceId),
				Name:        C.GoString((*C.char)(unsafe.Pointer(&info.profileName))),
				ProfileID:   int(info.profileId),
				UUID:        fmt.Sprintf("MLU-%s", C.GoString((*C.char)(unsafe.Pointer(&info.uuid)))),
			})
	}
	log.Debugf("Smlu infos for device %d are %+v", idx, smluInfos)
	return smluInfos, nil
}

func GetDeviceCount() (uint, error) {
	var cardInfos C.cndevCardInfo_t
	cardInfos.version = C.CNDEV_VERSION_5
	r := C.cndevGetDeviceCount(&cardInfos)
	return uint(cardInfos.number), errorString(r)
}

func GetDeviceHealthState(d *Device, delayTime int) (int, error) {
	ret, err := getDeviceHealthState(d.Slot, delayTime)
	if err != nil || ret == 0 {
		return 0, err
	}
	res, err := getDeviceXidError(d.Slot)
	if err != nil || res {
		return 0, err
	}
	return 1, nil
}

func GetDeviceMemory(idx uint) (uint, error) {
	var cardMemInfo C.cndevMemoryInfo_t
	cardMemInfo.version = C.CNDEV_VERSION_5
	r := C.cndevGetMemoryUsage(&cardMemInfo, cndevHandleMap[idx])
	return uint(cardMemInfo.physicalMemoryTotal), errorString(r)
}

func GetDeviceModel(idx uint) string {
	return C.GoString(C.cndevGetCardNameStringByDevId(cndevHandleMap[idx]))
}

func GetDeviceProfileInfo(index uint) ([]DsmluProfileInfo, error) {
	var dsmluProfileInfos []DsmluProfileInfo
	var deviceProfiles C.cndevSMluProfileIdInfo_t
	deviceProfiles.version = C.CNDEV_VERSION_5
	r := C.cndevGetSMluProfileIdInfo(&deviceProfiles, cndevHandleMap[index])
	if errorString(r) != nil {
		return dsmluProfileInfos, errorString(r)
	}
	for i := 0; i < int(deviceProfiles.count); i++ {
		var profileInfo C.cndevSMluProfileInfo_t
		profileInfo.version = C.CNDEV_VERSION_5
		r := C.cndevGetSMluProfileInfo(&profileInfo, deviceProfiles.profileId[i], cndevHandleMap[index])
		if errorString(r) != nil {
			return dsmluProfileInfos, errorString(r)
		}
		dsmluProfileInfos = append(dsmluProfileInfos,
			DsmluProfileInfo{
				Memory:    uint64(profileInfo.memorySize[C.CNDEV_SMLU_MAX]),
				Name:      C.GoString((*C.char)(unsafe.Pointer(&profileInfo.name))),
				ProfileID: int(deviceProfiles.profileId[i]),
				Quota:     uint(profileInfo.mluQuota[C.CNDEV_SMLU_MAX]),
				Remain:    int(profileInfo.remainCapacity),
				Total:     int(profileInfo.totalCapacity),
			})
	}

	return dsmluProfileInfos, nil
}

func GetDeviceUUID(idx uint) (string, error) {
	var uuidInfo C.cndevUUID_t
	uuidInfo.version = C.CNDEV_VERSION_5
	r := C.cndevGetUUID(&uuidInfo, cndevHandleMap[idx])
	if err := errorString(r); err != nil {
		return "", err
	}
	return C.GoString((*C.char)(unsafe.Pointer(&uuidInfo.uuid))), nil
}

func GetExistProfile(pl *DsmluProfile, memUnit int) (*DsmluProfileInfo, bool) {
	infos, err := GetDeviceProfileInfo(uint(pl.Slot))
	if err != nil {
		log.Printf("failed to get device %d profileInfo %v", pl.Slot, err)
		return nil, false
	}

	log.Debugf("Get exist profile infos %v with %v", infos, pl)

	for _, info := range infos {
		if info.Quota == uint(pl.Vcore) &&
			info.Memory == uint64(pl.Vmemory*memUnit*1024*1024) {
			log.Debugf("Get exist profile info %v", info)
			return &info, true
		}
	}
	return nil, false
}

func GetMLULinkGroups() ([][]uint, error) {
	num, err := GetDeviceCount()
	if err != nil {
		return nil, err
	}
	slots := map[string]uint{}
	for i := uint(0); i < num; i++ {
		uuid, _, _, _, err := getDeviceInfo(i)
		if err != nil {
			return nil, err
		}
		slots[uuid] = i
	}
	group := map[uint]bool{}
	queue := []uint{0}
	visited := map[uint]bool{}
	for len(queue) != 0 {
		slot := queue[0]
		queue = queue[1:]
		visited[slot] = true
		devs, err := getDeviceMLULinkDevs(slot)
		if err != nil {
			return nil, err
		}
		for dev := range devs {
			if _, ok := slots[dev]; !ok {
				continue
			}
			if !visited[slots[dev]] {
				queue = append(queue, slots[dev])
			}
		}
		group[slot] = true
	}
	// We assume there are at most 2 groups.
	group1 := []uint{}
	group2 := []uint{}
	for idx := range group {
		group1 = append(group1, idx)
	}
	for slot := uint(0); slot < num; slot++ {
		if !group[slot] {
			group2 = append(group2, slot)
		}
	}
	if len(group2) != 0 {
		log.Debugf("getmlulinkgroups groups %+v", [][]uint{group1, group2})
		return [][]uint{group1, group2}, nil
	}

	log.Debugf("getmlulinkgroups groups %+v", [][]uint{group1})
	return [][]uint{group1}, nil
}

func GetSmluInfo(instanceHandle int) (SmluInfo, error) {
	var smluInfo C.cndevSMluInfo_t
	smluInfo.version = C.CNDEV_VERSION_5
	r := C.cndevGetSMluInstanceInfo(&smluInfo, C.int(instanceHandle))
	return SmluInfo{
		DevNodeName: C.GoString((*C.char)(unsafe.Pointer(&smluInfo.devNodeName))),
		InstanceID:  int(smluInfo.instanceId),
		Name:        C.GoString((*C.char)(unsafe.Pointer(&smluInfo.profileName))),
		UUID:        fmt.Sprintf("MLU-%s", C.GoString((*C.char)(unsafe.Pointer(&smluInfo.uuid)))),
	}, errorString(r)
}

func NewDeviceLite(idx uint) (*Device, error) {
	uuid, sn, motherBoard, path, err := getDeviceInfo(idx)
	if err != nil {
		return nil, err
	}
	d := &Device{
		Slot:        idx,
		UUID:        uuid,
		SN:          sn,
		Path:        path,
		MotherBoard: motherBoard,
	}
	log.Debugf("newdevicelite device %+v", d)
	return d, nil
}

func errorString(ret C.cndevRet_t) error {
	if ret == C.CNDEV_SUCCESS {
		return nil
	}
	err := C.GoString(C.cndevGetErrorString(ret))
	return fmt.Errorf("cndev: %v", err)
}

func getDeviceInfo(idx uint) (string, string, string, string, error) {
	var cardName C.cndevCardName_t
	var cardSN C.cndevCardSN_t
	var uuidInfo C.cndevUUID_t

	cardName.version = C.CNDEV_VERSION_5
	r := C.cndevGetCardName(&cardName, cndevHandleMap[idx])
	err := errorString(r)
	if err != nil {
		return "", "", "", "", err
	}

	if cardName.id == C.MLU100 {
		log.Panicln("MLU100 detected, there is no way to be here.")
	}

	cardSN.version = C.CNDEV_VERSION_5
	r = C.cndevGetCardSN(&cardSN, cndevHandleMap[idx])
	err = errorString(r)
	if err != nil {
		return "", "", "", "", err
	}

	uuidInfo.version = C.CNDEV_VERSION_5
	r = C.cndevGetUUID(&uuidInfo, cndevHandleMap[idx])
	err = errorString(r)
	if err != nil {
		return "", "", "", "", err
	}
	uuid := C.GoString((*C.char)(unsafe.Pointer(&uuidInfo.uuid)))

	log.Debugf("getdeviceinfo uuid: %s cardsn: %s mothernoardsn: %s path: %s  ", fmt.Sprintf("MLU-%s", uuid), fmt.Sprintf("%x", uint64(cardSN.sn)), fmt.Sprintf("%x", uint64(cardSN.motherBoardSn)), fmt.Sprintf("/dev/cambricon_dev%d", idx))
	return fmt.Sprintf("MLU-%s", uuid), fmt.Sprintf("%x", uint64(cardSN.sn)), fmt.Sprintf("%x", uint64(cardSN.motherBoardSn)), fmt.Sprintf("/dev/cambricon_dev%d", idx), nil
}

func getDeviceHealthState(idx uint, delayTime int) (int, error) {
	var ret C.cndevRet_t
	var cardHealthState C.cndevCardHealthState_t
	var healthCode int
	cardHealthState.version = C.CNDEV_VERSION_5
	// sleep for some seconds
	time.Sleep(time.Duration(delayTime) * time.Second)
	ret = C.cndevGetCardHealthState(&cardHealthState, cndevHandleMap[idx])
	healthCode = int(cardHealthState.health)
	return healthCode, errorString(ret)
}

func getDeviceMLULinkDevs(idx uint) (map[string]int, error) {
	devs := make(map[string]int)
	portNum := C.cndevGetMLULinkPortNumber(cndevHandleMap[idx])
	for i := 0; i < int(portNum); i++ {
		var status C.cndevMLULinkStatus_t
		status.version = C.CNDEV_VERSION_5
		r := C.cndevGetMLULinkStatus(&status, cndevHandleMap[idx], C.int(i))
		err := errorString(r)
		if err != nil {
			return nil, err
		}
		if status.isActive == C.CNDEV_FEATURE_DISABLED {
			log.Printf("MLU %v port %v disabled", idx, i)
			continue
		}
		var remoteinfo C.cndevMLULinkRemoteInfo_t
		remoteinfo.version = C.CNDEV_VERSION_5
		r = C.cndevGetMLULinkRemoteInfo(&remoteinfo, cndevHandleMap[idx], C.int(i))
		err = errorString(r)
		if err != nil {
			return nil, err
		}
		uuid := fmt.Sprintf("MLU-%s", C.GoString((*C.char)(unsafe.Pointer(&remoteinfo.uuid))))
		devs[uuid]++
	}
	log.Debugf("getdevicemlulinkdevs devs %+v", devs)
	return devs, nil
}

func getDeviceXidError(idx uint) (bool, error) {
	var xidErr C.cndevXidErrorV2_t
	xidErr.version = C.CNDEV_VERSION_5
	r := C.cndevGetXidErrorV2(&xidErr, cndevHandleMap[idx])
	if err := errorString(r); err != nil {
		return true, err
	}
	xidErrorMap := map[string]struct{}{}
	for _, x := range xidErrorsList {
		xidErrorMap[x] = struct{}{}
	}
	for i, count := range xidErr.xidCount {
		if count == 0 {
			continue
		}
		errString := C.GoString(C.cndevGetXidErrorString(C.cndevXidEnum_t(i)))
		if _, ok := xidErrorMap[errString]; ok {
			log.Warnf("Found xid error %s for device %d", errString, idx)
			return true, nil
		}
	}

	return false, nil
}

func generateDeviceHandleMap() error {
	num, err := GetDeviceCount()
	if err != nil {
		return err
	}
	cndevHandleMap = map[uint]C.cndevDevice_t{}
	for i := uint(0); i < num; i++ {
		var handle C.cndevDevice_t
		r := C.cndevGetDeviceHandleByIndex(C.int(i), &handle)
		if errorString(r) != nil {
			return errorString(r)
		}
		cndevHandleMap[i] = handle
	}
	return nil
}
