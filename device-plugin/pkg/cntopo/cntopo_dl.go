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

package cntopo

// #include <dlfcn.h>
// #include "include/cntopo.h"
import "C"

import (
	"log"
	"unsafe"
)

type dlhandles struct{ handles []unsafe.Pointer }

var dl dlhandles

var ctx C.cntopoContext_t

const machineLabel = "localhost"

// Initialize CNTOPO, open a dynamic reference to the CNTOPO library in the process.
func (dl *dlhandles) cntopoInit() C.cntopoResult_t {
	lib := C.CString("libcntopo.so")
	defer C.free(unsafe.Pointer(lib))

	handle := C.dlopen(lib, C.RTLD_LAZY|C.RTLD_GLOBAL)
	if handle == C.NULL {
		log.Printf("Open libcntopo with err:%s", C.GoString(C.dlerror()))
		return C.CNTOPO_CONTEXT_NOT_INIT
	}
	dl.handles = append(dl.handles, handle)
	r := C.cntopoInitContext(&ctx)
	if err := errorString(r); err != nil {
		return r
	}
	var nodeInfos C.cntopoMachineInfo_t
	var sizeBytes int
	r = C.cntopoGetLocalMachineInfo(ctx, (*C.cntopoMachineInfo_t)(unsafe.Pointer(&nodeInfos)), (*C.size_t)(unsafe.Pointer(&sizeBytes)))
	if err := errorString(r); err != nil {
		return r
	}

	ml := C.CString(machineLabel)
	defer C.free(unsafe.Pointer(ml))
	return C.cntopoAddMachineInfo(ctx, nodeInfos, ml)
}

// Release CNTOPO, close the dynamic reference to the CNTOPO library in the process.
func (dl *dlhandles) cntopoRelease() C.cntopoResult_t {
	ret := C.cntopoDestroyContext(ctx)
	if ret != C.CNTOPO_RET_SUCCESS {
		return ret
	}

	for _, handle := range dl.handles {
		if err := C.dlclose(handle); err != C.CNTOPO_RET_SUCCESS {
			return C.CNTOPO_INTERNEL_ERROR
		}
	}

	return C.CNTOPO_RET_SUCCESS
}
