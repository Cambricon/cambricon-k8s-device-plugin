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
	"strings"

	flags "github.com/jessevdk/go-flags"
	log "github.com/sirupsen/logrus"
	"sigs.k8s.io/yaml"
)

type Options struct {
	CnmonPath           string     `long:"cnmon-path" description:"host cnmon path" json:"cnmonPath,omitempty"`
	ConfigFile          string     `long:"config-file" description:"config file" env:"CONFIG_FILE"`
	DisableHealthCheck  bool       `long:"disable-health-check" description:"disable MLU health check" json:"disableHealthCheck,omitempty"`
	EnableConsole       bool       `long:"enable-console" description:"enable UART console device(/dev/ttyMS) in container" json:"enableConsole,omitempty"`
	EnableDeviceType    bool       `long:"enable-device-type" description:"enable device registration with type info" json:"enableDeviceType,omitempty"`
	LogLevel            string     `long:"log-level" description:"set log level: trace/debug/info/warn/error/fatal/panic" default:"info" json:"logLevel,omitempty"`
	MinDsmluUnit        int        `long:"min-dsmlu-unit" description:"minimum unit for dsmu, used only in dynamic-smlu mode" default:"0" env:"MIN-DSMLU-UNIT" json:"minDsmluUnit,omitempty"`
	MLULinkPolicy       string     `long:"mlulink-policy" description:"MLULink topology policy" default:"best-effort" choice:"best-effort" choice:"restricted" choice:"guaranteed" json:"mluLinkPolicy,omitempty"`
	Mode                pluginMode `long:"mode" description:"device plugin mode" default:"default" choice:"default" choice:"dynamic-smlu" choice:"env-share" choice:"mim" choice:"topology-aware" json:"mode,omitempty"`
	MountRPMsg          bool       `long:"mount-rpmsg" description:"mount RPMsg directory, will be deprecated in the near future" json:"mountRPMsg,omitempty"`
	NodeName            string     `long:"node-name" description:"host node name" env:"NODE_NAME" json:"nodeName,omitempty"`
	NodeLabel           bool       `long:"node-label" description:"enable node label for MLU devices" json:"nodeLabel,omitempty"`
	OneShotForNodeLabel bool       `long:"one-shot-for-node-label" description:"enable one-shot mode for node label, only works when nodeLabel is enabled" json:"oneShotForNodeLabel,omitempty"`
	UseRuntime          bool       `long:"use-runtime" description:"only set enabled when cambricon container runtime is configed as the default runtime" json:"useRuntime,omitempty"`
	Version             bool       `long:"version" description:"print out version"`
	VirtualizationNum   int        `long:"virtualization-num" description:"the virtualization number for each MLU, used only in env-share mode" default:"1" env:"VIRTUALIZATION_NUM" json:"virtualizationNum,omitempty"`
}

func LoadConfig() Options {
	opt := parseFlags()

	final, err := updateWithConfigFile(opt)
	if err != nil {
		log.Printf("update option with config file failed: %v", err)
		os.Exit(1)
	}
	log.Printf("Final options: %v", final)
	return final
}

func parseFlags() Options {
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
	log.Printf("Parsed options: %v", options)
	return options
}

func updateWithConfigFile(opt Options) (Options, error) {
	if opt.ConfigFile == "" {
		return opt, nil
	}

	final := &Options{}
	f, err := os.ReadFile(opt.ConfigFile)
	if err != nil {
		return Options{}, fmt.Errorf("error read config file: %v", err)
	}
	if err := yaml.UnmarshalStrict(f, final); err != nil {
		return Options{}, err
	}

	flagCfg, err := yaml.Marshal(opt)
	if err != nil {
		return Options{}, err
	}
	if err := yaml.UnmarshalStrict(flagCfg, final); err != nil {
		return Options{}, err
	}
	return *final, nil
}
