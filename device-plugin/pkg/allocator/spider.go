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

type spiderAllocator struct {
	policy string
	cntopo cntopo.Cntopo
	devs   map[string]*cndev.Device
}

func NewSpiderAllocator(policy string, devs map[string]*cndev.Device) Allocator {
	return &spiderAllocator{
		policy: policy,
		cntopo: cntopo.New(),
		devs:   devs,
	}
}

func (a *spiderAllocator) Allocate(available []uint, _ []uint, size int) ([]uint, error) {
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
		mbs := splitByMotherBoards(available, a.devs)
		if len(rings) == 0 {
			log.Printf("found no rings for %v", available)
			if a.policy != bestEffort && !a.sizeAlwaysFailsToFormRing(size) {
				return nil, fmt.Errorf("mode %s found no rings", a.policy)
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
			for _, mb := range mbs {
				if allocateRemainingFrom(mb) {
					return allocated, nil
				}
			}
			return nil, errors.New("finished allocateRemainingFrom, should not be here")
		}
		if a.policy == restricted && size == 4 && rings[0].NonConflictRingNum < 4 {
			return nil, fmt.Errorf("mode %s, max non-conflict ring num %d", a.policy, rings[0].NonConflictRingNum)
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
		for _, mb := range mbs {
			for _, candidate := range candidates {
				if containsAll(mb, candidate.Ordinals) {
					return candidate.Ordinals, nil
				}
			}
		}
		return candidates[0].Ordinals, nil
	}
}

func (a *spiderAllocator) sizeAlwaysFailsToFormRing(size int) bool {
	if size <= 1 || size > 8 {
		return true
	}
	return false
}

func splitByMotherBoards(available []uint, devs map[string]*cndev.Device) [][]uint {
	motherBoards := make(map[string][]uint)
	for _, dev := range devs {
		if !contains(available, dev.Slot) {
			continue
		}
		motherBoards[dev.MotherBoard] = append(motherBoards[dev.MotherBoard], dev.Slot)
	}
	log.Printf("available devices seperated by mother board %v", motherBoards)
	mbs := [][]uint{}
	for _, mb := range motherBoards {
		mbs = append(mbs, mb)
	}
	sort.Slice(mbs, func(i int, j int) bool {
		return len(mbs[i]) < len(mbs[j])
	})
	log.Printf("sorted available devices seperated by mother board %v", mbs)
	return mbs
}
