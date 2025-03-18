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

package cntopo

// #cgo LDFLAGS: -ldl -Wl,--export-dynamic -Wl,--unresolved-symbols=ignore-in-object-files
// #include "include/cntopo.h"
import "C"

import (
	"errors"
	"fmt"
	"sync"
	"unsafe"

	log "github.com/sirupsen/logrus"
)

type cntopo struct {
	sync.Mutex
}

type Ring struct {
	Ordinals           []uint
	NonConflictRingNum int
}

type Cntopo interface {
	GetRings(available []uint, size int) ([]Ring, error)
}

func Init() error {
	r := dl.cntopoInit()
	if r == C.CNTOPO_CONTEXT_NOT_INIT {
		return errors.New("could not load CNTOPO library")
	}
	return errorString(r)
}

func Release() error {
	r := dl.cntopoRelease()
	return errorString(r)
}

func New() Cntopo {
	return &cntopo{}
}

func (c *cntopo) GetRings(available []uint, size int) ([]Ring, error) {
	ml := C.CString(machineLabel)
	defer C.free(unsafe.Pointer(ml))

	var queryHandle C.cntopoQuery_t
	r := C.cntopoCreateQuery(ctx, &queryHandle)
	if err := errorString(r); err != nil {
		return nil, err
	}
	defer C.cntopoDestroyQuery(queryHandle)

	r = C.cntopoSetDevNumFilter(queryHandle, ml, C.uint(size))
	if err := errorString(r); err != nil {
		return nil, err
	}
	for _, value := range available {
		r = C.cntopoSetWhitelistDevOrdinal(queryHandle, ml, C.size_t(value))
		if err := errorString(r); err != nil {
			return nil, err
		}
	}

	var numDevSet C.size_t
	var devSets *C.cntopoDevSet_t
	defer C.free(unsafe.Pointer(devSets))
	r = C.cntopoFindDevSets(queryHandle, C.RING, 1000000, &devSets, &numDevSet)
	if err := errorString(r); err != nil {
		return nil, err
	}
	devSetsResult := (*[100]C.cntopoDevSet_t)(unsafe.Pointer(devSets))[:int(numDevSet)]

	var rings []Ring
	var devSize C.size_t
	for i := 0; i < int(numDevSet); i++ {
		r = C.cntopoGetDevSetSize(devSetsResult[i], &devSize)
		if err := errorString(r); err != nil {
			return nil, err
		}
		var devOrdinals []uint
		for index := 0; index < int(devSize); index++ {
			var devInfo C.cntopoDevInfo_t
			C.cntopoGetDevInfoFromDevSet(devSetsResult[i], C.size_t(index), &devInfo)
			devOrdinals = append(devOrdinals, uint(devInfo.dev_ordinal))
		}
		var topos *C.cntopoTopo_t
		var numTopo C.size_t
		r = C.cntopoFindTopos(devSetsResult[i], C.RING, &topos, &numTopo)
		if err := errorString(r); err != nil {
			return nil, err
		}
		rings = append(rings, Ring{
			NonConflictRingNum: int(numTopo),
			Ordinals:           devOrdinals,
		})
	}

	log.Debugf("get rings %+v", rings)
	return rings, nil
}

func errorString(ret C.cntopoResult_t) error {
	if ret == C.CNTOPO_RET_SUCCESS {
		return nil
	}
	err := C.GoString(C.cntopoGetErrorStr(ret))
	return fmt.Errorf("cntopo: %v", err)
}
