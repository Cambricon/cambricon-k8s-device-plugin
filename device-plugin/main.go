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
	"os/signal"
	"syscall"

	"github.com/cambricon/cambricon-k8s-device-plugin/device-plugin/pkg/cndev"
	"github.com/fsnotify/fsnotify"
	pluginapi "k8s.io/kubelet/pkg/apis/deviceplugin/v1beta1"
)

func main() {

	options := ParseFlags()

	log.Println("Loading CNDEV")
	if err := cndev.Init(); err != nil {
		log.Printf("Failed to initialize CNDEV: %s.", err)

		select {}
	}
	defer func() { log.Println("Shutdown of CNDEV returned:", cndev.Release()) }()

	log.Println("Fetching devices.")
	n, err := cndev.GetDeviceCount()
	check(err)
	if n == 0 {
		log.Println("No devices found. Waiting indefinitely.")
		select {}
	}

	log.Println("Starting FS watcher.")
	watcher, err := startFSWatcher(pluginapi.DevicePluginPath)
	if err != nil {
		log.Println("Failed to created FS watcher.")
		os.Exit(1)
	}
	defer watcher.Close()

	log.Println("Starting OS watcher.")
	sigs := startOSWatcher(syscall.SIGHUP, syscall.SIGINT, syscall.SIGTERM, syscall.SIGQUIT)

	restart := true
	var devicePlugin *CambriconDevicePlugin

L:
	for {
		if restart {
			if devicePlugin != nil {
				devicePlugin.Stop()
			}

			devicePlugin = NewCambriconDevicePlugin(options)
			if err := devicePlugin.Serve(); err != nil {
				log.Println("Could not contact Kubelet, retrying. Did you enable the device plugin feature gate?")
			} else {
				restart = false
			}
		}

		select {
		case event := <-watcher.Events:
			if event.Name == pluginapi.KubeletSocket && event.Op&fsnotify.Create == fsnotify.Create {
				log.Printf("inotify: %s created, restarting.", pluginapi.KubeletSocket)
				restart = true
			}

		case err := <-watcher.Errors:
			log.Printf("inotify: %s", err)

		case s := <-sigs:
			switch s {
			case syscall.SIGHUP:
				log.Println("Received SIGHUP, restarting.")
				restart = true
			default:
				log.Printf("Received signal \"%v\", shutting down.", s)
				devicePlugin.Stop()
				break L
			}
		}
	}
}

func startFSWatcher(files ...string) (*fsnotify.Watcher, error) {
	watcher, err := fsnotify.NewWatcher()
	if err != nil {
		return nil, err
	}

	for _, f := range files {
		err = watcher.Add(f)
		if err != nil {
			watcher.Close()
			return nil, err
		}
	}

	return watcher, nil
}

func startOSWatcher(sigs ...os.Signal) chan os.Signal {
	sigChan := make(chan os.Signal, 1)
	signal.Notify(sigChan, sigs...)

	return sigChan
}
