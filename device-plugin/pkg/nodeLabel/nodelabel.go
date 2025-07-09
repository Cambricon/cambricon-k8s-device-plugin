package nodeLabel

import (
	"context"
	"fmt"
	"os"
	"reflect"
	"regexp"
	"strings"
	"sync"
	"time"

	"github.com/Cambricon/cambricon-k8s-device-plugin/device-plugin/pkg/cndev"
	"github.com/Cambricon/cambricon-k8s-device-plugin/device-plugin/pkg/mlu"
	"github.com/pkg/errors"
	log "github.com/sirupsen/logrus"
	corev1 "k8s.io/api/core/v1"
	metav1 "k8s.io/apimachinery/pkg/apis/meta/v1"
	"k8s.io/apimachinery/pkg/util/wait"
	"k8s.io/client-go/informers"
	"k8s.io/client-go/kubernetes"
	"k8s.io/client-go/tools/cache"
	"k8s.io/client-go/util/retry"
)

const (
	cpuTypeLabel       = "CPUType"
	driverVersionLabel = "DriverVersion"
	mcuVersionLabel    = "MCUVersion"
	modelLabel         = "Model"

	checkInterval = 30 * time.Second
	resyncPeriod  = 30 * time.Minute
)

type NodeLabel struct {
	cpuType       string
	k8sClient     kubernetes.Interface
	nodeCache     *corev1.Node
	nodeCacheLock sync.RWMutex
	nodeName      string
	oneShot       bool
}

func NewNodeLabel(o mlu.Options) *NodeLabel {
	return &NodeLabel{
		cpuType:   getPhysicalCPUType(),
		k8sClient: mlu.InitClientSet(),
		nodeName:  o.NodeName,
		oneShot:   o.OneShotForNodeLabel,
	}
}

func (nl *NodeLabel) Run(ctx context.Context) {
	log.Info("Starting NodeLabel manager")

	if err := nl.checkAndUpdateNodeLabels(); err != nil {
		log.Errorf("Initial node label check failed: %v", err)
	}

	if nl.oneShot {
		log.Info("One-shot mode enabled, exiting after initial label check")
		return
	}

	nl.initNodeInformer(ctx)

	ticker := time.NewTicker(checkInterval)
	defer ticker.Stop()

	for {
		select {
		case <-ctx.Done():
			log.Info("Shutting down NodeLabel manager")
			return
		case <-ticker.C:
			if err := nl.checkAndUpdateNodeLabels(); err != nil {
				log.Errorf("Failed to check and update node labels: %v", err)
			} else {
				log.Debug("Node labels checked ok")
			}
		}
	}
}

func (nl *NodeLabel) initNodeInformer(ctx context.Context) {
	log.Debug("Initializing node informer")

	factory := informers.NewSharedInformerFactory(nl.k8sClient, resyncPeriod)
	nodeInformer := factory.Core().V1().Nodes().Informer()

	nodeInformer.AddEventHandler(cache.FilteringResourceEventHandler{
		FilterFunc: func(obj interface{}) bool {
			node := obj.(*corev1.Node)
			return node.Name == nl.nodeName
		},
		Handler: cache.ResourceEventHandlerFuncs{
			AddFunc: func(obj interface{}) {
				nl.updateNodeCache(obj.(*corev1.Node))
			},
			UpdateFunc: func(oldObj, newObj interface{}) {
				oldNode := oldObj.(*corev1.Node)
				newNode := newObj.(*corev1.Node)
				if reflect.DeepEqual(oldNode.Labels, newNode.Labels) {
					return
				}
				log.Info("Detects node labels changed, should refresh cache")
				log.Debugf("Old labels: %v, New labels: %v", oldNode.Labels, newNode.Labels)
				nl.updateNodeCache(newNode)
			},
		},
	})

	stopCh := make(chan struct{})
	go func() {
		<-ctx.Done()
		close(stopCh)
	}()

	go nodeInformer.Run(stopCh)

	if err := wait.PollUntilContextCancel(ctx, 2*time.Second, true,
		func(context.Context) (bool, error) {
			return nodeInformer.HasSynced(), nil
		}); err != nil {
		log.Errorf("Node cache sync failed: %v", err)
	} else {
		log.Info("Node cache synced successfully")
	}
}

func (nl *NodeLabel) updateNodeCache(node *corev1.Node) {
	if node.Name != nl.nodeName {
		return
	}

	nl.nodeCacheLock.Lock()
	nl.nodeCache = node.DeepCopy()
	nl.nodeCacheLock.Unlock()
	log.Debug("Node cache updated from informer")

	nl.checkAndUpdateNodeLabels()
}

