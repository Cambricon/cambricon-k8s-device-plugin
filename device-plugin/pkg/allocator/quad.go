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
	"fmt"
	"sort"

	"github.com/Cambricon/cambricon-k8s-device-plugin/device-plugin/pkg/cndev"
	log "github.com/sirupsen/logrus"
)

// quadAllocator prefers MLULink fully-connected 4-card groups, then two such
// groups that share a PCIe switch (an 8-card domain). This matches MLU580-X6
// 16-card machines where cntopo RING often returns no rings, for example:
// quads {0,1,12,13} and {2,3,14,15} are MLULink-meshed and the pair is SGL-linked.
type quadAllocator struct {
	policy  string
	devs    map[string]*cndev.Device
	groups  [][]uint
	domains [][]uint
}

func NewQuadAllocator(policy string, devs map[string]*cndev.Device) Allocator {
	groups := getCPUGroups()
	domains := PairQuadDomains(groups)
	log.Printf("quad allocator groups %v domains %v", groups, domains)
	return &quadAllocator{
		policy:  policy,
		devs:    devs,
		groups:  groups,
		domains: domains,
	}
}

func (a *quadAllocator) Allocate(available []uint, _ []uint, size int) ([]uint, error) {
	if size <= 0 || size > len(available) {
		return nil, fmt.Errorf("invalid allocate size %d for available %v", size, available)
	}

	groups := filterDevSets(a.groups, available)
	domains := filterDevSets(a.domains, available)
	sortDevSets(groups)
	sortDevSets(domains)
	log.Printf("quad allocate available %v size %d groups %v domains %v", available, size, groups, domains)

	allocated := pickByLinkTopology(groups, domains, available, size)
	if len(allocated) != size {
		if a.policy != bestEffort {
			return nil, fmt.Errorf("mode %s failed to allocate %d from %v", a.policy, size, available)
		}
		allocated = append([]uint{}, available[:size]...)
	}

	if a.policy == restricted && !isRestrictedQuad(allocated, size, groups, domains) {
		return nil, fmt.Errorf("mode %s allocated %v is not the nearest topology", a.policy, allocated)
	}
	if a.policy == guaranteed && !isGuaranteedQuad(allocated, size, groups, domains) {
		return nil, fmt.Errorf("mode %s allocated %v cannot form required topology", a.policy, allocated)
	}
	return allocated, nil
}

// PairQuadDomains pairs size-4 MLULink groups by ascending min slot.
// On MLU580-X6 this yields the SGL-connected 8-card sets such as
// {0,1,12,13} + {2,3,14,15}.
func PairQuadDomains(groups [][]uint) [][]uint {
	var quads [][]uint
	var others [][]uint
	for _, g := range groups {
		cp := append([]uint{}, g...)
		if len(cp) == 4 {
			quads = append(quads, cp)
			continue
		}
		if len(cp) > 0 {
			others = append(others, cp)
		}
	}
	sortDevSets(quads)

	var domains [][]uint
	for i := 0; i+1 < len(quads); i += 2 {
		d := append(append([]uint{}, quads[i]...), quads[i+1]...)
		domains = append(domains, d)
	}
	if len(quads)%2 == 1 {
		domains = append(domains, append([]uint{}, quads[len(quads)-1]...))
	}
	domains = append(domains, others...)
	return domains
}

func pickByLinkTopology(groups, domains [][]uint, available []uint, size int) []uint {
	if size <= 4 {
		for _, g := range groups {
			if len(g) >= size {
				return append([]uint{}, g[:size]...)
			}
		}
		for _, d := range domains {
			if len(d) >= size {
				if got := takeFromGroups(groups, d, size); len(got) == size {
					return got
				}
			}
		}
	}
	if size <= 8 {
		for _, d := range domains {
			if len(d) >= size {
				if got := takeFromGroups(groups, d, size); len(got) == size {
					return got
				}
			}
		}
	}
	if got := takeFromGroups(groups, nil, size); len(got) == size {
		return got
	}
	if size <= len(available) {
		out := append([]uint{}, available...)
		sort.Slice(out, func(i, j int) bool { return out[i] < out[j] })
		return out[:size]
	}
	return nil
}

