# Changelog

## v2.0.16

- Bump `cndev` to version 3.14.2
- Bump `cntopo` to 1.7.2

## v2.0.15

- Support to deploy with helm
- Add handle when get ring timeout
- Bump `cndev` to version 3.13.0

## v2.0.14

- Support retrying when initial allocation is insufficient
- Remove xid check
- Bump `cndev` to version 3.12.3

## v2.0.13

- Bump cndev to 3.12.1

## v2.0.12

- Bump cndev to 3.11.0
- Upgrade dependence to cntopo 1.4.0
- Support numa topology

## v2.0.11

- Support cambricon_gdr
- Remove SR_IOV、mlu-share

## v2.0.10

- Support read configuration from file
- Remove device remapping

## v2.0.9

- Add host ipc and pid config
- Support dsmlu restore
- Bump cndev to 3.9.0
- Rename dynamic-mim

## v2.0.8

- Support dynamic smlu
- Support xid errors check in health check
- Add dynamic smlu readme
- Support print out version

## v2.0.7

- Support use new cndev api to get mim devpath
- Support smlu2.0

## v2.0.6

- Support mlu type in all virtual mode

## v2.0.5

- Support to report topology in topologyaware mode
- Support to report real mlu counts to kubelet
- support multi cards per container in env-share mode
- Replace ioutil with os package
- Upgrade dependence to cndev 3.4.2

## v2.0.4

- Add log level config
- Upgrade dependence to cndev 3.4.1
- Bump go to 1.19 and baseimage to ubuntu:20.04

## v2.0.3

- Add liveness probe
- Support mlu share for volcano scheduler
- Support cntopo lib

## v2.0.2

- Upgrade dependence to cntopo 0.7.0

## v2.0.1

- Upgrade dependence to cndev 3.0.1
- Add MountRPMsg in flags

## v2.0.0

- Upgrade dependence to cndev 3.0.0
- **Not support SR-IOV since cndev 3.0.0**

## v1.3.6

- Bump cntoolkit to 2.7.0

## v1.3.5

- Use cntopo to implement topology-aware mode
- Turn device to healthy when it recovers from unhealthy
- Refactor cndev dl implementation from c to go
- Bump cntoolkit to 2.6.0

## v1.3.4

- Add mlu-share mode
- Refactor code to mlu pkg

## v1.3.3

- Fix uuid \x00 suffix
- Support cncodec device dynamic mount

## v1.3.2

- Support new devices

## v1.3.1

- Support allocating MLU370 ipcm devices

## v1.3.0

**BREAKING CHANGE**: MLU driver must be equal or above 4.15.2

- Remove MLU100 code
- Support mlulink topology awareness
- Support device registration with MLU type
- Upgrade dependence to cntoolkit 2.2.0
- Support mounting host cnmon to pod(s) consuming MLU(s)
- Get MLU uuid from cndev instead of using MLU sn
- Refactor mock test
- Use cndev to get mlulink port number

## v1.1.3

- Allow changing sriov num by yaml after vf num has been set

## v1.1.2

- Support allocating UART console device(/dev/ttyMS) in container.

## v1.1.1

- Fix SR-IOV vf device name same as env-share fake device name.
- Support building image on arm64 machines.

## v1.1.0

- Implement SR-IOV and env-share features.

## v1.0.0

- Implement basic function.
