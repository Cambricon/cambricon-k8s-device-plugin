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

usage() {
	echo "Usage:"
	echo "$0 TAG"
	exit 0
}

tag=$1
[[ $tag == "" ]] && usage

rm -rf image
mkdir -p image
TAG=$tag ./build_image.sh
sed "s|v[0-9]\.[0-9].[0-9]|$tag|g" examples/cambricon-device-plugin-daemonset.yaml >image/cambricon-device-plugin-daemonset.yaml
sed "s|v[0-9]\.[0-9].[0-9]|$tag|g" examples/cambricon-device-plugin-static-pod.yaml >image/cambricon-device-plugin-static-pod.yaml
cp examples/pod.yaml image/

tar -zcvf cambricon_k8s_device_plugin_packages_"$tag".tar.gz image
