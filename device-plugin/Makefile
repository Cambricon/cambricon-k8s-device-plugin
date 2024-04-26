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

TARGETPLATFORM ?= linux/amd64
VERSION ?= v1.0.0
export GOOS := $(word 1, $(subst /, ,$(TARGETPLATFORM)))
export GOARCH := $(word 2, $(subst /, ,$(TARGETPLATFORM)))
export CGO_ENABLED := 1
ifeq ($(GOARCH), arm64)
export CC=aarch64-linux-gnu-gcc
endif

generate:
	mockgen -package mock -destination pkg/cntopo/mock/cntopo.go -mock_names=Cntopo=Cntopo github.com/Cambricon/cambricon-k8s-device-plugin/device-plugin/pkg/cntopo Cntopo

lint:
	golangci-lint run -v

build:
	go build -trimpath -ldflags="-s -w" -ldflags="-X 'main.version=$(VERSION)'" -o k8s-device-plugin cmd/main.go

test: go-test mock-test cntopo-mock-test

go-test: pkg/cndev/mock/libcndev.so
	LD_LIBRARY_PATH=$(CURDIR)/pkg/cndev/mock \
		MOCK_JSON=$(CURDIR)/test/mock.json \
		go test -v ./...

integration-test: pkg/cndev/mock/libcndev.so pkg/cntopo/test/libcntopo.so
	./test/device-plugin-integration-test.sh

LIBCNDEV_MOCK_DEPS := $(wildcard pkg/cndev/mock/*.h pkg/cndev/mock/*.c pkg/cndev/include/*)
pkg/cndev/mock/libcndev.so: $(LIBCNDEV_MOCK_DEPS)
	$(CC) -g -fPIC -shared pkg/cndev/mock/cJSON.c pkg/cndev/mock/cndev.c -lm -o $@

pkg/cndev/mock/mock_test: pkg/cndev/mock/main.c pkg/cndev/mock/libcndev.so
	$(CC) -g pkg/cndev/mock/main.c \
		-Wl,-rpath='$$ORIGIN' -Lpkg/cndev/mock -lcndev \
		-o pkg/cndev/mock/mock_test

mock-test: pkg/cndev/mock/mock_test
	MOCK_JSON=$(CURDIR)/test/mock.json ./pkg/cndev/mock/mock_test

LIBCNTOPO_MOCK_DEPS := $(wildcard pkg/cntopo/test/*.h pkg/cntopo/test/*.c pkg/cntopo/include/*)
pkg/cntopo/test/libcntopo.so: $(LIBCNTOPO_MOCK_DEPS)
	$(CC) -g -fPIC -shared pkg/cntopo/test/cJSON.c pkg/cntopo/test/cntopo.c -lm -o $@

pkg/cntopo/test/mock_test: pkg/cntopo/test/main.c pkg/cntopo/test/libcntopo.so
	$(CC) -g pkg/cntopo/test/main.c \
		-Wl,-rpath='$$ORIGIN' -Lpkg/cntopo/test -lcntopo \
		-o pkg/cntopo/test/mock_test

cntopo-mock-test: pkg/cntopo/test/mock_test
	MOCK_JSON=$(CURDIR)/test/mock.json ./pkg/cntopo/test/mock_test

addlicense:
	# install with `go get github.com/google/addlicense`
	addlicense -c 'Cambricon, Inc.' -l apache -v .

clean:
	rm -f pkg/cndev/mock/libcndev.so
	rm -f pkg/cntopo/test/libcntopo.so
	rm -f pkg/cndev/mock/mock_test
	rm -f pkg/cntopo/test/mock_test
	rm -f k8s-device-plugin
