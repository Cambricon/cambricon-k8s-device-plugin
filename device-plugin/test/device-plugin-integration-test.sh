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

function mock_mlu100() {
    sudo mknod /dev/cambricon_c10Dev0 c 239 0 || true
    sudo mknod /dev/cambricon_c10Dev1 c 239 1 || true
    sudo mknod /dev/cambricon_c10Dev2 c 239 2 || true
    sudo mknod /dev/cambricon_c10Dev3 c 239 3 || true
    sudo mknod /dev/cncodec_dev c 10 58 || true
    sudo mknod /dev/cnmon_dev c 238 0 || true
}

function mock_mlu270() {
    sudo mknod /dev/cambricon_dev0 c 240 0 || true
    sudo mknod /dev/cambricon_dev1 c 240 1 || true
    sudo mknod /dev/cambricon_dev2 c 240 2 || true
    sudo mknod /dev/cambricon_dev3 c 240 3 || true
    sudo mknod /dev/cambricon_dev4 c 240 4 || true
    sudo mknod /dev/cambricon_dev5 c 240 5 || true
    sudo mknod /dev/cambricon_dev6 c 240 6 || true
    sudo mknod /dev/cambricon_dev7 c 240 7 || true

    sudo mknod /dev/cambr-msgq:0 c 241 0 || true
    sudo mknod /dev/cambr-msgq:1 c 241 1 || true
    sudo mknod /dev/cambr-msgq:2 c 241 2 || true
    sudo mknod /dev/cambr-msgq:3 c 241 3 || true
    sudo mknod /dev/cambr-msgq:4 c 241 4 || true
    sudo mknod /dev/cambr-msgq:5 c 241 5 || true
    sudo mknod /dev/cambr-msgq:6 c 241 6 || true
    sudo mknod /dev/cambr-msgq:7 c 241 7 || true

    sudo mknod /dev/cambr-rpc:0 c 242 0 || true
    sudo mknod /dev/cambr-rpc:1 c 242 1 || true
    sudo mknod /dev/cambr-rpc:2 c 242 2 || true
    sudo mknod /dev/cambr-rpc:3 c 242 3 || true
    sudo mknod /dev/cambr-rpc:4 c 242 4 || true
    sudo mknod /dev/cambr-rpc:5 c 242 5 || true
    sudo mknod /dev/cambr-rpc:6 c 242 6 || true
    sudo mknod /dev/cambr-rpc:7 c 242 7 || true

    sudo mknod /dev/cmsg_ctrl0 c 243 0 || true
    sudo mknod /dev/cmsg_ctrl1 c 243 1 || true
    sudo mknod /dev/cmsg_ctrl2 c 243 2 || true
    sudo mknod /dev/cmsg_ctrl3 c 243 3 || true
    sudo mknod /dev/cmsg_ctrl4 c 243 4 || true
    sudo mknod /dev/cmsg_ctrl5 c 243 5 || true
    sudo mknod /dev/cmsg_ctrl6 c 243 6 || true
    sudo mknod /dev/cmsg_ctrl7 c 243 7 || true

    sudo mknod /dev/commu0 c 244 0 || true
    sudo mknod /dev/commu1 c 244 1 || true
    sudo mknod /dev/commu2 c 244 2 || true
    sudo mknod /dev/commu3 c 244 3 || true
    sudo mknod /dev/commu4 c 244 4 || true
    sudo mknod /dev/commu5 c 244 5 || true
    sudo mknod /dev/commu6 c 244 6 || true
    sudo mknod /dev/commu7 c 244 7 || true

    sudo mknod /dev/cambricon_ctl c 245 0 || true
}

function remove_devices() {
    sudo rm -f /dev/cambricon_c10Dev* || true
    sudo rm -f /dev/cncodec_dev || true
    sudo rm -f /dev/cnmon_dev || true

    sudo rm -f /dev/cambricon_dev* || true
    sudo rm -f /dev/cambr-msgq* || true
    sudo rm -f /dev/cambr-rpc* || true
    sudo rm -f /dev/cmsg_ctrl* || true
    sudo rm -f /dev/commu* || true
    sudo rm -f /dev/cambricon_ctl || true
}

function do_recover() {
    echo "Recovering testing environments"

    remove_devices

    kubectl delete ds cambricon-device-plugin-daemonset -n kube-system || true
    pod=$(kubectl get pods -n kube-system -o name -l name=cambricon-device-plugin-ds) || true
    kubectl wait --for=delete "$pod" -n kube-system --timeout=120s || true
    rm -f test/mock.json || true
    kubectl delete deployment binpack-n1 || true
    mapfile -t PODS < <(kubectl get pods -o name -l app=binpack-1) || true
    kubectl wait --for=delete "${PODS[0]}" --timeout=120s || true
    mapfile -t images < <(docker images -q cambricon-k8s-device-plugin) || true
    mapfile -t -O "${#images[@]}" images < <(docker images -f "dangling=true" -q) || true
    for image in "${images[@]}"; do
        docker rmi "$image" || true
    done
}

do_recover

mock_mlu100

./test/do-test-device-plugin.sh MLU100 || {
    echo "MLU100 Device Plugin Test Failed"
    exit 1
}

echo "=================================="
echo "MLU100 Device Plugin Test Passed"
echo "=================================="

do_recover

mock_mlu270
./test/do-test-device-plugin.sh MLU270 default || {
    echo "MLU270 Device Plugin Test Default Mode Failed"
    exit 1
}

do_recover

mock_mlu270
./test/do-test-device-plugin.sh MLU270 env_share || {
    echo "MLU270 Device Plugin Test Env Share Mode Failed"
    exit 1
}

echo "=================================="
echo "MLU270 Device Plugin Test Passed"
echo "=================================="
