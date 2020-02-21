# Cambricon Device Plugin for Kubernetes

## About Cambricon Device Plugin

The Cambricon device plugin for Kubernetes is a Daemonset which allows you to automatically:

- Report the quantity of MLU on each nodes of your cluster.
- Monitor the health status of MLUs.
- Be capable to run containers with MLU enabled.

This repository contains Cambricon's official implementation of the Kubernetes device plugin.

## Prerequisites

The list of prerequisites for running the Cambricon device plugin is described below:

- MLU100 and MLU270 supported
- MLU100 driver > 3.5; MLU270 driver >2.2.0
- libcndev.so >= V1.8.0
- Kubernetes >= v1.11.2

## Quick Start

### Preparing your MLU Nodes

It assumes that the Cambricon drivers and neauware are installed on your MLU Nodes.

### Download and build

```shell
git clone github.com/cambricon/cambricon-k8s-device-plugin.git
cd cambricon-k8s-device-plugin
git checkout v1.0.0
bash build_image.sh
```

Please make sure Cambricon neauware is installed in your compiling environment.
It uses libcndev.so binary in your compiling machine and generates cambricon device plugin docker images under folder ./image.

### Enabling MLU Support in Kubernetes

1. Push the docker image to the docker repo of your cluster or load the docker image on all your MLU nodes by:

```shell
docker load -i cambricon_device_plugin_ubuntu.tar
```

2. Enable MLU support in your cluster by deploying the following Daemonset:

```shell
kubectl create -f cambricon-device-plugin.yml
```

### Running MLU Jobs

Cambricon MLUs can now be consumed via container level resource requirements using the resource name cambricon.com/mlu:

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

## Changelog

### Version v1.0.0

Implement basic function.
