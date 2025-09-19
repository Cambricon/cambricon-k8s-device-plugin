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
	"io"
	"os"
	"path/filepath"
	"strconv"
	"strings"
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
	if healthCheck {
		return (errorString(C.cndevInit(C.int(0))))
	}
	return errorString(dl.cndevInit())
}

func Release() error {
	return errorString(dl.cndevRelease())
}

func CreateSmluProfile(pl *DsmluProfile, memUnit int) (uint, error) {
	if ret := dl.checkExist("cndevCreateSMluProfileInfo"); ret != C.CNDEV_SUCCESS {
		return 0, errorString(ret)
	}

	var profileID C.int
	var profile C.cndevSMluSet_t
	profile.version = C.CNDEV_VERSION_6
	profile.mluQuota = C.uint(pl.Vcore)
	profile.memorySize = C.ulong(pl.Vmemory * memUnit * 1024 * 1024)
	r := C.cndevCreateSMluProfileInfo(&profile, &profileID, cndevHandleMap[uint(pl.Slot)])
	return uint(profileID), errorString(r)
}

func CreateSmluProfileInstance(profileID, index uint) (int, error) {
	if ret := dl.checkExist("cndevCreateSMluInstanceByProfileId"); ret != C.CNDEV_SUCCESS {
		return 0, errorString(ret)
	}

	var instance C.cndevMluInstance_t
	name := C.CString("")
	defer C.free(unsafe.Pointer(name))
	r := C.cndevCreateSMluInstanceByProfileId(&instance, C.uint(profileID), cndevHandleMap[index], name)
	return int(instance), errorString(r)
}

func DestroySmlu(instanceHandle int) error {
	if ret := dl.checkExist("cndevDestroySMluInstanceByHandle"); ret != C.CNDEV_SUCCESS {
		return errorString(ret)
	}

	return errorString(C.cndevDestroySMluInstanceByHandle(C.int(instanceHandle)))
}

func DestroySmluProfile(profileID, index uint) error {
	if ret := dl.checkExist("cndevDestroySMluProfileInfo"); ret != C.CNDEV_SUCCESS {
		return errorString(ret)
	}

	return errorString(C.cndevDestroySMluProfileInfo(C.int(profileID), cndevHandleMap[index]))
}

func DeviceMimModeEnabled(idx uint) (bool, error) {
	if ret := dl.checkExist("cndevGetMimMode"); ret != C.CNDEV_SUCCESS {
		return false, errorString(ret)
	}

	var mode C.cndevMimMode_t
	mode.version = C.CNDEV_VERSION_6
	r := C.cndevGetMimMode(&mode, cndevHandleMap[idx])
	return mode.mimMode == C.CNDEV_FEATURE_ENABLED, errorString(r)
}

func DeviceSmluModeEnabled(idx uint) (bool, error) {
	if ret := dl.checkExist("cndevGetSMLUMode"); ret != C.CNDEV_SUCCESS {
		return false, errorString(ret)
	}

	var mode C.cndevSMLUMode_t
	mode.version = C.CNDEV_VERSION_6
	r := C.cndevGetSMLUMode(&mode, cndevHandleMap[idx])
	return mode.smluMode == C.CNDEV_FEATURE_ENABLED, errorString(r)
}

func GetAllMluInstanceInfo(idx uint) ([]MimInfo, error) {
	if ret := dl.checkExist("cndevGetAllMluInstanceInfo"); ret != C.CNDEV_SUCCESS {
		return nil, errorString(ret)
	}

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
	if ret := dl.checkExist("cndevGetAllSMluInstanceInfo"); ret != C.CNDEV_SUCCESS {
		return nil, errorString(ret)
	}

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
	if ret := dl.checkExist("cndevGetDeviceCount"); ret != C.CNDEV_SUCCESS {
		return 0, errorString(ret)
	}

	var cardInfos C.cndevCardInfo_t
	cardInfos.version = C.CNDEV_VERSION_6
	r := C.cndevGetDeviceCount(&cardInfos)
	return uint(cardInfos.number), errorString(r)
}

