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
	"log"
	"os"
	"strings"

	flags "github.com/jessevdk/go-flags"
)

const (
	sriov    string = "sriov"
	envShare string = "env-share"
)

type Options struct {
	Mode               string `long:"mode" description:"device plugin mode" default:"default" choice:"default" choice:"sriov" choice:"env-share"`
	VirtualizationNum  uint   `long:"virtualization-num" description:"the virtualization number for each MLU, used only in sriov mode or env-share mode" default:"1" env:"VIRTUALIZATION_NUM"`
	DisableHealthCheck bool   `long:"disable-health-check" description:"disable MLU health check"`
	EnableConsole      bool   `long:"enable-console" description:"enable UART console device(/dev/ttyMS) in container"`
}

func ParseFlags() Options {
	for index, arg := range os.Args {
		if strings.HasPrefix(arg, "-mode") {
			os.Args[index] = strings.Replace(arg, "-mode", "--mode", 1)
			break
		}
	}
	if os.Getenv("DP_DISABLE_HEALTHCHECKS") == "all" {
		os.Args = append(os.Args, "--disable-health-check")
	}
	options := Options{}
	parser := flags.NewParser(&options, flags.Default)
	if _, err := parser.Parse(); err != nil {
		code := 1
		if fe, ok := err.(*flags.Error); ok {
			if fe.Type == flags.ErrHelp {
				code = 0
			}
		}
		os.Exit(code)
	}
	log.Printf("Parsed options: %v\n", options)
	return options
}
