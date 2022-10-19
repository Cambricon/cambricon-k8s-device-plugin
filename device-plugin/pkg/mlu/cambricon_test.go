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

package mlu

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
	assert.Equal(t, "MLU-40001012-1916-0000-0000-000000000000", devs[3].ID)
	assert.Equal(t, 8, len(devsInfo))
	assert.Equal(t, uint(3), devsInfo["MLU-40001012-1916-0000-0000-000000000000"].Slot)
	assert.Equal(t, "/dev/cambricon_dev3", devsInfo["MLU-40001012-1916-0000-0000-000000000000"].Path)

	devs, devsInfo = getDevices(envShare, 2)
	assert.Equal(t, 16, len(devs))
	assert.Equal(t, "MLU-20001012-1916-0000-0000-000000000000-_-2", devs[3].ID)
	assert.Equal(t, 16, len(devsInfo))
	assert.Equal(t, uint(1), devsInfo["MLU-20001012-1916-0000-0000-000000000000-_-2"].Slot)
	assert.Equal(t, "/dev/cambricon_dev1", devsInfo["MLU-20001012-1916-0000-0000-000000000000-_-2"].Path)
}

func TestGenerateFakeDevs(t *testing.T) {
	d := &cndev.Device{
		Slot: 1,
		UUID: "MLU-20001012-1916-0000-0000-000000000000",
		Path: "/dev/cambricon_dev1",
	}
	devs, devsInfo := generateFakeDevs(d, 4, true)
	assert.Equal(t, 4, len(devs))
	assert.Equal(t, fmt.Sprintf("%s--fake--4", d.UUID), devs[3].ID)
	assert.Equal(t, 4, len(devsInfo))
	assert.Equal(t, "/dev/cambricon_dev1vf4", devsInfo[fmt.Sprintf("%s--fake--4", d.UUID)].Path)
	assert.Equal(t, uint(1), devsInfo[fmt.Sprintf("%s--fake--4", d.UUID)].Slot)
	devs, devsInfo = generateFakeDevs(d, 2, false)
	assert.Equal(t, 2, len(devs))
	assert.Equal(t, fmt.Sprintf("%s-_-2", d.UUID), devs[1].ID)
	assert.Equal(t, 2, len(devsInfo))
	assert.Equal(t, "/dev/cambricon_dev1", devsInfo[fmt.Sprintf("%s-_-2", d.UUID)].Path)
	assert.Equal(t, uint(1), devsInfo[fmt.Sprintf("%s-_-2", d.UUID)].Slot)
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