func GetDeviceMemory(idx uint) (uint, error) {
	if ret := dl.checkExist("cndevGetMemoryUsageV2"); ret != C.CNDEV_SUCCESS {
		return 0, errorString(ret)
	}

	var cardMemInfo C.cndevMemoryInfoV2_t
	r := C.cndevGetMemoryUsageV2(&cardMemInfo, cndevHandleMap[idx])
	return uint(cardMemInfo.physicalMemoryTotal), errorString(r)
}

func GetDeviceModel(idx uint) string {
	if ret := dl.checkExist("cndevGetCardNameStringByDevId"); ret != C.CNDEV_SUCCESS {
		return ""
	}

	return C.GoString(C.cndevGetCardNameStringByDevId(cndevHandleMap[idx]))
}

func GetDeviceProfileInfo(index uint) ([]DsmluProfileInfo, error) {
	if ret := dl.checkExist("cndevGetSMluProfileIdInfo"); ret != C.CNDEV_SUCCESS {
		return nil, errorString(ret)
	}

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
	if ret := dl.checkExist("cndevGetUUID"); ret != C.CNDEV_SUCCESS {
		return "", errorString(ret)
	}

	var uuidInfo C.cndevUUID_t
	uuidInfo.version = C.CNDEV_VERSION_6
	r := C.cndevGetUUID(&uuidInfo, cndevHandleMap[idx])
	if err := errorString(r); err != nil {
		return "", err
	}
	return C.GoString((*C.char)(unsafe.Pointer(&uuidInfo.uuid))), nil
}

func GetDeviceVersion(idx uint) (uint, uint, uint, uint, uint, uint, error) {
	if ret := dl.checkExist("cndevGetVersionInfo"); ret != C.CNDEV_SUCCESS {
		return 0, 0, 0, 0, 0, 0, errorString(ret)
	}

	var versionInfo C.cndevVersionInfo_t
	versionInfo.version = C.CNDEV_VERSION_6
	r := C.cndevGetVersionInfo(&versionInfo, cndevHandleMap[idx])
	return uint(versionInfo.mcuMajorVersion), uint(versionInfo.mcuMinorVersion), uint(versionInfo.mcuBuildVersion), uint(versionInfo.driverMajorVersion), uint(versionInfo.driverMinorVersion), uint(versionInfo.driverBuildVersion), errorString(r)
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
	if ret := dl.checkExist("cndevGetSMluInstanceInfo"); ret != C.CNDEV_SUCCESS {
		return SmluInfo{}, errorString(ret)
	}

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
	if r := dl.checkExist("cndevGetErrorString"); r != C.CNDEV_SUCCESS {
		return errorString(r)
	}

	if ret == C.CNDEV_SUCCESS {
		return nil
	}
	err := C.GoString(C.cndevGetErrorString(ret))
	return fmt.Errorf("cndev: %v", err)
}

func GetDeviceComputeMode(idx uint, delayTime int) (bool, error) {
	if ret := dl.checkExist("cndevGetComputeMode"); ret != C.CNDEV_SUCCESS {
		return false, errorString(ret)
	}

	var ret C.cndevRet_t
	var cardComputeMode C.cndevComputeMode_t
	cardComputeMode.version = C.CNDEV_VERSION_6
	// sleep for some seconds
	time.Sleep(time.Duration(delayTime) * time.Second)
	ret = C.cndevGetComputeMode(&cardComputeMode, cndevHandleMap[idx])
	return cardComputeMode.mode == C.CNDEV_COMPUTEMODE_PROHIBITED, errorString(ret)
}

