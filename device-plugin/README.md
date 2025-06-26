# Cambricon Device Plugin for Kubernetes

## About Cambricon Device Plugin

The Cambricon device plugin for Kubernetes is a Daemonset which allows you to automatically:

- Report the quantity of MLU on each nodes of your cluster.
- Monitor the health status of MLUs.
- Be capable to run containers with MLU enabled.

This repository contains Cambricon's official implementation of the Kubernetes device plugin.

## Prerequisites

The prerequisites for running the Cambricon Device Plugin:

- Support MLU3xx devices
- For MLU 3xx needs driver >= 4.20.9
- For MLU 3xx needs cntoolkit >= 2.8.2 on your building machine
- For SMLU needs driver >= 5.10.27 and cntoolkit >= 3.9.0

For MLU driver version before 4.9.13, and need to support MLU2xx please use [release v1.1.3].

For Kubernetes version < 1.19.0, mlulink topology-aware mode can not be used. If you want to use this feature, make sure your Kubernetes version >= 1.19.0.

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
| CNTOPO     | absolute path of the cntopo binary, neuware installation path by default      |

Docker should be >= 17.05.0 on your building machine. If you want to cross build, make sure docker version >= 19.03.

For amd64:

```shell
GOPROXY=https://goproxy.cn ./build_image.sh
```

For arm64:

```shell
ARCH=arm64 GOPROXY=https://goproxy.cn ./build_image.sh
```

Please make sure Cambricon neuware and cncl is installed in your compiling environment.
It uses **libcndev.so** and **cntopo** binary on your compiling machine and generates docker image in folder `./image`.

### Enabling MLU Support in Kubernetes

1. Push the docker image to the docker repo of your cluster or load the docker image on all your MLU nodes by:

   ```shell
   docker load -i image/cambricon-device-plugin-amd64.tar
   ```

2. Enable MLU support in your cluster by deploying the daemonset in [examples](examples) folder:

   Set the args in the yaml file

   ```yaml
   args:
     - --mode=default #device plugin mode: default, env-share, mim and topology-aware
     - --virtualization-num=1 #  virtualization number for each MLU, used only in env-share mode, set to 110 to support multi cards per container in env-share mode
     - --mlulink-policy=best-effort # MLULink topology policy: best-effort, guaranteed or restricted, used only in topology-aware mode
     - --cnmon-path=/usr/bin/cnmon # host machine cnmon path, must be absolute path. comment out this line if use-runtime is enabled
     - --enable-device-type #comment to enable device registration with type info
     #- --use-runtime # enable interaction with cambricon container runtime to complete device mounting
     #- --enable-console #uncomment to enable UART console device(/dev/ttyMS) in container
     #- --disable-health-check #uncomment to disable health check
     #- --mount-rpmsg #uncomment to mount RPMsg directory, will be deprecated in the near future
   ```

   supported features:

   - default: default mode
   - env-share: a whole card can be allocated into multiple containers, should set `virtualization-num` as maximum number of containers one MLU can be allocated into.
   - mim: supports `mim` mode, need to preset `mim` in the host before device plugin starting
   - topology-aware: device plugin is aware of MLULink topology and tries to allocate MLUs forming a ring. Set `mlulink-policy` as described below

   MLULink topology policies, guaranteed and restricted only works for 1,2,4,8 requested MLUs:

   - best-effort: allocate devices forming maximum number of rings whenever possible
   - guaranteed: allocated devices must form at least one ring, otherwise return error
   - restricted: allocated devices must form the best rings, otherwise return error

   ```shell
   kubectl create -f examples/cambricon-device-plugin-daemonset.yaml
   ```

   (Optional) If you do not want the daemonset way of deployment, edit the static pod template in examples folder and put the file into your configured static pod folder (`/etc/kubernetes/manifests` by default).

3. Create rbac role and config.

   ```shell
   kubectl create -f examples/cambricon-device-plugin-config.yaml
   ```

### Install with helm

This step is optional, you can also install device plugin via helm

1. Download and install helm firstly, refer <https://helm.sh/docs/intro/install/>

2. Clone this repo, `cd device-plugin/deploys/helm` and run `helm install release-x.x.x device-plugin/`

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
          #cambricon.com/mlu370: "1" #uncomment to use when device type is enabled
          #cambricon.com/mlu370.share: "1" #uncomment to use device with env-share mode
          #cambricon.com/mlu370.mim-2m.8gb: "1" #uncomment to use device with mim mode
```

### Change log level

Support to change log level and validate without restarting

change to debug

```shell
curl -i http://{{device-plugin-pod-ip}}:30107/logLevel?level=debug
```

change to info

```shell
curl -i http://{{device-plugin-pod-ip}}:30107/logLevel?level=info
```

## Upgrade Notice

**Please see [changelog](CHANGELOG.md) for deprecation and breaking changes.**

[release v1.1.3]: https://github.com/Cambricon/cambricon-k8s-device-plugin/releases/tag/v1.1.3
