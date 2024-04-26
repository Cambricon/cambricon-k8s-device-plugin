package dsmlu

import (
	"context"
	"strconv"
	"strings"
	"sync"
	"time"

	"github.com/Cambricon/cambricon-k8s-device-plugin/device-plugin/pkg/cndev"
	"github.com/Cambricon/cambricon-k8s-device-plugin/device-plugin/pkg/mlu"
	log "github.com/sirupsen/logrus"
	v1 "k8s.io/api/core/v1"
	metav1 "k8s.io/apimachinery/pkg/apis/meta/v1"
	"k8s.io/apimachinery/pkg/fields"
	"k8s.io/apimachinery/pkg/util/wait"
	"k8s.io/client-go/kubernetes"
	"k8s.io/client-go/rest"
	"k8s.io/client-go/tools/cache"
)

type Dsmlu struct {
	executeLock sync.Mutex
	option      mlu.Options

	k8sClient *kubernetes.Clientset
}

func NewDsmlu(o mlu.Options) *Dsmlu {
	return &Dsmlu{
		option: o,

		k8sClient: initClientSet(),
	}
}

func (d *Dsmlu) SyncDsmlu() {
	resyncPeriod := 60 * time.Second
	stopCh := make(chan struct{})
	defer close(stopCh)

	go wait.Until(d.exec, resyncPeriod, stopCh)

	podWatcher := cache.NewFilteredListWatchFromClient(
		d.k8sClient.CoreV1().RESTClient(),
		"pods",
		metav1.NamespaceAll,
		func(options *metav1.ListOptions) {
			options.FieldSelector = fields.OneTermEqualSelector("spec.nodeName", d.option.NodeName).String()
		},
	)
	_, controller := cache.NewInformer(
		podWatcher,
		&v1.Pod{},
		resyncPeriod,
		cache.ResourceEventHandlerFuncs{
			DeleteFunc: func(obj interface{}) {
				pod := obj.(*v1.Pod)
				if matchResource(pod) {
					log.Debugf("Find pod use dsmlu %s is being deleted", pod.Name)
					d.recycleProfileAndInstance(pod)
					d.fixNodeLock(pod)
				}
			},
		},
	)
	go controller.Run(stopCh)
	<-stopCh
}

func (d *Dsmlu) fixNodeLock(pod *v1.Pod) {
	if assigned, ok := pod.ObjectMeta.Annotations[mlu.DsmluResourceAssigned]; !ok ||
		assigned != "false" || (pod.Status.Phase != v1.PodPending && pod.Status.Phase != v1.PodFailed) {
		return
	}

	log.Debugf("Find pod %v use dsmlu is being deleted, try to delete its node lock", pod)

	node, err := d.k8sClient.CoreV1().Nodes().Get(context.TODO(), d.option.NodeName, metav1.GetOptions{})
	if err != nil {
		log.Errorf("Get node %s failed %v", d.option.NodeName, err)
		return
	}
	newNode := node.DeepCopy()
	if newNode.Annotations != nil {
		if time, ok := newNode.Annotations[mlu.DsmluLockTime]; ok {
			log.Printf("Node lock timestamp %s", time)
			delete(newNode.Annotations, mlu.DsmluLockTime)
		} else {
			log.Println("Lock is released, No Need to update node")
			return
		}
	}

	_, err = d.k8sClient.CoreV1().Nodes().Update(context.TODO(), newNode, metav1.UpdateOptions{})
	if err != nil {
		log.Printf("Failed to release node lock %s, err %v", mlu.DsmluLockTime, err)
	} else {
		log.Printf("Release node lock %s successfully.", mlu.DsmluLockTime)
	}
}

func (d *Dsmlu) recycleProfileAndInstance(pod *v1.Pod) {

	d.executeLock.Lock()
	defer d.executeLock.Unlock()

	anno, ok := pod.Annotations[mlu.DsmluProfileAndInstance]
	if !ok || pod.DeletionTimestamp == nil {
		return
	}

	log.Debugf("Try to recycle instance from pod %s use dsmlu with annotation %s", pod.Name, anno)

	v := strings.Split(anno, "_")
	if len(v) != 4 {
		log.Errorf("Invalid pod annotation %s", anno)
		return
	}
	profile, err := strconv.Atoi(v[0])
	if err != nil {
		log.Errorf("Strconv value %s, %v", v[0], err)
		return
	}
	instance, err := strconv.Atoi(v[1])
	if err != nil {
		log.Errorf("Strconv value %s, %v", v[1], err)
		return
	}

	log.Debugf("Try to recycle instance %d from pod %s use dsmlu", instance, pod.Name)
	if err := cndev.DestroySmlu(instance); err != nil {
		log.Errorf("Failed to destroy smlu %d, %v", instance, err)
		return
	}

	pl, err := mlu.GetProfileFromAnnotation(pod)
	if err != nil {
		log.Errorf("Failed to get vcore and vmemory from annotation, err %v", err)
		return
	}

	memUnit := d.option.MinDsmluUnit
	if d.option.MinDsmluUnit <= 0 {
		mem, err := cndev.GetDeviceMemory(uint(pl.Slot))
		if err != nil {
			log.Panicln("Fatal:", err)
		}
		memUnit = int(mem) / 100
	}
	if info, ok := cndev.GetExistProfile(pl, memUnit); ok {
		if info.Remain == info.Total && info.ProfileID == profile {
			log.Debugf("Try to recycle profile %d for device %d", profile, pl.Slot)
			if err := cndev.DestroySmluProfile(uint(profile), uint(pl.Slot)); err != nil {
				log.Errorf("Failed to destroy smlu profile %d for device %d, err %v", profile, pl.Slot, err)
				return
			}
		}
	}
}