func getDeviceInfo(idx uint) (string, string, string, string, error) {
	if ret := dl.checkExist("cndevGetCardName", "cndevGetCardSN", "cndevGetUUID"); ret != C.CNDEV_SUCCESS {
		return "", "", "", "", errorString(ret)
	}

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

func GetDeviceHealthState(idx uint, delayTime int) (int, bool, bool, error) {
	if ret := dl.checkExist("cndevGetCardHealthState"); ret != C.CNDEV_SUCCESS {
		return 0, false, false, errorString(ret)
	}

	var ret C.cndevRet_t
	var cardHealthState C.cndevCardHealthState_t
	var healthCode int
	cardHealthState.version = C.CNDEV_VERSION_6
	// sleep for some seconds
	time.Sleep(time.Duration(delayTime) * time.Second)
	ret = C.cndevGetCardHealthState(&cardHealthState, cndevHandleMap[idx])
	healthCode = int(cardHealthState.health)
	return healthCode, cardHealthState.deviceState == C.CNDEV_HEALTH_STATE_DEVICE_GOOD, cardHealthState.driverState == C.CNDEV_HEALTH_STATE_DRIVER_RUNNING, errorString(ret)
}

func getDeviceMLULinkDevs(idx uint) (map[string]int, error) {
	if ret := dl.checkExist("cndevGetMLULinkPortNumber", "cndevGetMLULinkStatusV2", "cndevGetMLULinkRemoteInfo"); ret != C.CNDEV_SUCCESS {
		return nil, errorString(ret)
	}

	devs := make(map[string]int)
	portNum := C.cndevGetMLULinkPortNumber(cndevHandleMap[idx])
	for i := 0; i < int(portNum); i++ {
		var status C.cndevMLULinkStatusV2_t
		r := C.cndevGetMLULinkStatusV2(&status, cndevHandleMap[idx], C.int(i))
		err := errorString(r)
		if err != nil {
			return nil, err
		}
		if status.macState == C.CNDEV_MLULINK_MAC_STATE_DOWN {
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
	if ret := dl.checkExist("cndevGetNUMANodeIdByDevId"); ret != C.CNDEV_SUCCESS {
		return 0, errorString(ret)
	}

	var numaNode C.cndevNUMANodeId_t
	numaNode.version = C.CNDEV_VERSION_6
	r := C.cndevGetNUMANodeIdByDevId(&numaNode, cndevHandleMap[idx])
	return int(numaNode.nodeId), errorString(r)
}

func generateDeviceHandleMap(count uint) error {
	if ret := dl.checkExist("cndevGetDeviceHandleByIndex"); ret != C.CNDEV_SUCCESS {
		return errorString(ret)
	}

	cndevHandleMap = map[uint]C.cndevDevice_t{}
	for i := uint(0); i < count; i++ {
		var handle C.cndevDevice_t
		r := C.cndevGetDeviceHandleByIndex(C.int(i), &handle)
		if errorString(r) != nil {
			return errorString(r)
		}
		cndevHandleMap[i] = handle
	}
	return nil
}

func FetchMLUCounts() (uint, error) {
	targetVendorID := uint16(0xcabc) // cambricon mlu vendor ID is 0xcabc
	targetClassBase := uint8(0x12)   // cambricon mlu class code base is 0x12
	pciDevicesPath := "/sys/bus/pci/devices"

	readHexFile := func(path string) (uint64, error) {
		data, err := os.ReadFile(path)
		if err != nil {
			return 0, err
		}
		s := strings.TrimSpace(string(data))
		s = strings.TrimPrefix(s, "0x")
		val, err := strconv.ParseUint(s, 16, 32)
		if err != nil {
			return 0, err
		}
		return val, nil
	}

	entries, err := os.ReadDir(pciDevicesPath)
	if err != nil {
		log.Errorf("Can't read pci dir: %v", err)
		return 0, err
	}

	var count uint
	for _, entry := range entries {
		devicePath := filepath.Join(pciDevicesPath, entry.Name())
		if _, err := os.Stat(filepath.Join(devicePath, "physfn")); err == nil {
			log.Debugf("Skip SR-IOV VF device: %s", devicePath)
			continue
		}
		vendorID, err := readHexFile(filepath.Join(devicePath, "vendor"))
		if err != nil {
			log.Warnf("Can't read vendor file: %v", err)
			continue
		}
		if uint16(vendorID) != targetVendorID {
			log.Debugf("VendorID not match 0x%x", vendorID)
			continue
		}
		classCode, err := readHexFile(filepath.Join(devicePath, "class"))
		if err != nil {
			log.Warnf("Can't read class file: %v", err)
			continue
		}
		classBase := uint8((classCode >> 16) & 0xFF)
		if classBase == targetClassBase {
			log.Debugf("Find mlu device: %s with vendorID 0x%x,classCode: 0x%x, classBase: 0x%x", devicePath, vendorID, classCode, classBase)
			count++
		}
	}

	log.Debugf("Find %d mlu devices", count)
	return count, nil
}

func isDriverRunning(counts uint) bool {
	for i := range counts {
		_, good, running, err := GetDeviceHealthState(i, 0)
		if err != nil {
			log.Warnf("GetDeviceHealth for slot %d with err %v", i, err)
			return false
		}
		if !good {
			log.Warnf("MLU device %d health maybe in problem, ignoring at init", i)
		}
		if !running {
			log.Warnf("MLU device %d driver is not running", i)
			return false
		}
	}
	return true
}

func EnsureMLUAllOk() {
	log.Infof("Start to ensure mlu driver status is ok")
	i := 1
	for {
		if i < 60000 {
			i = i << 1
		}
		time.Sleep(time.Duration(min(i+100, 60000)) * time.Millisecond)

		if i == 2 {
			if err := Init(false); err != nil {
				log.Errorf("Init cndev client failed with err: %v", err)
				continue
			}
		} else {
			if err := Init(true); err != nil {
				log.Errorf("Init cndev client failed with err: %v", err)
				continue
			}
		}

		if os.Getenv("INTEGRATION_TEST") == "true" {
			counts, _ := GetDeviceCount()
			generateDeviceHandleMap(counts)
			log.Info("Skip ensuring mlu driver status is ok for integration test")
			return
		}

		log.Debug("Start GetDeviceCount")
		counts, err := GetDeviceCount()
		if err != nil {
			log.Errorf("GetDeviceCount failed %v", err)
			continue
		}
		if counts == 0 {
			log.Warn("No MLU device found with GetDeviceCount")
			continue
		}
		log.Debugf("Device counts: %d", counts)

		realCounts, err := FetchMLUCounts()
		if err != nil {
			log.Errorf("FetchMLUCounts failed %v", err)
			continue
		}
		log.Debugf("RealCounts is :%d ", realCounts)

		if counts != realCounts {
			log.Warnf("MLU device count not match, counts: %d, realCounts: %d", counts, realCounts)
			continue
		}
		log.Infof("MLU device count match, count is %d", counts)

		if err := generateDeviceHandleMap(counts); err != nil {
			log.Panicf("Generate Device Handle Map failed, this should never happen, counts: %d, err: %v", counts, err)
		}

		if !isDriverRunning(counts) {
			continue
		}

		log.Info("Driver of MLU devices are all running")
		return
	}
}

func EnsureCndevLib() error {
	var src string
	x86Src := "/host/usr/lib/x86_64-linux-gnu/libcndev.so"
	armSrc := "/host/usr/lib/aarch64-linux-gnu/libcndev.so"
	lib64Src := "/host/usr/lib64/libcndev.so"
	if _, err := os.Stat(x86Src); err == nil {
		src = x86Src
		log.Infof("Found libcndev.so on host: %s", x86Src)
	} else if _, err := os.Stat(armSrc); err == nil {
		src = armSrc
		log.Infof("Found libcndev.so on host: %s", armSrc)
	} else if _, err := os.Stat(lib64Src); err == nil {
		src = lib64Src
		log.Infof("Found libcndev.so on host: %s", lib64Src)
	} else {
		log.Info("Found no libcndev.so on host, use default")
		return nil
	}

	dst := "/usr/lib/libcndev.so"
	log.Info("Found libcndev.so in host, try to copy to /usr/lib")
	sourceFile, err := os.Open(src)
	if err != nil {
		log.Errorf("Can't open %s", src)
		return err
	}
	defer sourceFile.Close()

	destinationFile, err := os.Create(dst)
	if err != nil {
		log.Errorf("Can't create %s", dst)
		return err
	}
	defer destinationFile.Close()

	_, err = io.Copy(destinationFile, sourceFile)
	return err
}
