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

package main

import (
	"fmt"
	"log"
	"os"
	"testing"

	"github.com/Cambricon/cambricon-k8s-device-plugin/device-plugin/pkg/cndev"
	"github.com/stretchr/testify/assert"
)

func TestMain(m *testing.M) {
	err := cndev.Init()
	if err != nil {
		log.Fatal(err)
	}
	ret := m.Run()
	if ret != 0 {
		os.Exit(ret)
	}
	err = cndev.Release()
	if err != nil {
		log.Fatal(err)
	}
}

func TestGetDevices(t *testing.T) {

	devs, devsInfo := getDevices("default", 0)
	assert.Equal(t, 8, len(devs))
	assert.Equal(t, fmt.Sprintf("MLU-%x", 2701114), devs[3].ID)
	assert.Equal(t, 8, len(devsInfo))
	assert.Equal(t, uint(3), devsInfo[fmt.Sprintf("MLU-%x", 2701114)].Slot)
	assert.Equal(t, "/dev/cambricon_dev3", devsInfo[fmt.Sprintf("MLU-%x", 2701114)].Path)

	devs, devsInfo = getDevices(envShare, 2)
	assert.Equal(t, 16, len(devs))
	assert.Equal(t, fmt.Sprintf("MLU-%x-_-2", 2701112), devs[3].ID)
	assert.Equal(t, 16, len(devsInfo))
	assert.Equal(t, uint(1), devsInfo[fmt.Sprintf("MLU-%x-_-2", 2701112)].Slot)
	assert.Equal(t, "/dev/cambricon_dev1", devsInfo[fmt.Sprintf("MLU-%x-_-2", 2701112)].Path)
}

func TestGenerateFakeDevs(t *testing.T) {
	d := &cndev.Device{
		Slot: 1,
		UUID: fmt.Sprintf("MLU-%x", 2701112),
		Path: "/dev/cambricon_dev1",
	}
	devs, devsInfo := generateFakeDevs(d, 4, true)
	assert.Equal(t, 4, len(devs))
	assert.Equal(t, fmt.Sprintf("MLU-%x--fake--4", 2701112), devs[3].ID)
	assert.Equal(t, 4, len(devsInfo))
	assert.Equal(t, "/dev/cambricon_dev1vf4", devsInfo[fmt.Sprintf("MLU-%x--fake--4", 2701112)].Path)
	assert.Equal(t, uint(1), devsInfo[fmt.Sprintf("MLU-%x--fake--4", 2701112)].Slot)
	devs, devsInfo = generateFakeDevs(d, 2, false)
	assert.Equal(t, 2, len(devs))
	assert.Equal(t, fmt.Sprintf("MLU-%x-_-2", 2701112), devs[1].ID)
	assert.Equal(t, 2, len(devsInfo))
	assert.Equal(t, "/dev/cambricon_dev1", devsInfo[fmt.Sprintf("MLU-%x-_-2", 2701112)].Path)
	assert.Equal(t, uint(1), devsInfo[fmt.Sprintf("MLU-%x-_-2", 2701112)].Slot)
}

func TestHostDeviceExistsWithPrefix(t *testing.T) {
	filename := "/tmp/cambricon_dev0"
	prefix := "/tmp/cambricon_dev"
	_, err := os.Create(filename)
	assert.NoError(t, err)
	res := hostDeviceExistsWithPrefix(prefix)
	assert.Equal(t, res, true)
	err = os.Remove(filename)
	assert.NoError(t, err)
	res = hostDeviceExistsWithPrefix(prefix)
	assert.Equal(t, res, false)
}
