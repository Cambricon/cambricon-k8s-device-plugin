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

package allocator

import (
	"strings"
	"time"

	"github.com/Cambricon/cambricon-k8s-device-plugin/device-plugin/pkg/cndev"
)

const (
	bestEffort string = "best-effort"
	restricted string = "restricted"
	guaranteed string = "guaranteed"
)

var getRingTimeout = 60 * time.Second

type Allocator interface {
	Allocate(available []uint, required []uint, size int) ([]uint, error)
}

func New(policy string, devs map[string]*cndev.Device) Allocator {
	model := Reverse(cndev.GetDeviceModel(uint(0)))
	if strings.Contains(model, "092U") || strings.Contains(model, "8M-073U") {
		return NewSpiderAllocator(policy, devs)
	}
	if strings.Contains(model, "8X-073U") || strings.Contains(model, "8H-095U") {
		return NewBoardAllocator(policy, devs)
	}
	return NewDefaultAllocator(policy, devs)
}

func contains(set []uint, dev uint) bool {
	for i := range set {
		if set[i] == dev {
			return true
		}
	}
	return false
}

func containsAll(set []uint, devs []uint) bool {
	for _, dev := range devs {
		if !contains(set, dev) {
			return false
		}
	}
	return true
}

func Reverse(s string) string {
	b := []byte(s)
	for i, j := 0, len(b)-1; i < j; i, j = i+1, j-1 {
		b[i], b[j] = b[j], b[i]
	}
	return string(b)
}
