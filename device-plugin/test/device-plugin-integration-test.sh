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

function mock_devices() {
    sudo mknod /dev/cambricon_dev0 c 240 0
    sudo mknod /dev/cambricon_dev1 c 240 1
    sudo mknod /dev/cambricon_dev2 c 240 2
    sudo mknod /dev/cambricon_dev3 c 240 3
    sudo mknod /dev/cambricon_dev4 c 240 4
    sudo mknod /dev/cambricon_dev5 c 240 5
    sudo mknod /dev/cambricon_dev6 c 240 6
    sudo mknod /dev/cambricon_dev7 c 240 7

    sudo mknod /dev/cambr-msgq:0 c 241 0
    sudo mknod /dev/cambr-msgq:1 c 241 1
    sudo mknod /dev/cambr-msgq:2 c 241 2
    sudo mknod /dev/cambr-msgq:3 c 241 3
    sudo mknod /dev/cambr-msgq:4 c 241 4
    sudo mknod /dev/cambr-msgq:5 c 241 5
    sudo mknod /dev/cambr-msgq:6 c 241 6
    sudo mknod /dev/cambr-msgq:7 c 241 7

    sudo mknod /dev/cambr-rpc:0 c 242 0
    sudo mknod /dev/cambr-rpc:1 c 242 1
    sudo mknod /dev/cambr-rpc:2 c 242 2
    sudo mknod /dev/cambr-rpc:3 c 242 3
    sudo mknod /dev/cambr-rpc:4 c 242 4
    sudo mknod /dev/cambr-rpc:5 c 242 5
    sudo mknod /dev/cambr-rpc:6 c 242 6
    sudo mknod /dev/cambr-rpc:7 c 242 7

    sudo mknod /dev/cmsg_ctrl0 c 243 0
    sudo mknod /dev/cmsg_ctrl1 c 243 1
    sudo mknod /dev/cmsg_ctrl2 c 243 2
    sudo mknod /dev/cmsg_ctrl3 c 243 3
    sudo mknod /dev/cmsg_ctrl4 c 243 4
    sudo mknod /dev/cmsg_ctrl5 c 243 5
    sudo mknod /dev/cmsg_ctrl6 c 243 6
    sudo mknod /dev/cmsg_ctrl7 c 243 7

    sudo mknod /dev/commu0 c 244 0
    sudo mknod /dev/commu1 c 244 1
    sudo mknod /dev/commu2 c 244 2
    sudo mknod /dev/commu3 c 244 3
    sudo mknod /dev/commu4 c 244 4
    sudo mknod /dev/commu5 c 244 5
    sudo mknod /dev/commu6 c 244 6
    sudo mknod /dev/commu7 c 244 7

    sudo mknod /dev/cambricon_ctl c 245 0

    sudo mknod /dev/ttyMS0 c 246 0
    sudo mknod /dev/ttyMS1 c 246 1
    sudo mknod /dev/ttyMS2 c 246 2
    sudo mknod /dev/ttyMS3 c 246 3
    sudo mknod /dev/ttyMS4 c 246 4
    sudo mknod /dev/ttyMS5 c 246 5
    sudo mknod /dev/ttyMS6 c 246 6
    sudo mknod /dev/ttyMS7 c 246 7

    sudo mknod /dev/cambricon_ipcm0 c 247 0
    sudo mknod /dev/cambricon_ipcm1 c 247 1
    sudo mknod /dev/cambricon_ipcm2 c 247 2
    sudo mknod /dev/cambricon_ipcm3 c 247 3
    sudo mknod /dev/cambricon_ipcm4 c 247 4
    sudo mknod /dev/cambricon_ipcm5 c 247 5
    sudo mknod /dev/cambricon_ipcm6 c 247 6
    sudo mknod /dev/cambricon_ipcm7 c 247 7
}

function remove_devices() {
    sudo rm -f /dev/cambricon_dev*
    sudo rm -f /dev/cambr-msgq*
    sudo rm -f /dev/cambr-rpc*
    sudo rm -f /dev/cmsg_ctrl*
    sudo rm -f /dev/commu*
    sudo rm -f /dev/cambricon_ctl
    sudo rm -f /dev/ttyMS*
    sudo rm -f /dev/cambricon_ipcm*
}

function prepare_image() {
    tag=$(date +%s)
    TAG=$tag LIBCNDEV=pkg/cndev/mock/libcndev.so LIBCNTOPO=pkg/cntopo/test/libcntopo.so ./build_image.sh
    docker build --build-arg "BASE_IMAGE=cambricon-device-plugin:$tag" -t "cambricon-device-plugin:mock-$tag" test
    sed "s|cambricon-device-plugin:[A-Za-z0-9\.]*|cambricon-device-plugin:mock-$tag|" examples/cambricon-device-plugin-daemonset.yaml >test/cambricon-device-plugin.yml
    cp examples/cambricon-device-plugin-config.yaml test/
}

function remove_images() {
    mapfile -t stopped < <(docker ps --filter status=exited -q)
    for container in "${stopped[@]}"; do
        docker rm "$container"
    done
    mapfile -t images < <(docker images -q cambricon-device-plugin)
    mapfile -t -O "${#images[@]}" images < <(docker images -f "dangling=true" -q)
    for image in "${images[@]}"; do
        docker rmi "$image"
    done
}

function do_recover() {
    echo "Recovering testing environments"
    mapfile -t pods < <(kubectl get pods -n kube-system -o name -l name=cambricon-device-plugin-ds)
    kubectl delete ds cambricon-device-plugin-daemonset -n kube-system || true
    kubectl wait --for=delete "${pods[0]}" -n kube-system --timeout=120s || true
    mapfile -t pods < <(kubectl get pods -o name -l app=binpack-1)
    kubectl delete deployment binpack-n1 || true
    kubectl wait --for=delete "${pods[0]}" --timeout=120s || true
    sleep 30
}

do_recover
remove_devices
remove_images

mock_devices
prepare_image

do_recover
./test/do-test-device-plugin.sh default || {
    echo "Device Plugin Test Default Mode Failed"
    exit 1
}

do_recover
./test/do-test-device-plugin.sh env_share || {
    echo "Device Plugin Test Env Share Mode Failed"
    exit 1
}

do_recover
./test/do-test-device-plugin.sh topology_aware || {
    echo "Device Plugin Test Topology Aware Mode Failed"
    exit 1
}

echo "============================="
echo "MLU Device Plugin Test Passed"
echo "============================="
