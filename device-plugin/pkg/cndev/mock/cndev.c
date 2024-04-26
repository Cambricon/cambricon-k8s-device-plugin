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

#include "../include/cndev.h"
#include "cJSON.h"
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

cJSON *readJsonFile() {
	FILE *f;
	long len;
	char *content;
	cJSON *json;
	f = fopen(getenv("MOCK_JSON"), "rb");
	fseek(f, 0, SEEK_END);
	len = ftell(f);
	fseek(f, 0, SEEK_SET);
	content = (char *)malloc(len + 1);
	fread(content, 1, len, f);
	fclose(f);
	json = cJSON_Parse(content);
	if (!json) {
		printf("Error before: [%s]\n", cJSON_GetErrorPtr());
	}
	return json;
}

cndevRet_t cndevGetDeviceCount(cndevCardInfo_t *cardNum) {
	cJSON *config;
	unsigned numMLU;
	config = readJsonFile();
	cJSON *numObj = cJSON_GetObjectItem(config, "num");
	numMLU = numObj->valueint;
	cardNum->number = numMLU;

	cJSON_Delete(config);
	return CNDEV_SUCCESS;
}

cndevRet_t cndevInit(int reserved) { return CNDEV_SUCCESS; }

cndevRet_t cndevGetDeviceHandleByIndex(int index, cndevDevice_t *handle) {
	*handle = index;
	return CNDEV_SUCCESS;
}

cndevRet_t cndevGetCardHealthState(cndevCardHealthState_t *cardHealthState,
				   cndevDevice_t device) {
	cJSON *config;
	config = readJsonFile();

	cJSON *health_node = cJSON_GetObjectItem(config, "health");
	cardHealthState->health =
		cJSON_GetArrayItem(health_node, device)->valueint;

	cJSON_Delete(config);
	return CNDEV_SUCCESS;
}

cndevRet_t cndevGetCardSN(cndevCardSN_t *cardSN, cndevDevice_t device) {
	cJSON *config;
	config = readJsonFile();

	cJSON *mb_sn_node = cJSON_GetObjectItem(config, "motherboard");
	cardSN->motherBoardSn = cJSON_GetArrayItem(mb_sn_node, device)->valueint;

	cJSON_Delete(config);
	return CNDEV_SUCCESS;
}

cndevRet_t cndevRelease() { return CNDEV_SUCCESS; }

cndevRet_t cndevGetCardName(cndevCardName_t *cardName, cndevDevice_t device) {
	cJSON *config;
	config = readJsonFile();

	cJSON *card_type_node = cJSON_GetObjectItem(config, "type");
	int card_type = cJSON_GetArrayItem(card_type_node, device)->valueint;

	if (card_type == 0) {
		cardName->id = MLU100;
	} else if (card_type == 1) {
		cardName->id = MLU270;
	} else if (card_type == 16) {
		cardName->id = MLU220_M2;
	} else if (card_type == 17) {
		cardName->id = MLU220_EDGE;
	} else if (card_type == 18) {
		cardName->id = MLU220_EVB;
	} else if (card_type == 19) {
		cardName->id = MLU220_M2i;
	} else if (card_type == 20) {
		cardName->id = MLU290;
	} else if (card_type == 23) {
		cardName->id = MLU370;
	} else if (card_type == 26) {
		cardName->id = MLU590;
	}

	cJSON_Delete(config);
	return CNDEV_SUCCESS;
}

const char *cndevGetCardNameStringByDevId(cndevDevice_t device) {
	cJSON *config;
	config = readJsonFile();

	cJSON *card_type_array = cJSON_GetObjectItem(config, "type");
	int card_type = cJSON_GetArrayItem(card_type_array, device)->valueint;

	cJSON_Delete(config);

	if (card_type == 0) {
		return "MLU100";
	} else if (card_type == 1) {
		return "MLU270";
	} else if (card_type == 16 || card_type == 17 || card_type == 18 ||	card_type == 19) {
		return "MLU220";
	} else if (card_type == 20) {
		return "MLU290";
	} else if (card_type == 23) {
		return "MLU370";
	} else if (card_type == 26) {
		return "MLU590";
	}
}

