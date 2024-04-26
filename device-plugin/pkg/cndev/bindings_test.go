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
	"sort"
	"testing"

	"github.com/stretchr/testify/assert"
)

func TestMain(m *testing.M) {
	err := Init(false, nil)
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

func TestGetDeviceModel(t *testing.T) {
	model := GetDeviceModel(uint(0))
	assert.Equal(t, "MLU290", model)
}

func TestGetDeviceMemory(t *testing.T) {
	memory, err := GetDeviceMemory(uint(0))
	assert.NoError(t, err)
	assert.Equal(t, uint(16*1024), memory)
}

func TestGetDeviceInfo(t *testing.T) {
	uuid, _, mb, path, err := getDeviceInfo(uint(1))
	assert.NoError(t, err)
	assert.Equal(t, "/dev/cambricon_dev1", path)
	assert.Equal(t, "MLU-20001012-1916-0000-0000-000000000000", uuid)
	assert.Equal(t, fmt.Sprintf("%x", 1111111), mb)
}

func TestGetDeviceHealthState(t *testing.T) {
	health, err := getDeviceHealthState(uint(0), 1)
	assert.NoError(t, err)
	assert.Equal(t, 1, health)
}

func TestGetDeviceXidError(t *testing.T) {
	hasXidError, err := getDeviceXidError(uint(0))
	assert.NoError(t, err)
	assert.Equal(t, true, hasXidError)
}

func TestGetDeviceMLULinkDevs(t *testing.T) {
	devs, err := getDeviceMLULinkDevs(uint(0))
	assert.NoError(t, err)
	assert.Equal(t, map[string]int{
		"MLU-20001012-1916-0000-0000-000000000000": 1,
		"MLU-30001012-1916-0000-0000-000000000000": 2,
		"MLU-40001012-1916-0000-0000-000000000000": 1,
		"MLU-50001012-1916-0000-0000-000000000000": 1,
		"MLU-d0001012-1916-0000-0000-000000000000": 1,
	}, devs)
}

func TestGetMLULinkGroups(t *testing.T) {
	groups, err := GetMLULinkGroups()
	assert.NoError(t, err)
	for i := range groups {
		sort.Slice(groups[i], func(x, y int) bool {
			return groups[i][x] < groups[i][y]
		})
	}
	assert.Equal(t, [][]uint{{0, 1, 2, 3, 4, 5, 6, 7}}, groups)
}

func TestGetAllMluInstanceInfo(t *testing.T) {
	expectInfos := []MimInfo{
		{
			DevNodeName:     "/dev/cambricon-caps/cap_dev0_mi1",
			IpcmDevNodeName: "/dev/cambricon_ipcm0",
			Name:            "2m.16gb",
			UUID:            "MLU-B0001012-1916-0000-0000-000000000000",
		},
		{
			DevNodeName:     "/dev/cambricon-caps/cap_dev0_mi3",
			IpcmDevNodeName: "/dev/cambricon_ipcm0",
			Name:            "4m.32gb",
			UUID:            "MLU-C0001012-1916-0000-0000-000000000000",
		},
		{
			DevNodeName:     "/dev/cambricon-caps/cap_dev0_mi5",
			IpcmDevNodeName: "/dev/cambricon_ipcm0",
			Name:            "4m.16gb",
			UUID:            "MLU-D0001012-1916-0000-0000-000000000000",
		},
	}
	infos, err := GetAllMluInstanceInfo(0)
	assert.NoError(t, err)
	assert.Equal(t, infos, expectInfos)
}

func TestGetAllSMluInfo(t *testing.T) {
	expectInfos := []SmluInfo{
		{
			DevNodeName: "/dev/cambricon-caps/cap_dev0_mi1",
			Name:        "12.000m.78.784gb",
			UUID:        "MLU-B0001012-1916-0000-0000-000000000000",
			ProfileID:   0,
			InstanceID:  1,
		},
		{
			DevNodeName: "/dev/cambricon-caps/cap_dev0_mi2",
			Name:        "6.000m.39.392gb",
			UUID:        "MLU-C0001012-1916-0000-0000-000000000000",
			ProfileID:   1,
			InstanceID:  2,
		},
		{
			DevNodeName: "/dev/cambricon-caps/cap_dev0_mi3",
			Name:        "3.000m.19.696gb",
			UUID:        "MLU-D0001012-1916-0000-0000-000000000000",
			ProfileID:   2,
			InstanceID:  3,
		},
	}
	infos, err := GetAllSmluInfo(0)
	assert.NoError(t, err)
	assert.Equal(t, infos, expectInfos)
}

func TestGetDeviceProfileInfo(t *testing.T) {
	expectInfos := []DsmluProfileInfo{
		{
			ProfileID: 0,
			Total:     4,
			Remain:    4,
			Memory:    1073741824,
			Quota:     100,
			Name:      "10p.80675mb",
		},
	}
	infos, err := GetDeviceProfileInfo(0)
	assert.NoError(t, err)
	assert.Equal(t, infos, expectInfos)
}