func initClientSet() *kubernetes.Clientset {
	config, err := rest.InClusterConfig()
	if err != nil {
		log.Errorf("Failed to get in cluser config, err: %v", err)
	}
	clientset, err := kubernetes.NewForConfig(config)
	if err != nil {
		log.Errorf("Failed to init clientset, err: %v", err)
	}
	return clientset
}

func matchResource(pod *v1.Pod) bool {
	if pod.ObjectMeta.Annotations == nil {
		return false
	}
	_, found := pod.ObjectMeta.Annotations[mlu.DsmluProfile]
	return found
}

func (d *Dsmlu) exec() {

	d.executeLock.Lock()
	defer d.executeLock.Unlock()

	selector := fields.SelectorFromSet(fields.Set{"spec.nodeName": d.option.NodeName})
	podList, err := d.k8sClient.CoreV1().Pods(v1.NamespaceAll).List(context.TODO(), metav1.ListOptions{
		FieldSelector: selector.String(),
	})
	if err != nil {
		log.Errorf("Failed to list pod on node %s, err: %v", d.option.NodeName, err)
		return
	}

	toKeepInstances := make(map[int]map[int]struct{})
	toKeepProfiles := make(map[int]map[int]struct{})
	for _, pod := range podList.Items {
		if !matchResource(&pod) {
			continue
		}
		if pod.Status.Phase != v1.PodPending && pod.Status.Phase != v1.PodRunning {
			continue
		}
		anno, ok := pod.ObjectMeta.Annotations["CAMBRICON_DSMLU_PROFILE_INSTANCE"]
		if !ok {
			if pod.Status.Phase == v1.PodPending {
				log.Debugf("Found pending pod %s in handling, retry exec later", pod.Name)
				return
			}
			continue
		}

		v := strings.Split(anno, "_")
		// 1_256_1_0 represents profileID_instanceHandle_slot_instanceID
		if len(v) != 4 {
			log.Printf("Found pod %s with invalid annotation %s, ignore it", pod.Name, anno)
			continue
		}
		profileID, err := strconv.Atoi(v[0])
		if err != nil {
			log.Printf("strconv value %s, %v", v[0], err)
			continue
		}
		slot, err := strconv.Atoi(v[2])
		if err != nil {
			log.Printf("strconv value %s, %v", v[2], err)
			continue
		}
		instanceID, err := strconv.Atoi(v[3])
		if err != nil {
			log.Printf("strconv value %s, %v", v[3], err)
			continue
		}
		if toKeepInstances[slot] == nil {
			toKeepInstances[slot] = make(map[int]struct{})
		}
		toKeepInstances[slot][instanceID] = struct{}{}
		if toKeepProfiles[slot] == nil {
			toKeepProfiles[slot] = make(map[int]struct{})
		}
		toKeepProfiles[slot][profileID] = struct{}{}
	}

	num, err := cndev.GetDeviceCount()
	if err != nil {
		log.Errorf("Failed to get device count, err: %v", err)
		return
	}
	for i := uint(0); i < num; i++ {
		enabled, err := cndev.DeviceSmluModeEnabled(i)
		if err != nil {
			log.Errorf("Failed to get smlu mode for slot %d, err: %v", i, err)
			continue
		}
		if !enabled {
			log.Debugf("Smlu mode is disabled for slot %d, skip", i)
			continue
		}
		smluInfos, err := cndev.GetAllSmluInfo(i)
		if err != nil {
			log.Errorf("Failed to get smlu info, err: %v", err)
			return
		}
		for _, smluInfo := range smluInfos {
			if _, ok := toKeepInstances[int(i)][smluInfo.InstanceID]; !ok {
				instanceHandle := smluInfo.InstanceID<<8 | int(i)
				log.Debugf("Found legacy smlu instance %d for slot %d, try to destroy it", smluInfo.InstanceID, i)
				if err := cndev.DestroySmlu(instanceHandle); err != nil {
					log.Errorf("Failed to destroy smlu with instance handle %d err %v", instanceHandle, err)
				}
			}
		}
		profileInfos, err := cndev.GetDeviceProfileInfo(i)
		if err != nil {
			log.Errorf("Failed to get profile info, err: %v", err)
			return
		}
		for _, profileInfo := range profileInfos {
			if _, ok := toKeepProfiles[int(i)][profileInfo.ProfileID]; !ok {
				log.Debugf("Found legacy smlu profile %d for device %d, try to destroy it", profileInfo.ProfileID, i)
				if err := cndev.DestroySmluProfile(uint(profileInfo.ProfileID), i); err != nil {
					log.Errorf("Failed to recycle smlu profile %d for device %d, err %v", profileInfo.ProfileID, i, err)
				}
			}
		}
	}
}