cndevRet_t cndevGetUUID(cndevUUID_t *uuidInfo, cndevDevice_t device) {
	cJSON *config;
	config = readJsonFile();

	cJSON *uuid_info = cJSON_GetObjectItem(config, "uuid");
	cJSON *uuid = cJSON_GetArrayItem(uuid_info, device);
	for (int i = 0; i < UUID_SIZE; ++i) {
		uuidInfo->uuid[i] = cJSON_GetArrayItem(uuid, i)->valueint;
	}
	cJSON_Delete(config);
	return CNDEV_SUCCESS;
}

const char *cndevGetErrorString(cndevRet_t errorId) {
	return "mock return value of cndev get error string";
}

cndevRet_t cndevGetPCIeInfo(cndevPCIeInfo_t *deviceInfo, cndevDevice_t device) {
	cJSON *config;
	config = readJsonFile();

	cJSON *pcie_info = cJSON_GetObjectItem(config, "pcie_info");
	cJSON *pcie_node = cJSON_GetArrayItem(pcie_info, device);

	deviceInfo->domain = cJSON_GetArrayItem(pcie_node, 0)->valueint;
	deviceInfo->bus = cJSON_GetArrayItem(pcie_node, 1)->valueint;
	deviceInfo->device = cJSON_GetArrayItem(pcie_node, 2)->valueint;
	deviceInfo->function = cJSON_GetArrayItem(pcie_node, 3)->valueint;

	cJSON_Delete(config);

	return CNDEV_SUCCESS;
}

cndevRet_t cndevGetMemoryUsage(cndevMemoryInfo_t *memInfo, cndevDevice_t device) {
	cJSON *config;
	__int64_t memory;
	config = readJsonFile();

	cJSON *memoryObj = cJSON_GetObjectItem(config, "memory");
	memory = memoryObj->valueint;
	memInfo->physicalMemoryTotal = memory;

	cJSON_Delete(config);
	return CNDEV_SUCCESS;
}

cndevRet_t cndevGetMLULinkRemoteInfo(cndevMLULinkRemoteInfo_t *remoteinfo,
				     cndevDevice_t device, int link) {
	cJSON *config;
	config = readJsonFile();

	cJSON *remote_info = cJSON_GetObjectItem(config, "remote_info");
	cJSON *info_array = cJSON_GetArrayItem(remote_info, device);
	cJSON *link_info = cJSON_GetArrayItem(info_array, link);

	for (int i = 0; i < UUID_SIZE; ++i) {
		remoteinfo->uuid[i] = cJSON_GetArrayItem(link_info, i)->valueint;
	}
	cJSON_Delete(config);
	return CNDEV_SUCCESS;
}

cndevRet_t cndevGetMLULinkStatus(cndevMLULinkStatus_t *status, cndevDevice_t device,
				 int link) {
	cJSON *config;
	config = readJsonFile();

	cJSON *mlulink_status_array =
	    cJSON_GetObjectItem(config, "mlulink_status");
	cJSON *dev_info = cJSON_GetArrayItem(mlulink_status_array, device);
	status->isActive = cJSON_GetArrayItem(dev_info, link)->valueint;
	cJSON_Delete(config);
	return CNDEV_SUCCESS;
}

int cndevGetMLULinkPortNumber(cndevDevice_t device) {
	cJSON *config;
	config = readJsonFile();

	return cJSON_GetObjectItem(config, "mlulink_port")->valueint;
}

cndevRet_t cndevGetMimMode(cndevMimMode_t *mode, cndevDevice_t device) {
	mode->mimMode = CNDEV_FEATURE_DISABLED;
	if (device == 0 || device == 1) {
		mode->mimMode = CNDEV_FEATURE_ENABLED;
	}
	return CNDEV_SUCCESS;
}

cndevRet_t cndevGetSMLUMode(cndevSMLUMode_t *mode, cndevDevice_t device) {
	mode->smluMode = CNDEV_FEATURE_DISABLED;
	if (device == 0 || device == 1) {
		mode->smluMode = CNDEV_FEATURE_ENABLED;
	}
	return CNDEV_SUCCESS;
}

