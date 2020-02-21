#!/bin/bash

curpath=$(cd "$(dirname "$0")" || exit; pwd)

if [ ! -d "$curpath/image" ]; then
  mkdir -p  "$curpath/image"
else
  rm -rf "$curpath/image/*"
fi

cndev_path="/usr/local/neuware/lib64/libcndev.so"
if [ ! -f $cndev_path ]; then
        echo "Dependency check failed, please install Cambricon neauware!"
        exit 1
else
        echo "Dependency check passed."
fi

cp $cndev_path "$curpath/."

echo "Building Cambricon device plugin docker image."
docker build -t  cambricon-k8s-device-plugin:v1.0.0  -f Dockerfile  .

echo "Saving Cambricon device plugin docker image."
docker save -o "$curpath/image/cambricon_device_plugin_ubuntu.tar cambricon-k8s-device-plugin:v1.0.0"

rm -rf "$curpath/libcndev.so"
