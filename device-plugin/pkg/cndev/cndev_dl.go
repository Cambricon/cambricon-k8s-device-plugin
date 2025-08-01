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

// #include <dlfcn.h>
// #include "include/cndev.h"
import "C"

import (
	"log"
	"unsafe"
)

type dlhandles struct{ handles []unsafe.Pointer }

var dl dlhandles

// Initialize CNDEV, open a dynamic reference to the CNDEV library in the process.
func (dl *dlhandles) cndevInit() C.cndevRet_t {
	lib := C.CString("libcndev.so")
	defer C.free(unsafe.Pointer(lib))

	handle := C.dlopen(lib, C.RTLD_LAZY|C.RTLD_GLOBAL)
	if handle == C.NULL {
		log.Printf("Open libcndev with err:%s", C.GoString(C.dlerror()))
		return C.CNDEV_ERROR_UNINITIALIZED
	}
	dl.handles = append(dl.handles, handle)
	return C.cndevInit(C.int(0))
}

// Release CNDEV, close the dynamic reference to the CNDEV library in the process.
func (dl *dlhandles) cndevRelease() C.cndevRet_t {
	ret := C.cndevRelease()
	if ret != C.CNDEV_SUCCESS {
		return ret
	}

	for _, handle := range dl.handles {
		err := C.dlclose(handle)
		if err != 0 {
			return C.CNDEV_ERROR_UNKNOWN
		}
	}
	return C.CNDEV_SUCCESS
}

// Check cndev funcs exist in current '.so'
func (dl *dlhandles) checkExist(cndevFunc ...string) C.cndevRet_t {
	for _, funcName := range cndevFunc {
		cFunc := C.CString(funcName)
		defer C.free(unsafe.Pointer(cFunc))

		if len(dl.handles) == 0 {
			return C.CNDEV_ERROR_UNINITIALIZED
		}
		exist := C.dlsym(dl.handles[len(dl.handles)-1], cFunc)
		if exist == C.NULL {
			log.Printf("can't find %s in libcndev:%s", cndevFunc, C.GoString(C.dlerror()))
			return C.CNDEV_ERROR_NOT_SUPPORTED
		}
	}

	return C.CNDEV_SUCCESS
}
