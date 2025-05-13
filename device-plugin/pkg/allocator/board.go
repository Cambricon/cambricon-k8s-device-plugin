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
	"context"
	"errors"
	"fmt"
	"sort"

	"github.com/Cambricon/cambricon-k8s-device-plugin/device-plugin/pkg/cndev"
	"github.com/Cambricon/cambricon-k8s-device-plugin/device-plugin/pkg/cntopo"
	log "github.com/sirupsen/logrus"
)

type boardAllocator struct {
	policy string
	cntopo cntopo.Cntopo
	devs   map[string]*cndev.Device
	groups [][]uint
}

func NewBoardAllocator(policy string, devs map[string]*cndev.Device) Allocator {
	return &boardAllocator{
		policy: policy,
		cntopo: cntopo.New(),
		devs:   devs,
		groups: getCPUGroups(),
	}
}

func (a *boardAllocator) Allocate(available []uint, _ []uint, size int) ([]uint, error) {
	ctx, cancel := context.WithTimeout(context.Background(), getRingTimeout)
	defer cancel()

	resultChan := make(chan []cntopo.Ring)
	errorChan := make(chan error)

	go func() {
		rings, err := a.cntopo.GetRings(available, size)
		if err != nil {
			errorChan <- err
			return
		}
		resultChan <- rings
	}()

	select {
	case <-ctx.Done():
		log.Warnf("get rings timeout for %v", available)
		if a.policy != bestEffort {
			return nil, ctx.Err()
		}
		return available[0:size], nil
	case err := <-errorChan:
		return nil, err
	case rings := <-resultChan:
		sort.Slice(rings, func(i int, j int) bool {
			return rings[i].NonConflictRingNum > rings[j].NonConflictRingNum
		})
		boards := splitByBoards(available, a.devs)
		groups, err := a.filterAvaliableDevsByGroup(available)
		if err != nil {
			log.Printf("failed to filter %v by group %v, err: %v, maybe in pcie mode, ignore when allocating", available, a.groups, err)
		}
		log.Printf("available devs filtered by group: %v", groups)
		if len(rings) == 0 {
			log.Printf("found no rings for %v", available)
			if a.policy != bestEffort && !a.sizeAlwaysFailsToFormRing(size) {
				return nil, fmt.Errorf("mode %s found no rings for size %d", a.policy, size)
			}
			needed := size
			allocated := []uint{}
			allocateRemainingFrom := func(devices []uint) bool {
				for _, device := range devices {
					if contains(allocated, device) {
						continue
					}
					allocated = append(allocated, device)
					needed--
					if needed == 0 {
						return true
					}
				}
				return false
			}
			if groups == nil {
				for _, board := range boards {
					if allocateRemainingFrom(board) {
						return allocated, nil
					}
				}
			}
			for _, group := range groups {
				for _, board := range boards {
					if containsAll(group, board) {
						if allocateRemainingFrom(board) {
							return allocated, nil
						}
					}
				}
			}
			if allocateRemainingFrom(available) {
				return allocated, nil
			}
			return nil, errors.New("allocated from all available devices, should not be here")
		}
		if a.policy == restricted && size == 2 && rings[0].NonConflictRingNum < 2 {
			return nil, fmt.Errorf("mode %s, max non-conflict ring num %d", a.policy, rings[0].NonConflictRingNum)
		}
		candidates := rings
		for i, ring := range rings {
			if ring.NonConflictRingNum < rings[0].NonConflictRingNum {
				candidates = rings[0:i]
				break
			}
		}
		for _, group := range groups {
			for _, candidate := range candidates {
				if containsAll(group, candidate.Ordinals) {
					return candidate.Ordinals, nil
				}
			}
		}
		return candidates[0].Ordinals, nil
	}
}

func (a *boardAllocator) filterAvaliableDevsByGroup(available []uint) ([][]uint, error) {
	if len(a.groups) == 0 {
		return nil, fmt.Errorf("no groups available in allocator")
	}
	groups := make([][]uint, len(a.groups))
	for _, dev := range available {
		found := false
		for i, group := range a.groups {
			if contains(group, dev) {
				groups[i] = append(groups[i], dev)
				found = true
				break
			}
		}
		if !found {
			return nil, fmt.Errorf("dev %d not in any group %v", dev, a.groups)
		}
	}
	sort.SliceStable(groups, func(i, j int) bool {
		return len(groups[i]) < len(groups[j])
	})
	return groups, nil
}

func (a *boardAllocator) sizeAlwaysFailsToFormRing(size int) bool {
	if size > 8 || size <= 1 {
		return true
	}
	if size%2 == 1 {
		return true
	}
	return false
}

func splitByBoards(available []uint, devs map[string]*cndev.Device) [][]uint {
	boards := make(map[string][]uint)
	for _, dev := range devs {
		if !contains(available, dev.Slot) {
			continue
		}
		boards[dev.SN] = append(boards[dev.SN], dev.Slot)
	}
	log.Debugf("available devices seperated by board %v", boards)
	res := [][]uint{}
	for _, board := range boards {
		res = append(res, board)
	}
	sort.Slice(res, func(i, j int) bool {
		return len(res[i]) < len(res[j])
	})
	log.Debugf("sorted available devices seperated by board %v", res)
	return res
}

func getCPUGroups() [][]uint {
	groups, err := cndev.GetMLULinkGroups()
	if err != nil {
		log.Printf("failed to get CPU groups, err %v", err)
		return nil
	}
	return groups
}
