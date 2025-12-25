// Copyright 2024 Cambricon, Inc.
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

package topology

import (
	"context"
	"fmt"
	"math/rand"
	"net"
	"sort"
	"strconv"
	"strings"
	"sync"
	"time"

	"github.com/Cambricon/cambricon-k8s-device-plugin/device-plugin/pkg/allocator"
	"github.com/Cambricon/cambricon-k8s-device-plugin/device-plugin/pkg/cndev"
	"github.com/Cambricon/cambricon-k8s-device-plugin/device-plugin/pkg/cntopo"
	"github.com/Cambricon/cambricon-k8s-device-plugin/device-plugin/pkg/mlu"
	log "github.com/sirupsen/logrus"
	"google.golang.org/grpc"
	"google.golang.org/grpc/credentials/insecure"
	corev1 "k8s.io/api/core/v1"
	metav1 "k8s.io/apimachinery/pkg/apis/meta/v1"
	"k8s.io/apimachinery/pkg/fields"
	"k8s.io/client-go/kubernetes"
	"k8s.io/client-go/tools/cache"
	podresourcesapi "k8s.io/kubelet/pkg/apis/podresources/v1alpha1"
)

var (
	maxSize   = 1024 * 1024 * 16 //16 Mb
	socket    = "/var/lib/kubelet/pod-resources/kubelet.sock"
	timeout   = 10 * time.Second
	topoRules = map[string]map[int]int{
		"092U": {2: 2, 4: 4}, //key is the number of cards required, value is the best 'NonConflictRingNum' for this card number combination derived from cntopo
		"073U": {2: 2, 4: 2, 8: 4},
		"085U": {2: 2},
		"095U": {2: 2, 4: 6},
	}
	uuidPrefix = "MLU-"
)

type Topology struct {
	annotation  map[string]string
	available   []uint
	deviceMaps  map[string]uint
	executeLock sync.Mutex
	option      mlu.Options
	topoRule    map[int]int

	k8sClient  kubernetes.Interface
	topoClient cntopo.Cntopo
}

func NewTopology(o mlu.Options, num uint) *Topology {
	model := allocator.Reverse(cndev.GetDeviceModel(uint(0)))
	topoRule := make(map[int]int)
	for k := range topoRules {
		if strings.Contains(model, k) {
			topoRule = topoRules[k]
		}
	}
	devM := map[string]uint{}
	for i := uint(0); i < num; i++ {
		uuid, err := cndev.GetDeviceUUID(i)
		if err != nil {
			log.Panicf("failed to get device uuid %v", err)
		}
		devM[uuid] = i
	}
	return &Topology{
		deviceMaps: devM,
		option:     o,
		topoRule:   topoRule,

		k8sClient:  mlu.InitClientSet(),
		topoClient: cntopo.New(),
	}
}

func (t *Topology) findCardInUse() (bool, error) {
	oldAvail := t.available
	avail := []uint{}
	deviceIDs, err := getDeviceIDsInUse(socket, timeout, maxSize)
	if err != nil {
		log.Errorf("failed to get deviceIDs %v", err)
		return false, err
	}
	info := map[string]struct{}{}
	for _, id := range deviceIDs {
		d := strings.TrimLeft(id, uuidPrefix)
		info[d] = struct{}{}
	}
	for d, i := range t.deviceMaps {
		if _, ok := info[d]; ok {
			continue
		}
		avail = append(avail, i)
	}
	if equalWithoutOrder(oldAvail, avail) {
		return true, err
	}
	t.available = avail
	return false, nil
}

func (t *Topology) getTopo() error {
	annotation := map[string]string{}
	for size, ring := range t.topoRule {
		if len(t.available) < size {
			continue
		}
		rings, err := t.topoClient.GetRings(t.available, size)
		if err != nil {
			log.Errorf("failed to get rings %v", err)
			return err
		}
		if len(rings) == 0 {
			continue
		}
		sort.Slice(rings, func(i int, j int) bool {
			return rings[i].NonConflictRingNum > rings[j].NonConflictRingNum
		})
		if rings[0].NonConflictRingNum < ring {
			annotation[strconv.Itoa(size)+"-cards"] = "guaranteed"
			continue
		}
		annotation[strconv.Itoa(size)+"-cards"] = "restricted"
	}
	t.annotation = annotation
	return nil
}

func (t *Topology) UpdateNodeAnnotation() error {
	node, err := t.k8sClient.CoreV1().Nodes().Get(context.TODO(), t.option.NodeName, metav1.GetOptions{})
	if err != nil {
		log.Errorf("Failed get node, err: %v", err)
		return err
	}

	if len(node.Annotations) == 0 && len(t.annotation) == 0 {
		return nil
	}

	var needUpdate bool
	for size := range t.topoRule {
		key := strconv.Itoa(size) + "-cards"
		if _, ok := node.Annotations[key]; ok {
			if _, ok := t.annotation[key]; ok {
				continue
			}
			delete(node.Annotations, key)
			needUpdate = true
			continue
		}
		if v, ok := t.annotation[key]; ok {
			node.Annotations[key] = v
			needUpdate = true
		}
	}

	if !needUpdate {
		return nil
	}

	log.Printf("Try to update node annotation, available is: %v, topology is: %v", t.available, t.annotation)
	time.Sleep(time.Duration(rand.Intn(5)) * 5 * time.Millisecond)
	_, err = t.k8sClient.CoreV1().Nodes().Update(context.TODO(), node, metav1.UpdateOptions{})
	return err
}

