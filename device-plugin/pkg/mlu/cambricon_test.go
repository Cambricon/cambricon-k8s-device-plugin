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
	"os"
	"testing"

	"github.com/Cambricon/cambricon-k8s-device-plugin/device-plugin/pkg/cndev"
	"github.com/agiledragon/gomonkey/v2"
	log "github.com/sirupsen/logrus"
	"github.com/stretchr/testify/assert"
)

func TestMain(m *testing.M) {
	stub := gomonkey.ApplyFunc(cndev.FetchMLUCounts, func() (uint, error) {
		return 8, nil
	})
	defer stub.Reset()
	cndev.EnsureMLUAllOk()
	if ret := m.Run(); ret != 0 {
		os.Exit(ret)
	}
	if err := cndev.Release(); err != nil {
		log.Fatal(err)
	}
}

func TestGetDevices(t *testing.T) {
	devsM, devsInfoM := GetDevices(Options{Mode: Default})
	devs := devsM[normalMlu]
	devsInfo := devsInfoM[normalMlu]
	assert.Equal(t, 8, len(devs))
	assert.Equal(t, "MLU-40001012-1916-0000-0000-000000000000", devs[3].ID)
	assert.Equal(t, 8, len(devsInfo))
	assert.Equal(t, uint(3), devsInfo["MLU-40001012-1916-0000-0000-000000000000"].Slot)
	assert.Equal(t, "/dev/cambricon_dev3", devsInfo["MLU-40001012-1916-0000-0000-000000000000"].Path)
	devsReal := devsM[realCounts]
	devsRealInfo := devsInfoM[realCounts]
	assert.Equal(t, 8, len(devsReal))
	assert.Equal(t, "real-mlu-counts-MLU-40001012-1916-0000-0000-000000000000", devsReal[3].ID)
	assert.Equal(t, 8, len(devsRealInfo))
	assert.Equal(t, uint(3), devsRealInfo["real-mlu-counts-MLU-40001012-1916-0000-0000-000000000000"].Slot)

	devsM, devsInfoM = GetDevices(Options{Mode: EnvShare, VirtualizationNum: 2})
	devs = devsM[normalMlu]
	devsInfo = devsInfoM[normalMlu]
	assert.Equal(t, 16, len(devs))
	assert.Equal(t, "MLU-20001012-1916-0000-0000-000000000000-_-2", devs[3].ID)
	assert.Equal(t, 16, len(devsInfo))
	assert.Equal(t, uint(1), devsInfo["MLU-20001012-1916-0000-0000-000000000000-_-2"].Slot)
	assert.Equal(t, "/dev/cambricon_dev1", devsInfo["MLU-20001012-1916-0000-0000-000000000000-_-2"].Path)

	devsM, devsInfoM = GetDevices(Options{Mode: Mim})
	devs = devsM[normalMlu]
	devsInfo = devsInfoM[normalMlu]
	assert.Equal(t, 6, len(devs))
	assert.Equal(t, "MLU-30001012-1916-0000-0000-000000000000", devs[0].ID)
	assert.Equal(t, 6, len(devsInfo))
	assert.Equal(t, uint(2), devsInfo["MLU-30001012-1916-0000-0000-000000000000"].Slot)
	assert.Equal(t, "/dev/cambricon_dev2", devsInfo["MLU-30001012-1916-0000-0000-000000000000"].Path)
	assert.Equal(t, 3, len(devsM["2m.16gb"]))
	assert.Equal(t, 1, len(devsM["4m.16gb"]))
	assert.Equal(t, 2, len(devsM["4m.32gb"]))
	assert.Equal(t, "MLU-10001012-1916-0000-0000-000000000000-mim-MLU-B0001012-1916-0000-0000-000000000000", devsM["2m.16gb"][0].ID)
	assert.Equal(t, "MLU-10001012-1916-0000-0000-000000000000-mim-MLU-B0001012-1916-0000-0000-000000000000", devsInfoM["2m.16gb"]["MLU-10001012-1916-0000-0000-000000000000-mim-MLU-B0001012-1916-0000-0000-000000000000"].UUID)
	assert.Equal(t, 3, len(devsInfoM["2m.16gb"]))
	assert.Equal(t, "2m.16gb", devsInfoM["2m.16gb"]["MLU-10001012-1916-0000-0000-000000000000-mim-MLU-B0001012-1916-0000-0000-000000000000"].Profile)
	assert.Equal(t, "/dev/cambricon_dev0,/dev/cambricon_ipcm0,/dev/cambricon-caps/cap_dev0_mi1", devsInfoM["2m.16gb"]["MLU-10001012-1916-0000-0000-000000000000-mim-MLU-B0001012-1916-0000-0000-000000000000"].Path)
	assert.Equal(t, "/dev/cambricon_dev0,/dev/cambricon_ipcm0,/dev/cambricon-caps/cap_dev0_mi3", devsInfoM["4m.32gb"]["MLU-10001012-1916-0000-0000-000000000000-mim-MLU-C0001012-1916-0000-0000-000000000000"].Path)

	devsM, _ = GetDevices(Options{Mode: DynamicSmlu, MinDsmluUnit: 256})
	assert.Equal(t, 800, len(devsM["vcore"]))
	assert.Equal(t, 512, len(devsM["vmemory"]))
}

func TestGenerateFakeDevs(t *testing.T) {
	d := &cndev.Device{
		Slot: 1,
		UUID: "MLU-20001012-1916-0000-0000-000000000000",
		Path: "/dev/cambricon_dev1",
	}
	devs, devsInfo := generateFakeDevs(d, 2, EnvShare)
	assert.Equal(t, 2, len(devs))
	assert.Equal(t, fmt.Sprintf("%s-_-2", d.UUID), devs[1].ID)
	assert.Equal(t, 2, len(devsInfo))
	assert.Equal(t, "/dev/cambricon_dev1", devsInfo[fmt.Sprintf("%s-_-2", d.UUID)].Path)
	assert.Equal(t, uint(1), devsInfo[fmt.Sprintf("%s-_-2", d.UUID)].Slot)
	dev := *d
	dev.Profile = "vcore"
	devs, devsInfo = generateFakeDevs(&dev, 100, DynamicSmlu)
	assert.Equal(t, 100, len(devs))
	assert.Equal(t, fmt.Sprintf("%s-%s-2", d.UUID, "vcore"), devs[1].ID)
	assert.Equal(t, 100, len(devsInfo))
	assert.Equal(t, "/dev/cambricon_dev1", devsInfo[fmt.Sprintf("%s-%s-2", d.UUID, "vcore")].Path)
	assert.Equal(t, uint(1), devsInfo[fmt.Sprintf("%s-%s-2", d.UUID, "vcore")].Slot)
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
