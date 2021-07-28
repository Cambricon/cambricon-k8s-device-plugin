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
	"os"
	"testing"

	"github.com/stretchr/testify/assert"
)

func TestLegacyConfigs(t *testing.T) {
	err := os.Setenv("DP_DISABLE_HEALTHCHECKS", "all")
	assert.NoError(t, err)
	oldArgs := os.Args
	defer func() { os.Args = oldArgs }()
	cmd := []string{os.Args[0]}
	tests := []struct {
		args []string
		out  Options
	}{
		{
			args: []string{"-mode", "sriov"},
			out: Options{
				Mode:               sriov,
				MLULinkPolicy:      bestEffort,
				VirtualizationNum:  1,
				DisableHealthCheck: true,
			},
		},
		{
			args: []string{"-mode=env-share"},
			out: Options{
				Mode:               envShare,
				MLULinkPolicy:      bestEffort,
				VirtualizationNum:  1,
				DisableHealthCheck: true,
			},
		},
		{
			args: []string{"--mode=topology-aware"},
			out: Options{
				Mode:               topologyAware,
				MLULinkPolicy:      bestEffort,
				VirtualizationNum:  1,
				DisableHealthCheck: true,
			},
		},
	}
	for _, tt := range tests {
		t.Run("", func(t *testing.T) {
			os.Args = append(cmd, tt.args...)
			o := ParseFlags()
			assert.Equal(t, tt.out, o)
		})
	}
	err = os.Unsetenv("DP_DISABLE_HEALTHCHECKS")
	assert.NoError(t, err)
}
