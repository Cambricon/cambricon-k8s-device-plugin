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

function test_mlu100() {
    sed "s|replicas: .*|replicas: 2|" examples/deployment.yaml >test/mock-deployment.yaml
    sed -i "s|cambricon.com/mlu: .*|cambricon.com/mlu: 2|" test/mock-deployment.yaml
    kubectl create -f test/cambricon-device-plugin.yml
    pod=$(kubectl get pods -n kube-system -o name -l name=cambricon-device-plugin-ds)
    kubectl wait --for=condition=Ready "$pod" -n kube-system --timeout=120s
    kubectl create -f test/mock-deployment.yaml
    mapfile -t PODS < <( kubectl get pods -o name -l app=binpack-1 )
    for var in "${PODS[@]}"; do
        kubectl wait --for=condition=Ready "$var" --timeout=120s
        test "$(kubectl exec -ti "${var:4}" -- ls -l /dev | grep cambricon_c10Dev -c)" -eq 2
        test "$(kubectl exec -ti "${var:4}" -- ls -l /dev | grep cnmon_dev -c)" -eq 1
        test "$(kubectl exec -ti "${var:4}" -- ls -l /dev | grep cncodec_dev -c)" -eq 1
    done
}

function test_mlu270_default() {
    sed "s|replicas: .*|replicas: 2|" examples/deployment.yaml >test/mock-deployment.yaml
    sed -i "s|cambricon.com/mlu: .*|cambricon.com/mlu: 2|" test/mock-deployment.yaml
    kubectl create -f test/cambricon-device-plugin.yml
    pod=$(kubectl get pods -n kube-system -o name -l name=cambricon-device-plugin-ds)
    kubectl wait --for=condition=Ready "$pod" -n kube-system --timeout=120s
    kubectl create -f test/mock-deployment.yaml
    mapfile -t PODS < <( kubectl get pods -o name -l app=binpack-1 )
    for var in "${PODS[@]}"; do
        kubectl wait --for=condition=Ready "$var" --timeout=120s
        test "$(kubectl exec -ti "${var:4}" -- ls -l /dev | grep cambricon_dev -c)" -eq 2
        test "$(kubectl exec -ti "${var:4}" -- ls -l /dev | grep cambr-msgq -c)" -eq 2
        test "$(kubectl exec -ti "${var:4}" -- ls -l /dev | grep cambr-rpc -c)" -eq 2
        test "$(kubectl exec -ti "${var:4}" -- ls -l /dev | grep cmsg_ctrl -c)" -eq 2
        test "$(kubectl exec -ti "${var:4}" -- ls -l /dev | grep commu -c)" -eq 2
        test "$(kubectl exec -ti "${var:4}" -- ls -l /dev | grep cambricon_ctl -c)" -eq 1
    done
}

function test_mlu270_env_share() {
    sed -i "s|default|env-share|" test/cambricon-device-plugin.yml
    sed "s|replicas: .*|replicas: 16|" examples/deployment.yaml >test/mock-deployment.yaml
    sed -i "s|cambricon.com/mlu: .*|cambricon.com/mlu: 1|" test/mock-deployment.yaml
    sed -i "s|value: \"0\"|value: \"2\"|" test/cambricon-device-plugin.yml
    kubectl create -f test/cambricon-device-plugin.yml
    pod=$(kubectl get pods -n kube-system -o name -l name=cambricon-device-plugin-ds)
    kubectl wait --for=condition=Ready "$pod" -n kube-system --timeout=120s
    kubectl create -f test/mock-deployment.yaml
    mapfile -t PODS < <( kubectl get pods -o name -l app=binpack-1 )
    for var in "${PODS[@]}"; do
        kubectl wait --for=condition=Ready "$var" --timeout=120s
        test "$(kubectl exec -ti "${var:4}" -- ls -l /dev | grep cambricon_dev -c)" -eq 1
        test "$(kubectl exec -ti "${var:4}" -- ls -l /dev | grep cambr-msgq -c)" -eq 1
        test "$(kubectl exec -ti "${var:4}" -- ls -l /dev | grep cambr-rpc -c)" -eq 1
        test "$(kubectl exec -ti "${var:4}" -- ls -l /dev | grep cmsg_ctrl -c)" -eq 1
        test "$(kubectl exec -ti "${var:4}" -- ls -l /dev | grep commu -c)" -eq 1
        test "$(kubectl exec -ti "${var:4}" -- ls -l /dev | grep cambricon_ctl -c)" -eq 1
    done
}

function test_mlu270() {
    if [ "$1" == "default" ]; then
        test_mlu270_default
    elif [ "$1" == "env_share" ]; then
        test_mlu270_env_share
    fi
}

tag=$(date +%s)
mock_file="test/mockmlu100.json"

if [ "$1" == "MLU100" ]; then
    echo "Test MLU100 Device Plugin"
    mock_file="test/mockmlu100.json"
elif [ "$1" == "MLU270" ]; then
    echo "Test MLU270 Device Plugin"
    mock_file="test/mockmlu270.json"
fi

cp $mock_file test/mock.json

TAG=$tag LIBCNDEV=pkg/cndev/mock/libcndev.so ./build_image.sh
docker build --build-arg "BASE_IMAGE=cambricon-k8s-device-plugin:$tag" -t "cambricon-k8s-device-plugin:mock-$tag" test

sed "s|cambricon-k8s-device-plugin:[A-Za-z0-9\.]*|cambricon-k8s-device-plugin:mock-$tag|" examples/cambricon-device-plugin-daemonset.yaml >test/cambricon-device-plugin.yml

if [ "$1" == "MLU100" ]; then
    test_mlu100
fi

if [ "$1" == "MLU270" ]; then
    test_mlu270 "$2"
fi
