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
	"time"

	"github.com/Cambricon/cambricon-k8s-device-plugin/device-plugin/pkg/cndev"
	"github.com/Cambricon/cambricon-k8s-device-plugin/device-plugin/pkg/cntopo"
	. "github.com/onsi/ginkgo"
	. "github.com/onsi/ginkgo/extensions/table"
	. "github.com/onsi/gomega"
)

var _ = Describe("Default Allocator", func() {

	Context("Test allocate", func() {
		var (
			devsInfo  map[string]*cndev.Device
			allocator *defaultAllocator
		)

		BeforeEach(func() {
			devsInfo = map[string]*cndev.Device{
				"MLU-0": {
					UUID: "MLU-0",
					Slot: 0,
					SN:   "sn-0",
				},
				"MLU-1": {
					UUID: "MLU-1",
					Slot: 1,
					SN:   "sn-0",
				},
				"MLU-2": {
					UUID: "MLU-2",
					Slot: 2,
					SN:   "sn-1",
				},
				"MLU-3": {
					UUID: "MLU-3",
					Slot: 3,
					SN:   "sn-1",
				},
				"MLU-4": {
					UUID: "MLU-4",
					Slot: 4,
					SN:   "sn-2",
				},
				"MLU-5": {
					UUID: "MLU-5",
					Slot: 5,
					SN:   "sn-2",
				},
				"MLU-6": {
					UUID: "MLU-6",
					Slot: 6,
					SN:   "sn-3",
				},
				"MLU-7": {
					UUID: "MLU-7",
					Slot: 7,
					SN:   "sn-3",
				},
				"MLU-8": {
					UUID: "MLU-8",
					Slot: 8,
					SN:   "sn-4",
				},
				"MLU-9": {
					UUID: "MLU-9",
					Slot: 9,
					SN:   "sn-4",
				},
				"MLU-10": {
					UUID: "MLU-10",
					Slot: 10,
					SN:   "sn-5",
				},
				"MLU-11": {
					UUID: "MLU-11",
					Slot: 11,
					SN:   "sn-5",
				},
				"MLU-12": {
					UUID: "MLU-12",
					Slot: 12,
					SN:   "sn-6",
				},
				"MLU-13": {
					UUID: "MLU-13",
					Slot: 13,
					SN:   "sn-6",
				},
				"MLU-14": {
					UUID: "MLU-14",
					Slot: 14,
					SN:   "sn-7",
				},
				"MLU-15": {
					UUID: "MLU-15",
					Slot: 15,
					SN:   "sn-7",
				},
			}
		})

		DescribeTable("Allocation Devices",
			func(policy string, available []uint, size int, rings []cntopo.Ring, expected []uint, testTimeout ...bool) {
				var timeout bool
				if len(testTimeout) != 0 {
					timeout = testTimeout[0]
				}
				allocator = &defaultAllocator{
					policy: policy,
					cntopo: cntopoMock,
					devs:   devsInfo,
				}
				getRingTimeout = 50 * time.Millisecond
				cntopoMock.EXPECT().GetRings(available, size).Times(1).DoAndReturn(func(_, _ interface{}) ([]cntopo.Ring, error) {
					if timeout {
						time.Sleep(200 * time.Millisecond)
					}
					return rings, nil
				})
				got, err := allocator.Allocate(available, nil, size)
				if expected != nil {
					Expect(err).NotTo(HaveOccurred())
					sort.Slice(got, func(i, j int) bool {
						return got[i] < got[j]
					})
					Expect(got).To(Equal(expected))
				} else {
					Expect(err).To(HaveOccurred())
					Expect(got).To(BeNil())
				}
			},
			Entry("size 2 for best effort mode",
				bestEffort,
				[]uint{0, 1, 4},
				2,
				[]cntopo.Ring{
					{
						Ordinals:           []uint{0, 1},
						NonConflictRingNum: 2,
					},
					{
						Ordinals:           []uint{1, 4},
						NonConflictRingNum: 1,
					},
					{
						Ordinals:           []uint{0, 4},
						NonConflictRingNum: 1,
					},
				},
				[]uint{0, 1},
			),
			Entry("no rings with size 2 for best effort mode",
				bestEffort,
				[]uint{0, 14},
				2,
				[]cntopo.Ring{},
				[]uint{0, 14},
			),
			Entry("size 4 for best effort mode",
				bestEffort,
				[]uint{0, 1, 2, 3, 8, 9, 10},
				4,
				[]cntopo.Ring{
					{
						Ordinals:           []uint{0, 1, 2, 3},
						NonConflictRingNum: 2,
					},
				},
				[]uint{0, 1, 2, 3},
			),
			Entry("size 8 for best effort mode",
				bestEffort,
				[]uint{0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14},
				8,
				[]cntopo.Ring{
					{
						Ordinals:           []uint{0, 1, 2, 3, 4, 5, 6, 7},
						NonConflictRingNum: 4,
					},
				},
				[]uint{0, 1, 2, 3, 4, 5, 6, 7},
			),
			Entry("size 8 for best effort mode when get ring with timeout",
				bestEffort,
				[]uint{0, 1, 2, 3, 4, 5, 6, 7, 8},
				8,
				[]cntopo.Ring{{}},
				[]uint{0, 1, 2, 3, 4, 5, 6, 7},
				true,
			),
			Entry("size 4 for guaranteed mode",
				guaranteed,
				[]uint{0, 1, 2, 3, 8, 9, 10},
				4,
				[]cntopo.Ring{
					{
						Ordinals:           []uint{0, 1, 2, 3},
						NonConflictRingNum: 2,
					},
				},
				[]uint{0, 1, 2, 3},
			),
			Entry("no rings with size 4 for guaranteed mode",
				guaranteed,
				[]uint{0, 1, 3, 6},
				4,
				[]cntopo.Ring{},
				nil,
			),
			Entry("size 2 for guaranteed mode when get ring with timeout",
				guaranteed,
				[]uint{0, 1, 4},
				2,
				[]cntopo.Ring{{}},
				nil,
				true,
			),
			Entry("size 2 for restricted mode",
				restricted,
				[]uint{0, 1, 4},
				2,
				[]cntopo.Ring{
					{
						Ordinals:           []uint{0, 1},
						NonConflictRingNum: 2,
					},
					{
						Ordinals:           []uint{1, 4},
						NonConflictRingNum: 1,
					},
					{
						Ordinals:           []uint{0, 4},
						NonConflictRingNum: 1,
					},
				},
				[]uint{0, 1},
			),
			Entry("size 2 for restricted mode when get ring with timeout",
				restricted,
				[]uint{0, 1, 4},
				2,
				[]cntopo.Ring{{}},
				nil,
				true,
			),
		)
	})
})