func (t *Topology) SetTopology() {
	t.execute()

	podWatcher := cache.NewFilteredListWatchFromClient(
		t.k8sClient.CoreV1().RESTClient(),
		"pods",
		metav1.NamespaceAll,
		func(options *metav1.ListOptions) {
			options.FieldSelector = fields.OneTermEqualSelector("spec.nodeName", t.option.NodeName).String()
		},
	)

	resyncPeriod := 30 * time.Second
	stopCh := make(chan struct{})
	defer close(stopCh)
	_, controller := cache.NewInformer(
		podWatcher,
		&corev1.Pod{},
		resyncPeriod,
		cache.ResourceEventHandlerFuncs{
			AddFunc: func(obj interface{}) {
				pod := obj.(*corev1.Pod)
				if matchResource(pod) {
					log.Debugf("Find pod use mlu %s is being added", pod.Name)
					t.execute()
				}
			},
			UpdateFunc: func(_, newObj interface{}) {
				newPod := newObj.(*corev1.Pod)
				if matchResource(newPod) {
					log.Debugf("Find pod use mlu %s is being updated", newPod.Name)
					t.execute()
				}
			},
			DeleteFunc: func(obj interface{}) {
				pod := obj.(*corev1.Pod)
				if matchResource(pod) {
					log.Debugf("Find pod use mlu %s is being deleted", pod.Name)
					t.execute()
				}
			},
		},
	)
	go controller.Run(stopCh)
	<-stopCh
}

func (t *Topology) execute() {
	t.executeLock.Lock()
	defer t.executeLock.Unlock()

	t.annotation = map[string]string{}
	sameWithOld, err := t.findCardInUse()
	if err != nil {
		log.Errorf("Failed to find card in use %v", err)
		return
	}
	if sameWithOld {
		log.Debug("No change since last sync")
		return
	}
	if err := t.getTopo(); err != nil {
		log.Errorf("Failed to get topo %v", err)
		return
	}
	if err := t.UpdateNodeAnnotation(); err != nil {
		log.Errorf("Failed to update node annotation %v", err)
	}
}

func connectToServer(socket string, timeout time.Duration, maxSize int) (*grpc.ClientConn, error) {
	ctx, cancel := context.WithTimeout(context.Background(), timeout)
	defer cancel()

	creds := insecure.NewCredentials()
	dialer := func(_ context.Context, address string) (net.Conn, error) {
		return net.DialTimeout("unix", address, timeout)
	}
	conn, err := grpc.DialContext(ctx, socket,
		grpc.WithTransportCredentials(creds),
		grpc.WithBlock(),
		grpc.WithDefaultCallOptions(grpc.MaxCallRecvMsgSize(maxSize)),
		grpc.WithContextDialer(dialer),
	)
	if err != nil {
		return nil, fmt.Errorf("failure connecting to %s: %v", socket, err)
	}
	return conn, nil
}

func listPods(socket string, timeout time.Duration, maxSize int) (*podresourcesapi.ListPodResourcesResponse, error) {
	conn, err := connectToServer(socket, timeout, maxSize)
	if err != nil {
		return nil, err
	}
	defer conn.Close()

	client := podresourcesapi.NewPodResourcesListerClient(conn)

	ctx, cancel := context.WithTimeout(context.Background(), timeout)
	defer cancel()

	resp, err := client.List(ctx, &podresourcesapi.ListPodResourcesRequest{})
	if err != nil {
		return nil, fmt.Errorf("failure getting pod resources %v", err)
	}
	return resp, nil
}

func getDeviceIDsInUse(socket string, timeout time.Duration, maxSize int) ([]string, error) {
	var deviceIDs []string
	pods, err := listPods(socket, timeout, maxSize)
	if err != nil {
		return nil, fmt.Errorf("list pods: %v", err)
	}
	for _, pod := range pods.GetPodResources() {
		for _, container := range pod.GetContainers() {
			for _, device := range container.GetDevices() {
				if !strings.Contains(device.GetResourceName(), "cambricon.com/mlu") {
					continue
				}
				deviceIDs = append(deviceIDs, device.GetDeviceIds()...)
			}
		}
	}
	log.Debugf("deviceIDs: %#v", deviceIDs)
	return deviceIDs, nil
}

func matchResource(pod *corev1.Pod) bool {
	if pod.Status.Phase == corev1.PodPending && pod.DeletionTimestamp == nil {
		return false
	}
	for _, container := range pod.Spec.Containers {
		for k := range container.Resources.Limits {
			if strings.Contains(k.String(), "cambricon.com/mlu") {
				log.Debugf("pod matched: %s", pod.Name)
				return true
			}
		}
	}
	return false
}

func equalWithoutOrder(a, b []uint) bool {
	if len(a) != len(b) {
		return false
	}
	tmpMap := make(map[uint]struct{})
	for _, v := range a {
		tmpMap[v] = struct{}{}
	}
	for _, v := range b {
		if _, ok := tmpMap[v]; !ok {
			return false
		}
	}
	return true
}
