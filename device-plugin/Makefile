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
export GOOS := $(word 1, $(subst /, ,$(TARGETPLATFORM)))
export GOARCH := $(word 2, $(subst /, ,$(TARGETPLATFORM)))
export CGO_ENABLED := 1
ifeq ($(GOARCH), arm64)
export CC=aarch64-linux-gnu-gcc
endif

lint:
	golangci-lint run -v

build:
	go build -trimpath -ldflags="-s -w" -o k8s-device-plugin .

test: go-test mock-test

go-test: pkg/cndev/mock/libcndev.so
	LD_LIBRARY_PATH=$(CURDIR)/pkg/cndev/mock \
		MOCK_JSON=$(CURDIR)/test/mockmlu270.json \
		go test -v ./...

integration-test: pkg/cndev/mock/libcndev.so
	./test/device-plugin-integration-test.sh

LIBCNDEV_MOCK_DEPS := $(wildcard pkg/cndev/mock/*.h pkg/cndev/mock/*.c pkg/cndev/include/*)
pkg/cndev/mock/libcndev.so: $(LIBCNDEV_MOCK_DEPS)
	$(CC) -fPIC -shared pkg/cndev/mock/cJSON.c pkg/cndev/mock/cndev.c -lm -o $@

pkg/cndev/mock/mock_test: pkg/cndev/mock/main.c pkg/cndev/mock/libcndev.so
	$(CC) pkg/cndev/mock/main.c \
		-Wl,-rpath='$$ORIGIN' -Lpkg/cndev/mock -lcndev \
		-o pkg/cndev/mock/mock_test

mock-test: pkg/cndev/mock/mock_test
	MOCK_JSON=$(CURDIR)/test/mockmlu270.json ./pkg/cndev/mock/mock_test

addlicense:
	# install with `go get github.com/google/addlicense`
	addlicense -c 'Cambricon, Inc.' -l apache -v .

clean:
	rm -f pkg/cndev/mock/libcndev.so
	rm -f pkg/cndev/mock/mock_test
	rm -f k8s-device-plugin
