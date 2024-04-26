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

package mlu

import (
	"context"
	"fmt"
	"strconv"
	"strings"
	"time"

	"github.com/Cambricon/cambricon-k8s-device-plugin/device-plugin/pkg/cndev"
	log "github.com/sirupsen/logrus"
	v1 "k8s.io/api/core/v1"
	metav1 "k8s.io/apimachinery/pkg/apis/meta/v1"
	"k8s.io/apimachinery/pkg/fields"
	"k8s.io/apimachinery/pkg/types"
)

func (m *CambriconDevicePlugin) getPendingPodsInNode(ctx context.Context) ([]v1.Pod, error) {
	pods := []v1.Pod{}
	podMap := make(map[types.UID]bool)

	selector := fields.SelectorFromSet(fields.Set{"spec.nodeName": m.nodeHostname, "status.phase": "Pending"})
	podList, err := m.clientset.CoreV1().Pods(v1.NamespaceAll).List(ctx, metav1.ListOptions{
		FieldSelector: selector.String(),
	})
	for i := 0; i < retries && err != nil; i++ {
		log.Printf("list pods error %v, retried %d times", err, i)
		time.Sleep(100 * time.Second)
		podList, err = m.clientset.CoreV1().Pods(v1.NamespaceAll).List(ctx, metav1.ListOptions{
			FieldSelector: selector.String(),
		})
	}
	if err != nil {
		return nil, fmt.Errorf("failed to get pending pods assigned to node %v", m.nodeHostname)
	}

	for _, pod := range podList.Items {
		if _, ok := podMap[pod.UID]; !ok {
			pods = append(pods, pod)
			podMap[pod.UID] = true
		}
	}
	return pods, nil
}

func isDynamicSmluAssumedPod(pod *v1.Pod) bool {
	if !requestsDynamicSmlu(pod) {
		return false
	}

	if pod.ObjectMeta.Annotations == nil {
		return false
	}

	if assigned, ok := pod.ObjectMeta.Annotations[DsmluResourceAssigned]; ok &&
		assigned == "false" {
		return true
	}

	return false
}

func requestsDynamicSmlu(pod *v1.Pod) bool {
	for _, c := range pod.Spec.Containers {
		for k, v := range c.Resources.Limits {
			if v.Value() > 0 && strings.HasPrefix(k.String(), "cambricon.com/") &&
				(strings.HasSuffix(k.String(), ".vcore") || strings.HasSuffix(k.String(), ".vmemory")) {
				return true
			}
		}
	}
	return false
}

func GetProfileFromAnnotation(pod *v1.Pod) (*cndev.DsmluProfile, error) {
	pl := &cndev.DsmluProfile{}
	value, found := pod.ObjectMeta.Annotations[DsmluProfile]
	if !found {
		return pl, fmt.Errorf("pod annotation %s not found", DsmluProfile)
	}

	// value is like slot_vcore_vmemory, for example 0_2_5
	log.Debugf("Get pod %s profile %s", pod.Name, value)
	v := strings.Split(value, "_")
	if len(v) != 3 {
		return pl, fmt.Errorf("invalid pod annotation %s", value)
	}

	p, err := strconv.Atoi(v[0])
	if err != nil {
		return pl, fmt.Errorf("strconv value %s, %v", v[0], err)
	}
	pl.Slot = p
	p, err = strconv.Atoi(v[1])
	if err != nil {
		return pl, fmt.Errorf("strconv value %s, %v", v[1], err)
	}
	pl.Vcore = p
	p, err = strconv.Atoi(v[2])
	if err != nil {
		return pl, fmt.Errorf("strconv value %s, %v", v[2], err)
	}
	pl.Vmemory = p

	return pl, nil
}

func (m *CambriconDevicePlugin) releaseNodeLock() error {
	node, err := m.clientset.CoreV1().Nodes().Get(context.TODO(), m.nodeHostname, metav1.GetOptions{})
	if err != nil {
		return err
	}
	if node.Annotations == nil {
		return nil
	}
	t, ok := node.Annotations[DsmluLockTime]
	if !ok {
		return nil
	}
	log.Printf("node lock timestamp %s", t)
	patchData := []byte(`[
				{
					"op": "remove",
					"path": "/metadata/annotations/cambricon.com~1dsmlu.lock"
				}
			]`)

	_, err = m.clientset.CoreV1().Nodes().Patch(context.TODO(), node.Name, types.JSONPatchType, patchData, metav1.PatchOptions{})

	if err != nil {
		log.Printf("Failed to release node lock %s, err %v", DsmluLockTime, err)
	} else {
		log.Printf("release node lock %s successfully.", DsmluLockTime)
	}
	return err
}

func (m *CambriconDevicePlugin) getDynamicSmluCandidatePod(ctx context.Context) (*v1.Pod, error) {
	var podCount, containerCount uint
	var candidatePod v1.Pod

	allPods, err := m.getPendingPodsInNode(ctx)
	if err != nil {
		return nil, err
	}
	log.Debugf("Found %d pending pods", len(allPods))

	for _, pod := range allPods {
		if isDynamicSmluAssumedPod(&pod) {
			candidatePod = pod
			log.Debugf("Found candidate pod %s", candidatePod.Name)
			podCount++
		}
		if podCount > 1 {
			return nil, fmt.Errorf("number of dynamic smlu candidate pods large than one")
		}
	}

	if podCount == 0 {
		return nil, fmt.Errorf("no dynamic smlu candidate pod found")
	}

	for _, c := range candidatePod.Spec.Containers {
		for k, v := range c.Resources.Limits {
			if v.Value() > 0 && strings.HasPrefix(k.String(), "cambricon.com/") &&
				(strings.HasSuffix(k.String(), ".vcore") || strings.HasSuffix(k.String(), ".vmemory")) {
				containerCount++
				break
			}
		}
		if containerCount > 1 {
			return nil, fmt.Errorf("number of containers of dynamic smlu candidate pod large than one")
		}
	}

	return &candidatePod, nil
}
