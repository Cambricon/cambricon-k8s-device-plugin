/*
 * Copyright 2020 Cambricon, Inc.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include "cndev_dl.h"
#include "./include/cndev.h"
#include <dlfcn.h>
#include <stddef.h>
#include <stdlib.h>

void *cndevHandle;

cndevRet_t (*cndevInitFunc)(int);

cndevRet_t (*cndevReleaseFunc)(void);

const char *(*cndevGetErrorStringFunc)(cndevRet_t errorId);
const char *cndevGetErrorString(cndevRet_t errorId) {
	if (cndevGetErrorStringFunc == NULL) {
		return "cndevGetErrorString Function not found";
	}
	return cndevGetErrorStringFunc(errorId);
}

cndevRet_t (*cndevGetDeviceCountFunc)(cndevCardInfo_t *cardNum);
cndevRet_t cndevGetDeviceCount(cndevCardInfo_t *cardNum) {
	if (cndevGetDeviceCountFunc == NULL) {
		return CNDEV_ERROR_UNKNOWN;
	}
	return cndevGetDeviceCountFunc(cardNum);
}

cndevRet_t (*cndevGetCardSNFunc)(cndevCardSN_t *cardSN, int devId);
cndevRet_t cndevGetCardSN(cndevCardSN_t *cardSN, int devId) {
	if (cndevGetCardSNFunc == NULL) {
		return CNDEV_ERROR_UNKNOWN;
	}
	return cndevGetCardSNFunc(cardSN, devId);
}

cndevRet_t (*cndevGetCardNameFunc)(cndevCardName_t *cardName, int devId);
cndevRet_t cndevGetCardName(cndevCardName_t *cardName, int devId) {
	if (cndevGetCardNameFunc == NULL) {
		return CNDEV_ERROR_UNKNOWN;
	}
	return cndevGetCardNameFunc(cardName, devId);
}

const char *(*getCardNameStringByDevIdFunc)(int devId);
const char *getCardNameStringByDevId(int devId) {
	if (getCardNameStringByDevIdFunc == NULL) {
		return "getCardNameStringByDevId Function not found";
	}
	return getCardNameStringByDevIdFunc(devId);
}

cndevRet_t (*cndevGetCardHealthStateFunc)(
    cndevCardHealthState_t *cardHealthState, int devId);
cndevRet_t cndevGetCardHealthState(cndevCardHealthState_t *cardHealthState,
				   int devId) {
	if (cndevGetCardHealthStateFunc == NULL) {
		return CNDEV_ERROR_UNKNOWN;
	}
	return cndevGetCardHealthStateFunc(cardHealthState, devId);
}

cndevRet_t (*cndevGetMLULinkDevSNFunc)(cndevMLULinkDevSN_t *devinfo, int devId);
cndevRet_t cndevGetMLULinkDevSN(cndevMLULinkDevSN_t *devinfo, int devId) {
	if (cndevGetMLULinkDevSNFunc == NULL) {
		return CNDEV_ERROR_UNKNOWN;
	}
	return cndevGetMLULinkDevSNFunc(devinfo, devId);
}

cndevRet_t (*cndevGetPCIeInfoFunc)(cndevPCIeInfo_t *deviceInfo, int devId);
cndevRet_t cndevGetPCIeInfo(cndevPCIeInfo_t *deviceInfo, int devId) {
	if (cndevGetPCIeInfoFunc == NULL) {
		return CNDEV_ERROR_UNKNOWN;
	}
	return cndevGetPCIeInfoFunc(deviceInfo, devId);
}

cndevRet_t (*cndevGetUUIDFunc)(cndevUUID_t *uuidInfo, int devId);
cndevRet_t cndevGetUUID(cndevUUID_t *uuidInfo, int devId) {
	if (cndevGetUUIDFunc == NULL) {
		return CNDEV_ERROR_UNKNOWN;
	}
	return cndevGetUUIDFunc(uuidInfo, devId);
}

cndevRet_t (*cndevGetMLULinkRemoteInfoFunc)(
    cndevMLULinkRemoteInfo_t *remoteinfo, int devId, int link);
cndevRet_t cndevGetMLULinkRemoteInfo(cndevMLULinkRemoteInfo_t *remoteinfo,
				     int devId, int link) {
	if (cndevGetMLULinkRemoteInfoFunc == NULL) {
		return CNDEV_ERROR_UNKNOWN;
	}
	return cndevGetMLULinkRemoteInfoFunc(remoteinfo, devId, link);
}

cndevRet_t (*cndevGetMLULinkStatusFunc)(cndevMLULinkStatus_t *status, int devId,
					int link);
cndevRet_t cndevGetMLULinkStatus(cndevMLULinkStatus_t *status, int devId,
				 int link) {
	if (cndevGetMLULinkStatusFunc == NULL) {
		return CNDEV_ERROR_UNKNOWN;
	}
	return cndevGetMLULinkStatusFunc(status, devId, link);
}

int (*cndevGetMLULinkPortNumberFunc)(int devId);
int cndevGetMLULinkPortNumber(int devId) {
	if (cndevGetMLULinkPortNumberFunc == NULL) {
		return CNDEV_ERROR_UNKNOWN;
	}
	return cndevGetMLULinkPortNumberFunc(devId);
}

cndevRet_t CNDEV_DL(cndevInit)(void) {
	cndevHandle = dlopen("libcndev.so", RTLD_LAZY);
	if (cndevHandle == NULL) {
		return CNDEV_ERROR_UNINITIALIZED;
	}
	cndevInitFunc = dlsym(cndevHandle, "cndevInit");
	if (cndevInitFunc == NULL) {
		return CNDEV_ERROR_UNKNOWN;
	}
	cndevReleaseFunc = dlsym(cndevHandle, "cndevRelease");
	if (cndevReleaseFunc == NULL) {
		return CNDEV_ERROR_UNKNOWN;
	}
	cndevGetErrorStringFunc = dlsym(cndevHandle, "cndevGetErrorString");
	if (cndevGetErrorStringFunc == NULL) {
		return CNDEV_ERROR_UNKNOWN;
	}
	cndevGetDeviceCountFunc = dlsym(cndevHandle, "cndevGetDeviceCount");
	if (cndevGetDeviceCountFunc == NULL) {
		return CNDEV_ERROR_UNKNOWN;
	}
	cndevGetCardSNFunc = dlsym(cndevHandle, "cndevGetCardSN");
	if (cndevGetCardSNFunc == NULL) {
		return CNDEV_ERROR_UNKNOWN;
	}
	cndevGetUUIDFunc = dlsym(cndevHandle, "cndevGetUUID");
	if (cndevGetUUIDFunc == NULL) {
		return CNDEV_ERROR_UNKNOWN;
	}
	cndevGetMLULinkStatusFunc = dlsym(cndevHandle, "cndevGetMLULinkStatus");
	if (cndevGetMLULinkStatusFunc == NULL) {
		return CNDEV_ERROR_UNKNOWN;
	}
	cndevGetMLULinkRemoteInfoFunc =
	    dlsym(cndevHandle, "cndevGetMLULinkRemoteInfo");
	if (cndevGetMLULinkRemoteInfoFunc == NULL) {
		return CNDEV_ERROR_UNKNOWN;
	}
	cndevGetMLULinkPortNumberFunc =
	    dlsym(cndevHandle, "cndevGetMLULinkPortNumber");
	if (cndevGetMLULinkPortNumberFunc == NULL) {
		return CNDEV_ERROR_UNKNOWN;
	}
	cndevGetCardNameFunc = dlsym(cndevHandle, "cndevGetCardName");
	if (cndevGetCardNameFunc == NULL) {
		return CNDEV_ERROR_UNKNOWN;
	}
	getCardNameStringByDevIdFunc =
	    dlsym(cndevHandle, "getCardNameStringByDevId");
	if (getCardNameStringByDevIdFunc == NULL) {
		return CNDEV_ERROR_UNKNOWN;
	}
	cndevGetCardHealthStateFunc =
	    dlsym(cndevHandle, "cndevGetCardHealthState");
	if (cndevGetCardHealthStateFunc == NULL) {
		return CNDEV_ERROR_UNKNOWN;
	}
	cndevGetMLULinkDevSNFunc = dlsym(cndevHandle, "cndevGetMLULinkDevSN");
	cndevGetPCIeInfoFunc = dlsym(cndevHandle, "cndevGetPCIeInfo");
	cndevRet_t result = cndevInitFunc(0);
	if (result != CNDEV_SUCCESS) {
		dlclose(cndevHandle);
		cndevHandle = NULL;
		return result;
	}
	return CNDEV_SUCCESS;
}

cndevRet_t CNDEV_DL(cndevRelease)(void) {
	if (cndevHandle == NULL) {
		return CNDEV_SUCCESS;
	}
	if (cndevReleaseFunc == NULL) {
		return CNDEV_ERROR_UNKNOWN;
	}
	cndevRet_t r = cndevReleaseFunc();
	if (r != CNDEV_SUCCESS) {
		return r;
	}
	return (dlclose(cndevHandle) ? CNDEV_ERROR_UNKNOWN : CNDEV_SUCCESS);
}
