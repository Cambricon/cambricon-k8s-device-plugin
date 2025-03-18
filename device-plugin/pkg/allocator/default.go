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
	"fmt"
	"sort"

	"github.com/Cambricon/cambricon-k8s-device-plugin/device-plugin/pkg/cndev"
	"github.com/Cambricon/cambricon-k8s-device-plugin/device-plugin/pkg/cntopo"
	log "github.com/sirupsen/logrus"
)

type defaultAllocator struct {
	policy string
	cntopo cntopo.Cntopo
	devs   map[string]*cndev.Device
}

func NewDefaultAllocator(policy string, devs map[string]*cndev.Device) Allocator {
	return &defaultAllocator{
		policy: policy,
		cntopo: cntopo.New(),
		devs:   devs,
	}
}

func (a *defaultAllocator) Allocate(available []uint, required []uint, size int) ([]uint, error) {
	// only for 8-mlu machine
	if len(available) > 0 && size == 1 || len(available) == size && size == 8 {
		return available[0:size], nil
	}

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
		if len(rings) == 0 {
			log.Printf("found no rings for %v", available)
			if a.policy != bestEffort {
				return nil, fmt.Errorf("mode %s found no rings", a.policy)
			}
			return available[0:size], nil
		}
		return rings[0].Ordinals, nil
	}
}
