set -e

tag=$(date +%s)
mock_file="test/mockmlu100.json"

if [ "$1" == "MLU100" ]; then
    echo "Test MLU100 Device Plugin"
    mock_file="test/mockmlu100.json"
elif [ "$1" == "MLU270" ]; then
    echo "Test MLU270 Device Plugin"
    mock_file="test/mockmlu270.json"
fi

echo "=================================="
echo   "=====Begining to test $1======"
echo "=================================="

cp $mock_file test/mock.json

docker build -t "cambricon-k8s-device-plugin:mock-$tag" -f test/Dockerfile.mock .

sed "s#cambricon-k8s-device-plugin:[A-Za-z0-9\.]*#cambricon-k8s-device-plugin:mock-$tag#" cambricon-device-plugin.yml > test/cambricon-device-plugin.yml

kubectl create -f test/cambricon-device-plugin.yml

PODS=($(kubectl get pods -n kube-system  -o name -l name=cambricon-device-plugin-ds))
kubectl wait --for=condition=Ready "${PODS[0]}" -n kube-system --timeout=60s

kubectl create -f test/mock-deployment.yaml

sleep 1

PODS=($(kubectl get pods -o name -l app=binpack-1))
for var in "${PODS[@]}"
do
    kubectl wait --for=condition=Ready "$var" --timeout=60s
    if [ "$?" -ne 0 ]; then
        echo "failed to create mlu enabled pod."
        exit 1
    fi
    if [ "$1" == "MLU100" ]; then
        test "$(kubectl exec -ti "${var: 4}"  -- ls -l /dev | grep cambricon_c10Dev -c)" -eq 1
        if [ $? -ne 0 ]; then
            echo "failed to find cambricon_c10Dev."
            exit 1
        fi
        kubectl exec -ti "${var: 4}" -- stat /dev/cnmon_dev
        if [ $? -ne 0 ]; then
            echo "failed to find cnmon_dev."
            exit 1
        fi
        kubectl exec -ti "${var: 4}" -- stat /dev/cncodec_dev
        if [ $? -ne 0 ]; then
            echo "failed to find cncodec_dev."
            exit 1
        fi
    else
        test "$(kubectl exec -ti "${var: 4}" -- ls -l /dev | grep cambricon_dev -c)" -eq 1
        if [ $? -ne 0 ]; then
                    echo "failed to find cambricon_dev."
            exit 1
        fi

        kubectl exec -ti "${var: 4}" -- stat /dev/cambricon_ctl
        if [ $? -ne 0 ]; then
            echo "failed  to find cambricon_ctl."
            exit 1
        fi
    fi
done
