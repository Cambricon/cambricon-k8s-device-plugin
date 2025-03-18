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

// #cgo LDFLAGS: -ldl -Wl,--export-dynamic -Wl,--unresolved-symbols=ignore-in-object-files
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
)

type Device struct {
	MotherBoard string
	Numa        int
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

func Init(healthCheck bool) error {
	r := dl.cndevInit()
	if err := errorString(r); err != nil || healthCheck {
		return err
	}
	return generateDeviceHandleMap()
}

func Release() error {
	return errorString(dl.cndevRelease())
}

func CreateSmluProfile(pl *DsmluProfile, memUnit int) (uint, error) {
	var profileID C.int
	var profile C.cndevSMluSet_t
	profile.version = C.CNDEV_VERSION_6
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
	mode.version = C.CNDEV_VERSION_6
	r := C.cndevGetMimMode(&mode, cndevHandleMap[idx])
	return mode.mimMode == C.CNDEV_FEATURE_ENABLED, errorString(r)
}

func DeviceSmluModeEnabled(idx uint) (bool, error) {
	var mode C.cndevSMLUMode_t
	mode.version = C.CNDEV_VERSION_6
	r := C.cndevGetSMLUMode(&mode, cndevHandleMap[idx])
	return mode.smluMode == C.CNDEV_FEATURE_ENABLED, errorString(r)
}

func GetAllMluInstanceInfo(idx uint) ([]MimInfo, error) {
	miCount := C.int(1 << 4)
	var miInfos []MimInfo
	var miInfo C.cndevMluInstanceInfo_t
	infs := (*C.cndevMluInstanceInfo_t)(C.malloc(C.size_t(miCount) * C.size_t(unsafe.Sizeof(miInfo))))
	defer func() {
		if infs != nil {
			C.free(unsafe.Pointer(infs))
		}
	}()

	infs.version = C.CNDEV_VERSION_6
	r := C.cndevGetAllMluInstanceInfo(&miCount, infs, cndevHandleMap[idx])
	if errorString(r) != nil && r != C.CNDEV_ERROR_INSUFFICIENT_SPACE {
		return miInfos, errorString(r)
	}

	// handle the case when the initial count is insufficient,
	// after cndevGetAllMluInstanceInfo miCount will be set to real count,
	// so just use it to reallocate and cndevGetAllMluInstanceInfo again.
	if r == C.CNDEV_ERROR_INSUFFICIENT_SPACE {
		log.Debugf("cndevGetAllMluInstanceInfo with insufficient space with real counts %d, with slot: %d, will try with the real counts", miCount, idx)
		newInfs := (*C.cndevMluInstanceInfo_t)(C.realloc(unsafe.Pointer(infs), C.size_t(miCount)*C.size_t(unsafe.Sizeof(miInfo))))
		if newInfs == nil {
			return miInfos, fmt.Errorf("realloc failed for cndevGetAllMluInstanceInfo")
		}
		infs = newInfs
		r := C.cndevGetAllMluInstanceInfo(&miCount, infs, cndevHandleMap[idx])
		if errorString(r) != nil {
			return miInfos, errorString(r)
		}
	}

	infos := (*[1 << 16]C.cndevMluInstanceInfo_t)(unsafe.Pointer(infs))[:miCount]
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
	smluCount := C.int(1 << 7)
	var smluInfos []SmluInfo
	var smluInfo C.cndevSMluInfo_t
	infs := (*C.cndevSMluInfo_t)(C.malloc(C.size_t(smluCount) * C.size_t(unsafe.Sizeof(smluInfo))))
	defer func() {
		if infs != nil {
			C.free(unsafe.Pointer(infs))
		}
	}()

	infs.version = C.CNDEV_VERSION_6
	r := C.cndevGetAllSMluInstanceInfo(&smluCount, infs, cndevHandleMap[idx])
	if errorString(r) != nil && r != C.CNDEV_ERROR_INSUFFICIENT_SPACE {
		return smluInfos, errorString(r)
	}

	// handle the case when the initial count is insufficient,
	// after cndevGetAllSMluInstanceInfo smluCount will be set to real count,
	// so just use it to reallocate and cndevGetAllSMluInstanceInfo again.
	if r == C.CNDEV_ERROR_INSUFFICIENT_SPACE {
		log.Debugf("cndevGetAllSMluInstanceInfo with insufficient space with real counts %d, with slot: %d, will try with the real counts", smluCount, idx)
		newInfs := (*C.cndevSMluInfo_t)(C.realloc(unsafe.Pointer(infs), C.size_t(smluCount)*C.size_t(unsafe.Sizeof(smluInfo))))
		if newInfs == nil {
			return smluInfos, fmt.Errorf("realloc failed for cndevGetAllSMluInstanceInfo")
		}
		infs = newInfs
		r := C.cndevGetAllSMluInstanceInfo(&smluCount, infs, cndevHandleMap[idx])
		if errorString(r) != nil {
			return smluInfos, errorString(r)
		}
	}

	infos := (*[1 << 16]C.cndevSMluInfo_t)(unsafe.Pointer(infs))[:smluCount]
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
	cardInfos.version = C.CNDEV_VERSION_6
	r := C.cndevGetDeviceCount(&cardInfos)
	return uint(cardInfos.number), errorString(r)
}

func GetDeviceHealthState(d *Device, delayTime int) (int, error) {
	ret, err := getDeviceHealthState(d.Slot, delayTime)
	if err != nil || ret == 0 {
		return 0, err
	}
	return 1, nil
}

func GetDeviceMemory(idx uint) (uint, error) {
	var cardMemInfo C.cndevMemoryInfo_t
	cardMemInfo.version = C.CNDEV_VERSION_6
	r := C.cndevGetMemoryUsage(&cardMemInfo, cndevHandleMap[idx])
	return uint(cardMemInfo.physicalMemoryTotal), errorString(r)
}

func GetDeviceModel(idx uint) string {
	return C.GoString(C.cndevGetCardNameStringByDevId(cndevHandleMap[idx]))
}

func GetDeviceProfileInfo(index uint) ([]DsmluProfileInfo, error) {
	var dsmluProfileInfos []DsmluProfileInfo
	var deviceProfiles C.cndevSMluProfileIdInfo_t
	deviceProfiles.version = C.CNDEV_VERSION_6
	r := C.cndevGetSMluProfileIdInfo(&deviceProfiles, cndevHandleMap[index])
	if errorString(r) != nil {
		return dsmluProfileInfos, errorString(r)
	}
	for i := 0; i < int(deviceProfiles.count); i++ {
		var profileInfo C.cndevSMluProfileInfo_t
		profileInfo.version = C.CNDEV_VERSION_6
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
	uuidInfo.version = C.CNDEV_VERSION_6
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
	visited := make([]bool, num)
	var groups [][]uint
	var dfs func(slot uint, currentGroup *[]uint) bool
	dfs = func(slot uint, currentGroup *[]uint) bool {
		visited[slot] = true
		*currentGroup = append(*currentGroup, slot)
		devs, err := getDeviceMLULinkDevs(slot)
		if err != nil {
			log.Debugf("failed to get device %d mlulink devs %v", slot, err)
			return false
		}
		for dev := range devs {
			if nextSlot, ok := slots[dev]; ok && !visited[nextSlot] {
				if !dfs(nextSlot, currentGroup) {
					return false
				}
			}
		}
		return true
	}
	for slot := uint(0); slot < num; slot++ {
		if !visited[slot] {
			currentGroup := []uint{}
			if !dfs(slot, &currentGroup) {
				return nil, fmt.Errorf("failed to get mlulink groups for slot %d", slot)
			}
			if len(currentGroup) > 0 {
				groups = append(groups, currentGroup)
			}
		}
	}
	log.Debugf("getmlulinkgroups groups %+v", groups)
	return groups, nil
}

func GetSmluInfo(instanceHandle int) (SmluInfo, error) {
	var smluInfo C.cndevSMluInfo_t
	smluInfo.version = C.CNDEV_VERSION_6
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
	numa, err := getDeviceNUMA(idx)
	if err != nil {
		return nil, err
	}
	d := &Device{
		MotherBoard: motherBoard,
		Numa:        numa,
		Path:        path,
		Slot:        idx,
		SN:          sn,
		UUID:        uuid,
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

	cardName.version = C.CNDEV_VERSION_6
	r := C.cndevGetCardName(&cardName, cndevHandleMap[idx])
	err := errorString(r)
	if err != nil {
		return "", "", "", "", err
	}

	if cardName.id == C.MLU100 {
		log.Panicln("MLU100 detected, there is no way to be here.")
	}

	cardSN.version = C.CNDEV_VERSION_6
	r = C.cndevGetCardSN(&cardSN, cndevHandleMap[idx])
	err = errorString(r)
	if err != nil {
		return "", "", "", "", err
	}

	uuidInfo.version = C.CNDEV_VERSION_6
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
	cardHealthState.version = C.CNDEV_VERSION_6
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
		status.version = C.CNDEV_VERSION_6
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
		remoteinfo.version = C.CNDEV_VERSION_6
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

func getDeviceNUMA(idx uint) (int, error) {
	var numaNode C.cndevNUMANodeId_t
	numaNode.version = C.CNDEV_VERSION_6
	r := C.cndevGetNUMANodeIdByDevId(&numaNode, cndevHandleMap[idx])
	return int(numaNode.nodeId), errorString(r)
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
