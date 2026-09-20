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
	"sort"

	"github.com/Cambricon/cambricon-k8s-device-plugin/device-plugin/pkg/cndev"
	. "github.com/onsi/ginkgo"
	. "github.com/onsi/ginkgo/extensions/table"
	. "github.com/onsi/gomega"
)

func mlu580X6Groups() [][]uint {
	return [][]uint{
		{0, 1, 12, 13},
		{2, 3, 14, 15},
		{4, 5, 8, 9},
		{6, 7, 10, 11},
	}
}

func mlu580X6Devs() map[string]*cndev.Device {
	devs := map[string]*cndev.Device{}
	for i := uint(0); i < 16; i++ {
		numa := 0
		if i >= 8 {
			numa = 1
		}
		id := "MLU-" + string(rune('a'+i))
		devs[id] = &cndev.Device{UUID: id, Slot: i, Numa: numa}
	}
	return devs
}

func sortedCopy(in []uint) []uint {
	out := append([]uint{}, in...)
	sort.Slice(out, func(i, j int) bool { return out[i] < out[j] })
	return out
}

var _ = Describe("Quad Allocator", func() {
	Context("PairQuadDomains", func() {
		It("pairs adjacent 4-card MLULink groups into SGL 8-card domains", func() {
			domains := PairQuadDomains(mlu580X6Groups())
			Expect(domains).To(HaveLen(2))
			Expect(sortedCopy(domains[0])).To(Equal([]uint{0, 1, 2, 3, 12, 13, 14, 15}))
			Expect(sortedCopy(domains[1])).To(Equal([]uint{4, 5, 6, 7, 8, 9, 10, 11}))
		})
	})

	Context("Allocate", func() {
		var allocator *quadAllocator

		BeforeEach(func() {
			groups := mlu580X6Groups()
			allocator = &quadAllocator{
				policy:  bestEffort,
				devs:    mlu580X6Devs(),
				groups:  groups,
				domains: PairQuadDomains(groups),
			}
		})

		DescribeTable("Allocation Devices",
			func(policy string, available []uint, size int, expected []uint) {
				allocator.policy = policy
				got, err := allocator.Allocate(available, nil, size)
				if expected != nil {
					Expect(err).NotTo(HaveOccurred())
					Expect(sortedCopy(got)).To(Equal(sortedCopy(expected)))
				} else {
					Expect(err).To(HaveOccurred())
					Expect(got).To(BeNil())
				}
			},
			Entry("size 4 prefers MLULink quad 0,1,12,13",
				bestEffort,
				[]uint{0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15},
				4,
				[]uint{0, 1, 12, 13},
			),
			Entry("size 8 prefers two SGL-paired quads even if they cross CPU",
				bestEffort,
				[]uint{0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15},
				8,
				[]uint{0, 1, 2, 3, 12, 13, 14, 15},
			),
			Entry("size 2 packs inside the first MLULink quad",
				bestEffort,
				[]uint{0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15},
				2,
				[]uint{0, 1},
			),
			Entry("size 4 skips a partial quad and takes the next full MLULink quad",
				bestEffort,
				[]uint{2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15},
				4,
				[]uint{2, 3, 14, 15},
			),
			Entry("size 2 packs remaining cards in a partial quad",
				bestEffort,
				[]uint{2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15},
				2,
				[]uint{12, 13},
			),
			Entry("size 8 still prefers a complete SGL domain when one quad is partial",
				bestEffort,
				[]uint{2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15},
				8,
				[]uint{4, 5, 6, 7, 8, 9, 10, 11},
			),
			Entry("restricted size 4 fails when no full MLULink quad remains",
				restricted,
				[]uint{0, 1, 2, 3, 4, 5, 6, 7},
				4,
				nil,
			),
			Entry("guaranteed size 4 succeeds with a full MLULink quad",
				guaranteed,
				[]uint{0, 1, 2, 3, 4, 5, 6, 7, 12, 13, 14, 15},
				4,
				[]uint{0, 1, 12, 13},
			),
			Entry("restricted size 8 succeeds with an SGL-paired domain",
				restricted,
				[]uint{0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15},
				8,
				[]uint{0, 1, 2, 3, 12, 13, 14, 15},
			),
			Entry("best effort size 4 on same-CPU SGL cards does not pick 0,1,2,3",
				bestEffort,
				[]uint{0, 1, 2, 3, 12, 13, 14, 15},
				4,
				[]uint{0, 1, 12, 13},
			),
		)
	})
})
