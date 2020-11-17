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

import (
	"fmt"
	"log"
	"os"
	"testing"

	"github.com/stretchr/testify/assert"
)

func TestMain(m *testing.M) {
	err := Init()
	if err != nil {
		log.Fatal(err)
	}
	ret := m.Run()
	if ret != 0 {
		os.Exit(ret)
	}
	err = Release()
	if err != nil {
		log.Fatal(err)
	}
}

func TestGetDeviceCount(t *testing.T) {
	count, err := GetDeviceCount()
	assert.NoError(t, err)
	assert.Equal(t, uint(8), count)
}

func TestGetDeviceUUID(t *testing.T) {
	uuid, err := getDeviceUUID(uint(0))
	assert.NoError(t, err)
	assert.Equal(t, fmt.Sprintf("MLU-%x", 2701111), uuid)
}

func TestGetDevicePath(t *testing.T) {
	path, err := getDevicePath(uint(1))
	assert.NoError(t, err)
	assert.Equal(t, "/dev/cambricon_dev1", path)
}

func TestGetDeviceHealthState(t *testing.T) {
	health, err := getDeviceHealthState(uint(0), 1)
	assert.NoError(t, err)
	assert.Equal(t, 1, health)
}

func TestGetDevicePCIeInfo(t *testing.T) {
	pcie, err := getDevicePCIeInfo(uint(0))
	assert.NoError(t, err)
	assert.Equal(t, 0, pcie.domain)
	assert.Equal(t, 12, pcie.bus)
	assert.Equal(t, 13, pcie.device)
	assert.Equal(t, 1, pcie.function)
}
