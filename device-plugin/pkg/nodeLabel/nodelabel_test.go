package nodeLabel

import (
	"context"
	"os"
	"testing"

	"github.com/Cambricon/cambricon-k8s-device-plugin/device-plugin/pkg/cndev"
	"github.com/agiledragon/gomonkey/v2"
	"github.com/pkg/errors"
	"github.com/stretchr/testify/assert"
	corev1 "k8s.io/api/core/v1"
	metav1 "k8s.io/apimachinery/pkg/apis/meta/v1"
	"k8s.io/apimachinery/pkg/runtime"
	"k8s.io/client-go/kubernetes/fake"
	k8stesting "k8s.io/client-go/testing"
)

func TestGetActualLabels(t *testing.T) {
	stub := gomonkey.ApplyFunc(cndev.GetDeviceModel, func(uint) string {
		return "MLU370-X8"
	})
	defer stub.Reset()

	stub.ApplyFunc(cndev.GetDeviceVersion, func(uint) (uint, uint, uint, uint, uint, uint, error) {
		return 1, 2, 3, 4, 5, 6, nil
	})

	nl := &NodeLabel{cpuType: "Intel Xeon"}
	labels := nl.getActualLabels()

	assert.Equal(t, "MLU370-X8", labels[modelLabel])
	assert.Equal(t, "v1.2.3", labels[mcuVersionLabel])
	assert.Equal(t, "v4.5.6", labels[driverVersionLabel])
	assert.Equal(t, "Intel Xeon", labels[cpuTypeLabel])
}

func TestGetActualLabels_HardwareError(t *testing.T) {
	stub := gomonkey.ApplyFunc(cndev.GetDeviceModel, func(uint) string {
		return "MLU370-X8"
	})
	defer stub.Reset()

	stub.ApplyFunc(cndev.GetDeviceVersion, func(uint) (uint, uint, uint, uint, uint, uint, error) {
		return 0, 0, 0, 0, 0, 0, errors.New("device error")
	})

	nl := &NodeLabel{cpuType: "Intel Xeon"}
	labels := nl.getActualLabels()

	assert.Equal(t, "MLU370-X8", labels[modelLabel])
	assert.Empty(t, labels[mcuVersionLabel])
	assert.Empty(t, labels[driverVersionLabel])
	assert.Equal(t, "Intel Xeon", labels[cpuTypeLabel])
}

func TestGetPhysicalCPUType(t *testing.T) {
	testContent := `processor    : 0
vendor_id    : GenuineIntel
model name   : Intel(R) Xeon(R) CPU E5-2680 v4 @ 2.40GHz

processor    : 1
vendor_id    : GenuineIntel
model name   : Intel(R) Xeon(R) CPU E5-2680 v4 @ 2.40GHz
...`

	patch := gomonkey.ApplyFunc(os.ReadFile, func(string) ([]byte, error) {
		return []byte(testContent), nil
	})
	defer patch.Reset()

	result := getPhysicalCPUType()
	assert.Equal(t, "Intel_R_Xeon_R_CPU_E5-2680_v4_2.40GHz", result)
}

func TestLabelsNeedUpdate(t *testing.T) {
	tests := []struct {
		name     string
		existing map[string]string
		desired  map[string]string
		expected bool
	}{
		{
			name:     "no change",
			existing: map[string]string{driverVersionLabel: "v1.0.0"},
			desired:  map[string]string{driverVersionLabel: "v1.0.0"},
			expected: false,
		},
		{
			name:     "new label",
			existing: map[string]string{},
			desired:  map[string]string{driverVersionLabel: "v1.0.0"},
			expected: true,
		},
		{
			name:     "label change",
			existing: map[string]string{driverVersionLabel: "v1.0.0"},
			desired:  map[string]string{driverVersionLabel: "v1.1.0"},
			expected: true,
		},
		{
			name:     "empty existing label",
			existing: map[string]string{driverVersionLabel: ""},
			desired:  map[string]string{driverVersionLabel: "v1.1.0"},
			expected: true,
		},
		{
			name:     "empty desired label",
			existing: map[string]string{driverVersionLabel: "v1.1.0"},
			desired:  map[string]string{driverVersionLabel: ""},
			expected: true,
		},
	}

	for _, tt := range tests {
		t.Run(tt.name, func(t *testing.T) {
			nl := &NodeLabel{}
			result := nl.labelsNeedUpdate(tt.existing, tt.desired)
			assert.Equal(t, tt.expected, result)
		})
	}
}

func TestUpdateNodeLabels(t *testing.T) {
	initNode := &corev1.Node{
		ObjectMeta: metav1.ObjectMeta{
			Name:   "test-node",
			Labels: map[string]string{driverVersionLabel: "v1.0.0"},
		},
	}

	fakeClient := fake.NewSimpleClientset(initNode)

	fakeClient.Fake.PrependReactor("update", "test-node", func(action k8stesting.Action) (handled bool, ret runtime.Object, err error) {
		updateAction := action.(k8stesting.UpdateAction)
		updatedNode := updateAction.GetObject().(*corev1.Node)

		assert.Contains(t, updatedNode.Labels, driverVersionLabel)
		return true, updatedNode, nil
	})

	nl := &NodeLabel{
		k8sClient: fakeClient,
		nodeName:  "test-node",
	}

	testLabels := map[string]string{
		cpuTypeLabel:       "Intel_R_Xeon_R_Gold_5118_CPU_2.30GHz",
		driverVersionLabel: "v1.2.3",
		modelLabel:         "MLU370-X8",
	}
	err := nl.updateNodeLabels(testLabels)
	assert.NoError(t, err)

	updatedNode, _ := fakeClient.CoreV1().Nodes().Get(context.TODO(), "test-node", metav1.GetOptions{})
	assert.Equal(t, "v1.2.3", updatedNode.Labels[driverVersionLabel])
	assert.Equal(t, "Intel_R_Xeon_R_Gold_5118_CPU_2.30GHz", updatedNode.Labels[cpuTypeLabel])
}
