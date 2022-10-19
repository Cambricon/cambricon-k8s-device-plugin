#!/bin/bash
# Copyright 2020 Cambricon, Inc.
#
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
#      http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.

set -e

function test_mlu_default() {
    echo "==================================="
    echo "Test MLU Device Plugin Default Mode"
    echo "==================================="
    sed -i "s|--mode=.*|--mode=default|" test/cambricon-device-plugin.yml
    sed -i "s|#- --enable-device-type|- --enable-device-type|" test/cambricon-device-plugin.yml
    sed "s|replicas: .*|replicas: 2|" examples/deployment.yaml >test/mock-deployment.yaml
    sed -i "s|cambricon.com/mlu: .*|cambricon.com/mlu290: 2|" test/mock-deployment.yaml
    kubectl create -f test/cambricon-device-plugin.yml
    mapfile -t pods < <(kubectl get pods -n kube-system -o name -l name=cambricon-device-plugin-ds)
    for pod in "${pods[@]}"; do
        kubectl wait --for=condition=Ready "$pod" -n kube-system --timeout=120s
    done
    kubectl create -f test/mock-deployment.yaml
    mapfile -t pods < <(kubectl get pods -o name -l app=binpack-1)
    for pod in "${pods[@]}"; do
        kubectl wait --for=condition=Ready "$pod" --timeout=120s
        test "$(kubectl exec "${pod:4}" -- ls -l /dev | grep cambricon_dev -c)" -eq 2
        test "$(kubectl exec "${pod:4}" -- ls -l /dev | grep cambr-msgq -c)" -eq 2
        test "$(kubectl exec "${pod:4}" -- ls -l /dev | grep cambr-rpc -c)" -eq 2
        test "$(kubectl exec "${pod:4}" -- ls -l /dev | grep cmsg_ctrl -c)" -eq 2
        test "$(kubectl exec "${pod:4}" -- ls -l /dev | grep commu -c)" -eq 2
        test "$(kubectl exec "${pod:4}" -- ls -l /dev | grep cambricon_ctl -c)" -eq 1
        test "$(kubectl exec "${pod:4}" -- ls -l /dev | grep cambricon_ipcm -c)" -eq 2
    done
    sed -i "s|- --enable-device-type|#- --enable-device-type|" test/cambricon-device-plugin.yml
}

function test_mlu_env_share() {
    echo "====================================="
    echo "Test MLU Device Plugin Env Share Mode"
    echo "====================================="
    sed -i "s|--mode=.*|--mode=env-share|" test/cambricon-device-plugin.yml
    sed "s|replicas: .*|replicas: 16|" examples/deployment.yaml >test/mock-deployment.yaml
    sed -i "s|cambricon.com/mlu: .*|cambricon.com/mlu: 1|" test/mock-deployment.yaml
    sed -i "s|virtualization-num=1|virtualization-num=2|" test/cambricon-device-plugin.yml
    sed -i "s|#- --enable-console|- --enable-console|" test/cambricon-device-plugin.yml
    sudo rm -f /dev/cambricon_ipcm*
    kubectl create -f test/cambricon-device-plugin.yml
    mapfile -t pods < <(kubectl get pods -n kube-system -o name -l name=cambricon-device-plugin-ds)
    for pod in "${pods[@]}"; do
        kubectl wait --for=condition=Ready "$pod" -n kube-system --timeout=120s
    done
    kubectl create -f test/mock-deployment.yaml
    mapfile -t pods < <(kubectl get pods -o name -l app=binpack-1)
    for pod in "${pods[@]}"; do
        kubectl wait --for=condition=Ready "$pod" --timeout=120s
        test "$(kubectl exec "${pod:4}" -- ls -l /dev | grep cambricon_dev -c)" -eq 1
        test "$(kubectl exec "${pod:4}" -- ls -l /dev | grep cambr-msgq -c)" -eq 1
        test "$(kubectl exec "${pod:4}" -- ls -l /dev | grep cambr-rpc -c)" -eq 1
        test "$(kubectl exec "${pod:4}" -- ls -l /dev | grep cmsg_ctrl -c)" -eq 1
        test "$(kubectl exec "${pod:4}" -- ls -l /dev | grep commu -c)" -eq 1
        test "$(kubectl exec "${pod:4}" -- ls -l /dev | grep cambricon_ctl -c)" -eq 1
        test "$(kubectl exec "${pod:4}" -- ls -l /dev | grep ttyMS -c)" -eq 1
    done
}

function test_mlu_topology() {
    echo "=========================================="
    echo "Test MLU Device Plugin Topology Aware Mode"
    echo "=========================================="
    sed -i "s|--mode=.*|--mode=topology-aware|" test/cambricon-device-plugin.yml
    sed -i "s|--mlulink-policy=.*|--mlulink-policy=guaranteed|" test/cambricon-device-plugin.yml
    sed "s|replicas: .*|replicas: 2|" examples/deployment.yaml >test/mock-deployment.yaml
    sed -i "s|cambricon.com/mlu: .*|cambricon.com/mlu: 2|" test/mock-deployment.yaml
    kubectl create -f test/cambricon-device-plugin.yml
    mapfile -t pods < <(kubectl get pods -n kube-system -o name -l name=cambricon-device-plugin-ds)
    for pod in "${pods[@]}"; do
        kubectl wait --for=condition=Ready "$pod" -n kube-system --timeout=120s
    done
    kubectl create -f test/mock-deployment.yaml
    mapfile -t pods < <(kubectl get pods -o name -l app=binpack-1)
    for pod in "${pods[@]}"; do
        kubectl wait --for=condition=Ready "$pod" --timeout=120s
        test "$(kubectl exec "${pod:4}" -- ls -l /dev | grep cambricon_dev -c)" -eq 2
    done
    read -r -a containers <<<"$(kubectl get pods -o jsonpath="{.items[*].status.containerStatuses[*].containerID}" -l app=binpack-1)"
    allocated=()
    for container in "${containers[@]}"; do
        id=$(echo "${container}" | cut -f3 -d'/')
        mapfile -t devices < <(docker inspect --format='{{range .HostConfig.Devices }}{{.PathOnHost}}{{"\n"}}{{end}}' "${id}")
        for device in "${devices[@]}"; do
            [[ $device == /dev/cambricon_dev* ]] && allocated+=("${device}")
        done
    done
    echo "${allocated[@]}"
    set1=("/dev/cambricon_dev0" "/dev/cambricon_dev1" "/dev/cambricon_dev4" "/dev/cambricon_dev5")
    set2=("/dev/cambricon_dev2" "/dev/cambricon_dev3" "/dev/cambricon_dev6" "/dev/cambricon_dev7")
    diff1=$(echo "${set1[@]}" "${allocated[@]}" | tr ' ' '\n' | sort | uniq -u | wc -l)
    diff2=$(echo "${set2[@]}" "${allocated[@]}" | tr ' ' '\n' | sort | uniq -u | wc -l)
    test "$diff1" -eq 0 -o "$diff2" -eq 0
}

function test_mlu() {
    if [ "$1" == "default" ]; then
        test_mlu_default
    elif [ "$1" == "env_share" ]; then
        test_mlu_env_share
    elif [ "$1" == "topology_aware" ]; then
        test_mlu_topology
    fi
}

test_mlu "$1"