cndevRet_t cndevGetAllMluInstanceInfo(int *count, cndevMluInstanceInfo_t *miInfo, cndevDevice_t device) {
	cJSON *config;
	config = readJsonFile();

	cJSON *item = cJSON_GetObjectItem(config, "mim");
	cJSON *subitem = cJSON_GetArrayItem(item, device);
	cJSON *uuids = cJSON_GetObjectItem(subitem, "uuid");
	cJSON *profileNames = cJSON_GetObjectItem(subitem, "profileName");
	cJSON *devNodeNames = cJSON_GetObjectItem(subitem, "devNodeName");
	cJSON *ipcmDevNodeNames = cJSON_GetObjectItem(subitem, "ipcmDevNodeName");
	cJSON *instanceIDs = cJSON_GetObjectItem(subitem, "instance_id");
	int n = cJSON_GetArraySize(profileNames);
	for (int i = 0; i < n; i++) {
		cJSON *uuid = cJSON_GetArrayItem(uuids, i);
		cJSON *profileName = cJSON_GetArrayItem(profileNames, i);
		cJSON *devNodeName = cJSON_GetArrayItem(devNodeNames, i);
		cJSON *ipcmDevNodeName = cJSON_GetArrayItem(ipcmDevNodeNames, i);
		(miInfo+i)->instanceId = cJSON_GetArrayItem(instanceIDs, i)->valueint;
		for (int j = 0; j < UUID_SIZE; ++j) {
			(miInfo+i)->uuid[j] = cJSON_GetArrayItem(uuid, j)->valueint;
		}
		for (int k = 0; k < sizeof(*profileName); k++) {
			(miInfo+i)->profileName[k] = (cJSON_GetStringValue(profileName))[k];
		}
		for (int l = 0; l < sizeof(*devNodeName); l++) {
			(miInfo+i)->devNodeName[l] = (cJSON_GetStringValue(devNodeName))[l];
		}
		for (int m = 0; m < sizeof(*ipcmDevNodeName); m++) {
			(miInfo+i)->ipcmDevNodeName[m] = (cJSON_GetStringValue(ipcmDevNodeName))[m];
		}
	}
	cJSON_Delete(config);
	*count = n;
	return CNDEV_SUCCESS;
}

cndevRet_t cndevGetAllSMluInstanceInfo(int *count, cndevSMluInfo_t *smluInfo, cndevDevice_t device) {
	cJSON *config;
	config = readJsonFile();

	cJSON *item = cJSON_GetObjectItem(config, "smlu");
	cJSON *subitem = cJSON_GetArrayItem(item, device);
	cJSON *uuids = cJSON_GetObjectItem(subitem, "uuids");
	cJSON *profileNames = cJSON_GetObjectItem(subitem, "profileNames");
	cJSON *devNodeNames = cJSON_GetObjectItem(subitem, "devNodeNames");
	cJSON *profileIds = cJSON_GetObjectItem(subitem, "profileIds");
	cJSON *instanceIds = cJSON_GetObjectItem(subitem, "instanceIds");

	int n = cJSON_GetArraySize(profileNames);
	for (int i = 0; i < n; i++) {
		cJSON *uuid = cJSON_GetArrayItem(uuids, i);
		cJSON *profileName = cJSON_GetArrayItem(profileNames, i);
		cJSON *devNodeName = cJSON_GetArrayItem(devNodeNames, i);
		(smluInfo+i)->profileId = cJSON_GetArrayItem(profileIds, i)->valueint;
		(smluInfo+i)->instanceId = cJSON_GetArrayItem(instanceIds, i)->valueint;
		for (int j = 0; j < UUID_SIZE; ++j) {
			(smluInfo+i)->uuid[j] = cJSON_GetArrayItem(uuid, j)->valueint;
		}
		for (int k = 0; k < sizeof(*profileName); k++) {
			(smluInfo+i)->profileName[k] = (cJSON_GetStringValue(profileName))[k];
		}
		for (int l = 0; l < sizeof(*devNodeName); l++) {
			(smluInfo+i)->devNodeName[l] = (cJSON_GetStringValue(devNodeName))[l];
		}
	}
	cJSON_Delete(config);
	*count = n;
	return CNDEV_SUCCESS;
}

cndevRet_t cndevCreateSMluProfileInfo(cndevSMluSet_t *profileInfo, int *profileId, cndevDevice_t device) {
	*profileId = 0;
	return CNDEV_SUCCESS;
}

cndevRet_t cndevCreateSMluInstanceByProfileId(cndevMluInstance_t *miHandle, unsigned int profileId, cndevDevice_t device, char * name) {
	*miHandle = 256;
	return CNDEV_SUCCESS;
}

