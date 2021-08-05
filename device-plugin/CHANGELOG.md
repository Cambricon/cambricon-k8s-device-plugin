# Changelog

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