func (nl *NodeLabel) checkAndUpdateNodeLabels() error {
	log.Debugln(strings.Repeat("=", 40))
	log.Debug("Checking and updating node labels")

	actualLabels := nl.getActualLabels()
	if len(actualLabels) == 0 {
		return fmt.Errorf("no valid labels obtained")
	}

	node, err := nl.getCurrentNode()
	if err != nil {
		return fmt.Errorf("failed to get current node: %v", err)
	}

	if !nl.labelsNeedUpdate(node.Labels, actualLabels) {
		log.Debug("No label changes detected")
		return nil
	}

	return nl.updateNodeLabels(actualLabels)
}

func (nl *NodeLabel) labelsNeedUpdate(existing, desired map[string]string) bool {
	log.Debug("Check if labels need update")

	var needUpdate bool
	for key, desiredVal := range desired {
		if existingVal, ok := existing[key]; !ok || existingVal != desiredVal {
			log.Infof("Label %q changed: %s -> %s", key, existingVal, desiredVal)
			needUpdate = true
		}
	}
	return needUpdate
}

func (nl *NodeLabel) getCurrentNode() (*corev1.Node, error) {
	log.Debug("Fetching current node information")

	if nl.nodeCache != nil {
		log.Debug("Using cached node information")
		return nl.nodeCache, nil
	}
	node, err := nl.k8sClient.CoreV1().Nodes().Get(context.TODO(), nl.nodeName, metav1.GetOptions{})
	if err != nil {
		return nil, fmt.Errorf("API get node failed: %v", err)
	}

	nl.nodeCacheLock.Lock()
	nl.nodeCache = node.DeepCopy()
	nl.nodeCacheLock.Unlock()

	log.Debugf("Fetching current node labels gives: %v", node.Labels)
	return node, nil
}

func (nl *NodeLabel) updateNodeLabels(labels map[string]string) error {
	log.Debug("Updating node labels")

	err := retry.RetryOnConflict(retry.DefaultBackoff, func() error {
		node, err := nl.k8sClient.CoreV1().Nodes().Get(context.TODO(), nl.nodeName, metav1.GetOptions{})
		if err != nil {
			log.Errorf("Failed to get node %s: %v", nl.nodeName, err)
			return fmt.Errorf("failed to get node %s: %v", nl.nodeName, err)
		}

		var needUpdate bool
		for key, value := range labels {
			if node.Labels == nil {
				node.Labels = make(map[string]string)
			}

			if existing, ok := node.Labels[key]; !ok || existing != value {
				node.Labels[key] = value
				needUpdate = true
			}
		}

		if !needUpdate {
			log.Info("No actual changes needed after conflict retry")
			return nil
		}

		_, err = nl.k8sClient.CoreV1().Nodes().Update(context.TODO(), node, metav1.UpdateOptions{})
		return err
	})

	if err != nil {
		log.Errorf("Failed to update node labels: %v", err)
		return fmt.Errorf("failed to update node labels: %v", err)
	}

	log.Infof("Node labels %v updated successfully", labels)
	return nil
}

func (nl *NodeLabel) getActualLabels() map[string]string {
	log.Debug("Start to get actual labels")

	labels := make(map[string]string)

	if model := cndev.GetDeviceModel(0); model != "" {
		labels[modelLabel] = model
	}

	if nl.cpuType != "" {
		labels[cpuTypeLabel] = nl.cpuType
	}

	mcuMajor, mcuMinor, mcuBuild, driverMajor, driverMinor, driverBuild, err := cndev.GetDeviceVersion(0)
	if err != nil {
		log.Error(errors.Wrapf(err, "GetDeviceVersion for slot %d", 0))
		return labels
	}
	labels[mcuVersionLabel] = fmt.Sprintf("v%d.%d.%d", mcuMajor, mcuMinor, mcuBuild)
	labels[driverVersionLabel] = fmt.Sprintf("v%d.%d.%d", driverMajor, driverMinor, driverBuild)

	log.Debugf("Actual labels are: %v", labels)

	return labels
}

func getPhysicalCPUType() string {
	data, err := os.ReadFile("/host/proc/cpuinfo")
	if err != nil {
		log.Errorf("Failed to read /proc/cpuinfo: %v", err)
		return ""
	}
	for _, line := range strings.Split(string(data), "\n") {
		if strings.HasPrefix(line, "model name") {
			if parts := strings.SplitN(line, ":", 2); len(parts) > 1 {
				return sanitizeLabelValue(strings.TrimSpace(parts[1]))
			}
		}
	}

	log.Warn("CPU model name not found in cpuinfo")
	return ""
}

func sanitizeLabelValue(value string) string {
	reg := regexp.MustCompile(`[^a-zA-Z0-9_\.-]+`)
	sanitized := reg.ReplaceAllString(value, "_")
	sanitized = strings.Trim(sanitized, "-_.")
	return sanitized
}