cndevRet_t cndevGetSMluProfileIdInfo(cndevSMluProfileIdInfo_t *profileID, cndevDevice_t device) {
	profileID->count = 1;
	profileID->profileId[0] = 0;
	return CNDEV_SUCCESS;
}

cndevRet_t cndevGetSMluProfileInfo(cndevSMluProfileInfo_t *profileInfo, int profile, cndevDevice_t device) {
	cJSON *config;
	config = readJsonFile();

	cJSON *item = cJSON_GetObjectItem(config, "smlu");
	cJSON *subitem = cJSON_GetArrayItem(item, 0);
	cJSON *profileName = cJSON_GetObjectItem(subitem, "profileName");
	for (int k = 0; k < sizeof(*profileName); k++) {
		profileInfo->name[k] = (cJSON_GetStringValue(profileName))[k];
	}
	profileInfo->profileId = cJSON_GetObjectItem(subitem, "profileId")->valueint;
	profileInfo->totalCapacity = cJSON_GetObjectItem(subitem, "total")->valueint;
	profileInfo->remainCapacity = cJSON_GetObjectItem(subitem, "remain")->valueint;
	profileInfo->memorySize[CNDEV_SMLU_MAX] = cJSON_GetObjectItem(subitem, "memorySize")->valueint;
	profileInfo->mluQuota[CNDEV_SMLU_MAX] = cJSON_GetObjectItem(subitem, "mluQuota")->valueint;
	cJSON_Delete(config);
	return CNDEV_SUCCESS;
}

cndevRet_t cndevGetSMluInstanceInfo(cndevSMluInfo_t *smluInfo, cndevMluInstance_t miHandle) {
	cJSON *config;
	config = readJsonFile();

	cJSON *item = cJSON_GetObjectItem(config, "smlu");
	cJSON *subitem = cJSON_GetArrayItem(item, 0);
	cJSON *uuids = cJSON_GetObjectItem(subitem, "uuids");
	cJSON *profileNames = cJSON_GetObjectItem(subitem, "profileNames");
	cJSON *devNodeNames = cJSON_GetObjectItem(subitem, "devNodeNames");
	cJSON *instanceIds = cJSON_GetObjectItem(subitem, "instanceIds");
	cJSON *uuid = cJSON_GetArrayItem(uuids, 0);
	cJSON *profileName = cJSON_GetArrayItem(profileNames, 0);
	cJSON *devNodeName = cJSON_GetArrayItem(devNodeNames, 0);
	smluInfo->instanceId = cJSON_GetArrayItem(instanceIds, 0)->valueint;
	for (int j = 0; j < UUID_SIZE; ++j) {
		smluInfo->uuid[j] = cJSON_GetArrayItem(uuid, j)->valueint;
	}
	for (int k = 0; k < sizeof(*profileName); k++) {
		smluInfo->profileName[k] = (cJSON_GetStringValue(profileName))[k];
	}
	for (int l = 0; l < sizeof(*devNodeName); l++) {
		smluInfo->devNodeName[l] = (cJSON_GetStringValue(devNodeName))[l];
	}
	cJSON_Delete(config);
	return CNDEV_SUCCESS;
}

cndevRet_t cndevDestroySMluInstanceByHandle(cndevMluInstance_t miHandle) {
	return CNDEV_SUCCESS;
}

cndevRet_t cndevDestroySMluProfileInfo(int profileId, cndevDevice_t device) {
	return CNDEV_SUCCESS;
}

cndevRet_t cndevGetXidErrorV2(cndevXidErrorV2_t *xidErr, cndevDevice_t device) {
	if (device != 0) {
		return CNDEV_SUCCESS;
	}

	cJSON *config;
	config = readJsonFile();

	cJSON *xidErrors = cJSON_GetObjectItem(config, "xidErrors");
	int n = cJSON_GetArraySize(xidErrors);
	for (int i = 0; i < n; i++) {
		xidErr->xidCount[i] = cJSON_GetArrayItem(xidErrors, i)->valueint;
	}
	cJSON_Delete(config);
	return CNDEV_SUCCESS;
}

const char *cndevGetXidErrorString(cndevXidEnum_t xid) {
	switch(xid) {
		case CNDEV_XID_MCU_ERROR:
			return "CNDEV_XID_MCU_ERROR";
		case CNDEV_XID_RPC_ERROR:
			return "CNDEV_XID_RPC_ERROR";
		default:
			return "unknown";
	}
}
