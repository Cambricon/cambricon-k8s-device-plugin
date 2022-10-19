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

curpath=$(dirname "$0")
cd "$curpath" || exit 1

: "${TAG:=v1.3.6}"
: "${ARCH:=amd64}"
: "${LIBCNDEV:=/usr/local/neuware/lib64/libcndev.so}"
: "${CNTOPO:=/usr/local/neuware/bin/cntopo}"

case $(awk -F= '/^NAME/{print $2}' /etc/os-release) in
"CentOS Linux")
	BASE_IMAGE=centos:7
	;;
esac

: "${BASE_IMAGE:=ubuntu:18.04}"

echo "Build environ (Can be overridden):"
echo "TAG       = $TAG"
echo "ARCH      = $ARCH"
echo "LIBCNDEV  = $LIBCNDEV"
echo "APT_PROXY = $APT_PROXY"
echo "GOPROXY   = $GOPROXY"
echo "BASE_IMAGE   = $BASE_IMAGE"
echo "CNTOPO = $CNTOPO"

case $(uname -m) in
x86_64)
	build_arch=amd64
	;;
aarch64*)
	build_arch=arm64
	;;
armv8*)
	build_arch=arm64
	;;
esac

rm -rf "$curpath/image"
mkdir -p "$curpath/image"

# Cambricon neuware installation path
if [[ ! -f "$LIBCNDEV" ]]; then
	echo "Can't find libcndev.so at $LIBCNDEV."
	echo "Please install Cambricon neuware, or set LIBCNDEV environ to path of libcndev.so"
	exit 1
fi

if [[ ! -f "$CNTOPO" ]]; then
	echo "Can't find cntopo at $CNTOPO."
	echo "Please install Cambricon cncl, or set CNTOPO environ to path of cntopo"
	exit 1
fi

case $ARCH in
amd64)
	file_arch=x86-64
	;;
arm64)
	file_arch=aarch64
	;;
*)
	echo "Unknown arch $ARCH"
	exit 1
	;;
esac

if ! file "$LIBCNDEV" --dereference | grep -q "$file_arch"; then
	echo "$LIBCNDEV is not for $ARCH"
	exit 1
fi
if ! file "$CNTOPO" --dereference | grep -q "$file_arch"; then
	echo "$CNTOPO is not for $ARCH"
	exit 1
fi

cp "$LIBCNDEV" "$curpath/libs/linux/$ARCH/libcndev.so"
cp "$CNTOPO" "$curpath/libs/linux/$ARCH/cntopo"

echo "Building Cambricon device plugin docker image."

# Legacy build for docker 18.06.
# Remove this when docker is upgraded to 19.03 in all environ.
[[ "$ARCH" == "$build_arch" ]] && docker build -t "cambricon-k8s-device-plugin:$TAG" \
	--build-arg "GOPROXY=$GOPROXY" --build-arg "APT_PROXY=$APT_PROXY" \
	--build-arg "BUILDPLATFORM=linux/$ARCH" \
	--build-arg "BASE_IMAGE=$BASE_IMAGE" \
	--build-arg "TARGETPLATFORM=linux/$ARCH" .

[[ "$ARCH" == "$build_arch" ]] && docker save -o "image/cambricon-k8s-device-plugin-$ARCH.tar" \
	"cambricon-k8s-device-plugin:$TAG"

if [[ "$ARCH" != "$build_arch" && "$(docker version -f '{{ge .Client.Version "19.03"}}')" != "true" ]]; then
	echo "Needs docker 19.03 and above"
	exit 1
fi

[[ "$ARCH" != "$build_arch" ]] && DOCKER_CLI_EXPERIMENTAL=enabled docker buildx build \
	--platform="linux/$ARCH" -t "cambricon-k8s-device-plugin:$TAG" \
	--build-arg "GOPROXY=$GOPROXY" --build-arg "APT_PROXY=$APT_PROXY" \
	--build-arg "BASE_IMAGE=$BASE_IMAGE" \
	--output type=docker,dest="./image/cambricon-k8s-device-plugin-$ARCH.tar" .

echo "Image is saved at ./image/cambricon-k8s-device-plugin-$ARCH.tar"
rm -f "$curpath/libs/linux/$ARCH/libcndev.so"
rm -f "$curpath/libs/linux/$ARCH/cntopo"
