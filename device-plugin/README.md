# Cambricon Device Plugin for Kubernetes

## About Cambricon Device Plugin

The Cambricon device plugin for Kubernetes is a Daemonset which allows you to automatically:

- Report the quantity of MLU on each nodes of your cluster.
- Monitor the health status of MLUs.
- Be capable to run containers with MLU enabled.

This repository contains Cambricon's official implementation of the Kubernetes device plugin.

## Prerequisites

The prerequisites for running the Cambricon device plugin:

- MLU100, MLU270, x5k, MLU220 devices
- MLU100 driver > 3.5; MLU270 driver >2.2.0; MLU220 driver > 4.1.1
- libcndev.so >= V1.8.0
- Kubernetes >= v1.11.2

## Quick Start

### Preparing your MLU Nodes

It assumes that the Cambricon drivers and neuware are installed on your MLU Nodes.

### Download and build

```shell
git clone https://github.com/Cambricon/cambricon-k8s-device-plugin.git
cd cambricon-k8s-device-plugin/device-plugin
```

Set the following environment variables if you need.

| env       | description                                                                   |
| --------- | ----------------------------------------------------------------------------- |
| APT_PROXY | apt proxy server                                                              |
| GOPROXY   | golang proxy server                                                           |
| ARCH      | target platform architecture, amd64 or arm64, amd64 by default                |
| LIBCNDEV  | absolute path of the libcndev.so binary, neuware installation path by default |

If you want to cross build, make sure docker version >= 19.03.

For amd64:

```shell
./build_image.sh
```

For arm64:

```shell
export ARCH=arm64
./build_image.sh
```

Please make sure Cambricon neuware is installed in your compiling environment.
It uses **libcndev.so** binary on your compiling machine and generates docker image in folder `./image`.

### Enabling MLU Support in Kubernetes

1. Push the docker image to the docker repo of your cluster or load the docker image on all your MLU nodes by:

   ```shell
   docker load -i image/cambricon-k8s-device-plugin-amd64.tar
   ```

2. Enable MLU support in your cluster by deploying the daemonset in [examples](examples) folder:

   Set the args in the yaml file

   ```yaml
   args:
     - --mode=default #device plugin mode: default, sriov or env-share
     - --virtualization-num=1 #  virtualization number for each MLU, used only in sriov mode or env-share mode
     #- --enable-console #uncomment to enable UART console device(/dev/ttyMS) in container
     #- --disable-health-check #uncomment to disable health check
   ```

   supported features:

   - default: default mode
   - sriov: supports SR-IOV. Set `virtualization-num` as number of VFs on host.
   - env-share: a whole card can be allocated into multiple containers. A container should use only one card in this mode.
     Set `virtualization-num` as maximum number of containers one MLU can be allocated into.

   ```shell
   kubectl create -f cambricon-device-plugin-daemonset.yml
   ```

   (Optional) If you do not want the daemonset way of deployment, edit the static pod template in examples folder and
   put the file into your configured static pod folder (`/etc/kubernetes/manifests` by default).

### Running MLU Jobs

Cambricon MLUs can now be consumed via container level resource requirements using the resource name `cambricon.com/mlu`:

```yaml
apiVersion: v1
kind: Pod
metadata:
  name: pod1
spec:
  restartPolicy: OnFailure
  containers:
    - image: ubuntu:16.04
      name: pod1-ctr
      command: ["sleep"]
      args: ["100000"]
      resources:
        limits:
          cambricon.com/mlu: 1
```
