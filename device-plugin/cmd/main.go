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
	"fmt"
	"net/http"
	_ "net/http/pprof"
	"os"
	"os/signal"
	"syscall"
	"time"

	"github.com/Cambricon/cambricon-k8s-device-plugin/device-plugin/pkg/cndev"
	"github.com/Cambricon/cambricon-k8s-device-plugin/device-plugin/pkg/cntopo"
	"github.com/Cambricon/cambricon-k8s-device-plugin/device-plugin/pkg/dsmlu"
	"github.com/Cambricon/cambricon-k8s-device-plugin/device-plugin/pkg/mlu"
	topo "github.com/Cambricon/cambricon-k8s-device-plugin/device-plugin/pkg/topology"
	"github.com/fsnotify/fsnotify"
	log "github.com/sirupsen/logrus"
	pluginapi "k8s.io/kubelet/pkg/apis/deviceplugin/v1beta1"
)

// Here version is a variable that stores the version of the application.
// The value of this variable will be set at build time with the -ldflags option.
// Using `-ldflags="-X 'main.Version=v1.0.0'"`.
var version string

func main() {
	options := mlu.LoadConfig()
	if options.Version {
		fmt.Println("Version:", version)
		return
	}

	go func() {
		log.Println(http.ListenAndServe("0.0.0.0:6060", nil))
	}()

	switch options.LogLevel {
	case "trace":
		log.SetLevel(log.TraceLevel)
	case "debug":
		log.SetLevel(log.DebugLevel)
	case "info":
		log.SetLevel(log.InfoLevel)
	case "warn":
		log.SetLevel(log.WarnLevel)
	case "error":
		log.SetLevel(log.ErrorLevel)
	case "fatal":
		log.SetLevel(log.FatalLevel)
	case "panic":
		log.SetLevel(log.PanicLevel)
	}

	log.Println("Loading CNDEV")
	if err := cndev.Init(false, options.AbnormalXIDErrorList); err != nil {
		log.Errorf("Failed to initialize CNDEV, err: %v", err)
		select {}
	}
	defer func() { log.Println("Shutdown of CNDEV returned:", cndev.Release()) }()

	log.Println("Fetching devices.")
	n, err := cndev.GetDeviceCount()
	if err != nil {
		log.Printf("Failed to get device count. err: %v", err)
	}
	if n == 0 {
		log.Println("No devices found. Waiting indefinitely.")
		select {}
	}

	if options.Mode == mlu.TopologyAware {
		log.Println("Loading CNTOPO")
		if err := cntopo.Init(); err != nil {
			log.Errorf("Failed to initialize CNTOPO, err: %v", err)
			select {}
		}
		defer func() { log.Println("Shutdown of CNTOPO returned:", cntopo.Release()) }()
		topology := topo.NewTopology(options, n)
		go topology.SetTopology()
	}

	if options.Mode == mlu.DynamicSmlu {
		dsmlu := dsmlu.NewDsmlu(options)
		go dsmlu.SyncDsmlu()
	}

	log.Println("Starting FS watcher.")
	watcher, err := startFSWatcher(pluginapi.DevicePluginPath)
	if err != nil {
		log.Panicf("Failed to created FS watcher. err: %v", err)
	}
	defer watcher.Close()

	log.Println("Starting OS watcher.")
	sigs := startOSWatcher(syscall.SIGHUP, syscall.SIGINT, syscall.SIGTERM, syscall.SIGQUIT)

	go func() {
		http.HandleFunc("/healthz", func(w http.ResponseWriter, r *http.Request) {
			if err := cndev.Init(true, nil); err != nil {
				w.WriteHeader(http.StatusInternalServerError)
			} else {
				w.WriteHeader(http.StatusOK)
				w.Write([]byte("ok"))
			}
		})

		server := &http.Server{
			Addr: "0.0.0.0:30107",
		}

		log.Printf("start serving at %s", server.Addr)
		log.Panic(server.ListenAndServe())
	}()

	var restarting bool
	var restartTimeout <-chan time.Time
	var plugins []*mlu.CambriconDevicePlugin
restart:
	// If we are restarting, stop plugins from previous run.
	if restarting {
		err := stopPlugins(plugins)
		if err != nil {
			log.Printf("Stop plugins failed with err:%v", err)
			return
		}
	}

	log.Println("Starting Plugins.")
	ts := time.Now()
	log.Debugf("Starting Plugins in time %s", ts)
	plugins, restartPlugins := startPlugins(options)
	tf := time.Now()
	log.Debugf("Finished Plugins in time %s, diff is %s", tf, tf.Sub(ts))
	if restartPlugins {
		log.Printf("Failed to start one or more plugins. Retrying in 30s...")
		restartTimeout = time.After(30 * time.Second)
	}
	restarting = true

	for {
		select {
		case <-restartTimeout:
			goto restart
		case event := <-watcher.Events:
			if event.Name == pluginapi.KubeletSocket && event.Op&fsnotify.Create == fsnotify.Create {
				log.Printf("inotify: %s created, restarting.", pluginapi.KubeletSocket)
				goto restart
			}
		case err := <-watcher.Errors:
			log.Printf("inotify err: %v", err)
		case s := <-sigs:
			switch s {
			case syscall.SIGHUP:
				log.Println("Received SIGHUP, restarting.")
				goto restart
			default:
				log.Printf("Received signal %v, shutting down.", s)
				err = stopPlugins(plugins)
				if err != nil {
					log.Printf("Stop plugins with err:%v", err)
					return
				}
			}
		}
	}
}

func startFSWatcher(files ...string) (*fsnotify.Watcher, error) {
	watcher, err := fsnotify.NewWatcher()
	if err != nil {
		log.Printf("New fsnotify watch with err:%v", err)
		return nil, err
	}

	for _, f := range files {
		err = watcher.Add(f)
		if err != nil {
			log.Printf("Add fsnotify watch with err:%v", err)
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

func stopPlugins(plugins []*mlu.CambriconDevicePlugin) error {
	log.Println("Stopping plugins.")
	for _, p := range plugins {
		if err := p.Stop(); err != nil {
			log.Printf("Stop plugins err:%v", err)
			return err
		}
	}
	return nil
}

func startPlugins(options mlu.Options) ([]*mlu.CambriconDevicePlugin, bool) {
	devsM, devsInfoM := mlu.GetDevices(options)
	var plugins []*mlu.CambriconDevicePlugin
	for profile, devsInfo := range devsInfoM {
		devicePlugin := mlu.NewCambriconDevicePlugin(options, profile, devsM[profile], devsInfo)
		if err := devicePlugin.Serve(); err != nil {
			log.Printf("Serve device plugin %s, err: %v, restarting.", profile, err)
			return plugins, true
		}
		plugins = append(plugins, devicePlugin)
	}
	if len(plugins) == 0 {
		log.Println("No devices found. Waiting indefinitely.")
	}
	return plugins, false
}