func takeFromGroups(groups [][]uint, limit []uint, size int) []uint {
	allocated := []uint{}
	appendFrom := func(src []uint, onlyFull bool) bool {
		if limit != nil && !isSubset(src, limit) {
			return false
		}
		if onlyFull && len(src) != 4 {
			return false
		}
		for _, d := range src {
			if contains(allocated, d) {
				continue
			}
			allocated = append(allocated, d)
			if len(allocated) == size {
				return true
			}
		}
		return false
	}
	if size >= 4 {
		for _, g := range groups {
			if appendFrom(g, true) {
				return allocated
			}
		}
	}
	for _, g := range groups {
		if appendFrom(g, false) {
			return allocated
		}
	}
	if limit != nil {
		for _, d := range limit {
			if contains(allocated, d) {
				continue
			}
			allocated = append(allocated, d)
			if len(allocated) == size {
				return allocated
			}
		}
	}
	return allocated
}

func isRestrictedQuad(allocated []uint, size int, groups, domains [][]uint) bool {
	switch size {
	case 1:
		return len(allocated) == 1
	case 2:
		return containedInAny(allocated, groups)
	case 4:
		return containedInExactSize(allocated, groups, 4)
	case 8:
		return containedInExactSize(allocated, domains, 8)
	default:
		return false
	}
}

func isGuaranteedQuad(allocated []uint, size int, groups, domains [][]uint) bool {
	switch size {
	case 1:
		return len(allocated) == 1
	case 2, 4:
		return containedInAny(allocated, groups)
	case 8:
		if containedInAny(allocated, domains) {
			return true
		}
		return countCoveringGroups(allocated, groups) >= 2
	default:
		return false
	}
}

func filterDevSets(sets [][]uint, available []uint) [][]uint {
	var out [][]uint
	for _, set := range sets {
		filtered := []uint{}
		for _, d := range set {
			if contains(available, d) {
				filtered = append(filtered, d)
			}
		}
		if len(filtered) == 0 {
			continue
		}
		sort.Slice(filtered, func(i, j int) bool { return filtered[i] < filtered[j] })
		out = append(out, filtered)
	}
	return out
}

func sortDevSets(sets [][]uint) {
	sort.SliceStable(sets, func(i, j int) bool {
		if len(sets[i]) != len(sets[j]) {
			return len(sets[i]) < len(sets[j])
		}
		return minSlot(sets[i]) < minSlot(sets[j])
	})
}

func minSlot(set []uint) uint {
	if len(set) == 0 {
		return 0
	}
	m := set[0]
	for _, s := range set[1:] {
		if s < m {
			m = s
		}
	}
	return m
}

func isSubset(sub, set []uint) bool {
	for _, d := range sub {
		if !contains(set, d) {
			return false
		}
	}
	return true
}

func containedInAny(devs []uint, sets [][]uint) bool {
	for _, set := range sets {
		if isSubset(devs, set) {
			return true
		}
	}
	return false
}

func containedInExactSize(devs []uint, sets [][]uint, n int) bool {
	for _, set := range sets {
		if len(set) == n && isSubset(devs, set) && len(devs) == n {
			return true
		}
	}
	return false
}

func countCoveringGroups(devs []uint, groups [][]uint) int {
	n := 0
	for _, g := range groups {
		hit := 0
		for _, d := range g {
			if contains(devs, d) {
				hit++
			}
		}
		if hit == len(g) && len(g) == 4 {
			n++
		}
	}
	return n
}

func uniqueSlots(devs map[string]*cndev.Device) int {
	seen := map[uint]struct{}{}
	for _, d := range devs {
		seen[d.Slot] = struct{}{}
	}
	return len(seen)
}
