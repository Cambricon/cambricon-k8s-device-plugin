#!/bin/bash

function do_recover()
{
    echo "Recovering testing enviroments"
    ###mock MLU100###
    sudo mknod /dev/cambricon_c10Dev0 c 239 0 || true
    sudo mknod /dev/cambricon_c10Dev1 c 239 1 || true
    sudo mknod /dev/cambricon_c10Dev2 c 239 2 || true
    sudo mknod /dev/cambricon_c10Dev3 c 239 3 || true
    sudo mknod /dev/cncodec_dev c 10 58 || true
    sudo mknod /dev/cnmon_dev c 238 0 || true

    ###mock MLU270###
    sudo mknod /dev/cambricon_dev0 c 240 0 || true
    sudo mknod /dev/cambricon_dev1 c 240 1 || true
    sudo mknod /dev/cambricon_dev2 c 240 2 || true
    sudo mknod /dev/cambricon_dev3 c 240 3 || true

    sudo mknod /dev/cambr-msgq:0 c 241 0 || true
    sudo mknod /dev/cambr-msgq:1 c 241 1 || true
    sudo mknod /dev/cambr-msgq:2 c 241 2 || true
    sudo mknod /dev/cambr-msgq:3 c 241 3 || true
    
    sudo mknod /dev/cambr-rpc:0 c 242 0 || true
    sudo mknod /dev/cambr-rpc:1 c 242 1 || true
    sudo mknod /dev/cambr-rpc:2 c 242 2 || true
    sudo mknod /dev/cambr-rpc:3 c 242 3 || true

    sudo mknod /dev/cmsg_ctrl0 c 243 0 || true
    sudo mknod /dev/cmsg_ctrl1 c 243 1 || true
    sudo mknod /dev/cmsg_ctrl2 c 243 2 || true
    sudo mknod /dev/cmsg_ctrl3 c 243 3 || true

    sudo mknod /dev/commu0 c 244 0 || true
    sudo mknod /dev/commu1 c 244 1 || true
    sudo mknod /dev/commu2 c 244 2 || true
    sudo mknod /dev/commu3 c 244 3 || true

    sudo mknod /dev/cambricon_ctl c 245 0 || true

    kubectl delete -f cambricon-device-plugin.yml || true
    PODS=$(kubectl get pods -n kube-system  -o name -l name=cambricon-device-plugin-ds) || true
    kubectl wait --for=delete ${PODS[0]} -n kube-system --timeout=120s || true
    rm -f test/mock.json || true
    kubectl delete -f test/mock-deployment.yaml || true
    PODS=($(kubectl get pods -o name -l app=binpack-1)) || true
    kubectl wait --for=delete ${PODS[0]}  --timeout=60s || true

    docker rmi $(docker images | grep cambricon-k8s-device-plugin | tr -s ' ' | cut -d ' ' -f 3) || true

    noneImg=$(docker images -f "dangling=true" -q)
    docker rmi "${noneImg}" || true
}

#main

do_recover

bash ./test/do-deploy-test-device-plugin.sh MLU100 || { echo "MLU100 Device Plugin Test Failed"; exit 1; }

echo "=================================="
echo "MLU100 Device Plugin Test Passed"
echo "=================================="

do_recover

bash ./test/do-deploy-test-device-plugin.sh MLU270 || { echo "MLU270 Device Plugin Test Failed"; exit 1; }

echo "=================================="
echo "MLU270 Device Plugin Test Passed"
echo "=================================="
