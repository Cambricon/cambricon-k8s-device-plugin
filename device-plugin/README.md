# Cambricon Device Plugin for Kubernetes

## About Cambricon Device Plugin

The Cambricon device plugin for Kubernetes is a Daemonset which allows you to automatically:

- Report the quantity of MLU on each nodes of your cluster.
- Monitor the health status of MLUs.
- Be capable to run containers with MLU enabled.

This repository contains Cambricon's official implementation of the Kubernetes device plugin.

## Prerequisites

The prerequisites for running the Cambricon Device Plugin:

- MLU270, MLU270-X5K, MLU220, MLU290, MLU370 devices
- MLU driver >= 4.15.2

For MLU driver version < 4.15.2, please use [release v1.1.3].

For Kubernetes version < 1.19.0, MLU290 mlulink topology awareness can not be used. If you want to use this feature, make sure your Kubernetes version >= 1.19.0.

## Quick Start

### Preparing your MLU Nodes

It is assumed that Cambricon MLU driver and neuware are installed on your MLU Nodes.

### Download and build

```shell
git clone https://github.com/Cambricon/cambricon-k8s-device-plugin.git
cd cambricon-k8s-device-plugin/device-plugin
```

Set the following environment variables if you need.

| env        | description                                                                   |
| ---------- | ----------------------------------------------------------------------------- |
| APT_PROXY  | apt proxy address                                                             |
| GOPROXY    | golang proxy address                                                          |
| ARCH       | target platform architecture, amd64 or arm64, amd64 by default                |
| LIBCNDEV   | absolute path of the libcndev.so binary, neuware installation path by default |
| BASE_IMAGE | device plugin base image                                                      |

If you want to cross build, make sure docker version >= 19.03.

For amd64:

```shell
GOPROXY=https://goproxy.cn ./build_image.sh
```

For arm64:

```shell
ARCH=arm64 GOPROXY=https://goproxy.cn ./build_image.sh
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
     - --mode=default #device plugin mode: default, sriov, env-share or topology-aware
     - --virtualization-num=1 #  virtualization number for each MLU, used only in sriov mode or env-share mode
     - --mlulink-policy=best-effort # MLULink topology policy: best-effort, guaranteed or restricted, used only in topology-aware mode
     - --cnmon-path=/usr/bin/cnmon # host machine cnmon path, must be absolute path. comment out this line to avoid mounting cnmon.
     #- --enable-console #uncomment to enable UART console device(/dev/ttyMS) in container
     #- --disable-health-check #uncomment to disable health check
     #- --enable-device-type #uncomment to enable device registration with type info
   ```

   supported features:

   - default: default mode
   - sriov: supports SR-IOV. Set `virtualization-num` as number of VFs on host.
   - env-share: a whole card can be allocated into multiple containers. A container should use only one card in this mode.
     Set `virtualization-num` as maximum number of containers one MLU can be allocated into.
   - topology-aware: device plugin is aware of MLULink topology and tries to allocate MLUs forming a cycle. Set `mlulink-policy` as described below. **Only supports MLU290 for now.**

   MLULink topology policies, guaranteed and restricted only works for 1,2,4,8 requested MLUs:

   - best-effort: allocate devices forming maximum number of cycles whenever possible
   - guaranteed: allocated devices must form at least one cycle, otherwise returns an error
   - restricted: for 2 MLUs, allocated devices must have 2 mlulinks, and for 4 MLUs, allocated devices must on one mother board, otherwise returns an error

   ```shell
   kubectl create -f examples/cambricon-device-plugin-daemonset.yaml
   ```

   (Optional) If you do not want the daemonset way of deployment, edit the static pod template in examples folder and
   put the file into your configured static pod folder (`/etc/kubernetes/manifests` by default).

3. If you want to use MLU290 topology-aware mode under guaranteed or restricted policy, enable device plugin to get and update nodes.

   ```shell
   kubectl create -f examples/cambricon-device-plugin-rbac.yaml
   ```

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
          cambricon.com/mlu: "1" # use this when device type is not enabled, else delete this line.
          #camrbricon.com/mlu270: "1" # use this when device type is enabled, supports mlu220, mlu270, mlu270-x5k, mlu290 and mlu370
```

## Upgrade Notice

**Please see [changelog](CHANGELOG.md) for deprecation and breaking changes.**

[release v1.1.3]: https://github.com/Cambricon/cambricon-k8s-device-plugin/releases/tag/v1.1.3
