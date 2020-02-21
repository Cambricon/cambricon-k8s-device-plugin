/*************************************************************************
 * Copyright (C) [2019] by Cambricon, Inc. All rights reserved
 *
 *  Licensed under the Apache License, Version 2.0 (the "License");
 *  you may not use this file except in compliance with the License.
 *  You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS
 * OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL
 * THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 *************************************************************************/

package cndev

// #cgo CFLAGS: -I ./
// #cgo LDFLAGS: -ldl -Wl,--unresolved-symbols=ignore-in-object-files
// #include "cndev_dl.h"
import "C"

import (
	"errors"
	"fmt"
	"github.com/google/uuid"
	"time"
)

type handle struct {
	UUID  string
	MINOR uint
	PATH  string
}

func errorString(ret C.cndevRet_t) error {
	if ret == C.CNDEV_SUCCESS {
		return nil
	}
	err := C.GoString(C.cndevGetErrorString(ret))
	return fmt.Errorf("cndev: %v", err)
}

func init_() error {
	r := C.cndevInit_dl()
	if r == C.CNDEV_ERROR_UNINITIALIZED {
		return errors.New("could not load CNDEV library")
	}
	return errorString(r)
}

func cndevInit() error {
	return init_()
}

func release_() error {
	r := C.cndevRelease_dl()
	return errorString(r)
}

func Release() error {
	return release_()
}

func deviceGetCount() (uint, error) {
	var card_infos C.cndevCardInfo_t
	card_infos.version = C.int(1)
	r := C.cndevGetDeviceCount(&card_infos)
	return uint(card_infos.Number), errorString(r)
}

func deviceGetCardName(devId uint) (C.cndevNameEnum_t, error) {
	var cardName C.cndevCardName_t
	cardName.version = C.int(3)
	r := C.cndevGetCardName(&cardName, C.int(devId))
	cardType := cardName.id
	return cardType, errorString(r)
}

func deviceGetCardSN(devId uint) (C.cndevCardSN_t, error) {
	var cardSN C.cndevCardSN_t

	cardSN.version = C.int(3)
	r := C.cndevGetCardSN(&cardSN, C.int(devId))

	return cardSN, errorString(r)
}

func deviceGetHandleByIndex(idx_uint uint) (handle, error) {
	var h handle
	var path string

	cardSN, err := deviceGetCardSN(idx_uint)
	if err != nil {
		return h, err
	}

	cardUUID := fmt.Sprintf("%x", int(cardSN.sn))

	// Type A or type B card has no SN code, fake one.
	if cardUUID == "0" {
		cardUUID = uuid.New().String()
	}
	cardUUID = "MLU-" + cardUUID

	cardName, err := deviceGetCardName(idx_uint)

	if err != nil {
		return h, err
	}

	if cardName == C.MLU100 {
		path = fmt.Sprintf("/dev/cambricon_c10Dev%d", idx_uint)
	} else {
		path = fmt.Sprintf("/dev/cambricon_dev%d", idx_uint)
	}

	h = handle{
		UUID:  cardUUID,
		MINOR: idx_uint,
		PATH:  path,
	}

	return h, nil
}

func (h handle) deviceGetUUID() (string, error) {
	var ret C.cndevRet_t = C.CNDEV_SUCCESS
	return h.UUID, errorString(ret)
}

func (h handle) deviceGetPath() (string, error) {
	var ret C.cndevRet_t = C.CNDEV_SUCCESS
	return h.PATH, errorString(ret)
}

func (h handle) deviceHealthCheckState(delayTime int) (int, error) {
	var ret C.cndevRet_t
	var cardHealthState C.cndevCardHealthState_t
	var healthCode int
	cardHealthState.version = C.int(3)
	devId := C.int(h.MINOR)
	// sleep some seconds
	time.Sleep(time.Duration(delayTime) * time.Second)
	ret = C.cndevGetCardHealthState(&cardHealthState, devId)
	healthCode = int(cardHealthState.health)
	return healthCode, errorString(ret)
}
