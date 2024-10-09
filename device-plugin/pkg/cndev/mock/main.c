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
#include <stdbool.h>
#include <stdio.h>
#include <string.h>

int Test_cndevGetDeviceCount() {
	cndevCardInfo_t *cardNum;
	cardNum = (cndevCardInfo_t *)malloc(sizeof(cndevCardInfo_t));
	cardNum->version = 5;
	cndevRet_t result;
	result = cndevGetDeviceCount(cardNum);
	printf("=== Test cndevGetDeviceCount ===\nnum:%d\nret:%d\n",
	       cardNum->number, result);
	return cardNum->number;
}
void Test_cndevInit() {
	cndevRet_t result;
	result = cndevInit(0);
	printf("=== Test cndevInit ===\nret: %d\n", result);
}
void Test_cndevGetCardHealthState(int id) {
	cndevCardHealthState_t *cardHealthState;
	cardHealthState =
	    (cndevCardHealthState_t *)malloc(sizeof(cndevCardHealthState_t));
	cardHealthState->version = 5;
	cndevRet_t result;
	result = cndevGetCardHealthState(cardHealthState, id);
	printf("=== Test cndevGetCardHealthState ===\nhealth:%d\nret:%d\n",
	       cardHealthState->health, result);
}
void Test_cndevGetCardSN(int id) {
	cndevCardSN_t *cardSN;
	cardSN = (cndevCardSN_t *)malloc(sizeof(cndevCardSN_t));
	cardSN->version = 5;
	cndevRet_t result;
	result = cndevGetCardSN(cardSN, id);
	printf("=== Test cndevGetCardSN ===\nmotherBoard:%ld\nret:%d\n",
	       cardSN->motherBoardSn, result);
}

void Test_cndevGetPCIeInfo(int id) {
	cndevPCIeInfo_t *cardPcie;
	cardPcie = (cndevPCIeInfo_t *)malloc(sizeof(cndevPCIeInfo_t));
	cardPcie->version = 5;
	cndevRet_t result;
	result = cndevGetPCIeInfo(cardPcie, id);
	printf("=== Test cndevGetPcieInfo "
	       "===\ndomain:%d\nbus:%d\ndevice:%d\nfunction:%d\nret:%d\n",
	       cardPcie->domain, cardPcie->bus, cardPcie->device,
	       cardPcie->function, result);
}

void Test_cndevGetUUID(int id) {
	cndevUUID_t *uuidInfo;
	uuidInfo = (cndevUUID_t *)malloc(sizeof(cndevUUID_t));
	uuidInfo->version = 5;
	cndevRet_t result;
	result = cndevGetUUID(uuidInfo, id);
	printf("=== Test cndevGetUUID ===\nuuid:%s\nret:%d\n", uuidInfo->uuid,
	       result);
}

void Test_cndevGetCardName(int id) {
	cndevCardName_t *cardName;
	cardName = (cndevCardName_t *)malloc(sizeof(cndevCardName_t));
	cardName->version = 5;
	cndevRet_t result;
	result = cndevGetCardName(cardName, id);
	printf("=== Test cndevGetCardName ===\nid:%d\nret:%d\n", cardName->id,
	       result);
}

void Test_cndevGetMemoryUsage(int id) {
	cndevMemoryInfo_t *memInfo;
	memInfo = (cndevMemoryInfo_t *)malloc(sizeof(cndevMemoryInfo_t));
	memInfo->version = 5;
	cndevRet_t result;
	result = cndevGetMemoryUsage(memInfo, id);
	printf("=== Test cndevGetMemoryUsage ===\nid:%d\nmemory:%ld\nret:%d\n",
	       id, memInfo->physicalMemoryTotal, result);
}

void Test_cndevGetMLULinkRemoteInfo(int id) {
	cndevMLULinkRemoteInfo_t *remoteinfo;
	remoteinfo = (cndevMLULinkRemoteInfo_t *)malloc(
	    sizeof(cndevMLULinkRemoteInfo_t));
	remoteinfo->version = 5;
	cndevRet_t result;
	printf("=== Test cndevGetMLULinkRemoteInfo ===\n");
	int num;
	num = cndevGetMLULinkPortNumber(id);
	for (int i = 0; i < num; ++i) {
		result = cndevGetMLULinkRemoteInfo(remoteinfo, id, i);
		printf("port:%d remote uuid:%s, ret:%d\n", i, remoteinfo->uuid,
		       result);
	}
}

void Test_cndevGetMLULinkStatus(int id) {
	cndevMLULinkStatus_t *status;
	status = (cndevMLULinkStatus_t *)malloc(sizeof(cndevMLULinkStatus_t));
	status->version = 5;
	cndevRet_t result;
	printf("=== Test cndevGetMLULinkStatus ===\n");
	int num;
	num = cndevGetMLULinkPortNumber(id);
	for (int i = 0; i < num; ++i) {
		result = cndevGetMLULinkStatus(status, id, i);
		printf("port:%d mlulink status:%d, ret:%d\n", i,
		       status->isActive, result);
	}
}

void Test_cndevGetMLULinkPortNumber(int id) {
	int result;
	result = cndevGetMLULinkPortNumber(id);
	printf("=== Test cndevGetMLULinkPortNumber ===\nret:%d\n", result);
}

void Test_cndevGetMimMode(cndevDevice_t device) {
	cndevMimMode_t mode;
	cndevGetMimMode(&mode, device);
	printf("=== Test cndevGetMimMode ===\nret:%d\n", (&mode)->mimMode);
}

void Test_cndevGetSMLUMode(cndevDevice_t device) {
	cndevSMLUMode_t mode;
	cndevGetSMLUMode(&mode, device);
	printf("=== Test cndevGetSMLUMode ===\nret:%d\n", (&mode)->smluMode);
}

void Test_cndevGetAllMluInstanceInfo(cndevDevice_t device) {
	int count;
	cndevMluInstanceInfo_t *miInfo;
	miInfo = (cndevMluInstanceInfo_t *)malloc(
	    3 * sizeof(cndevMluInstanceInfo_t));
	cndevGetAllMluInstanceInfo(&count, miInfo, device);
	printf("=== Test cndevGetAllMluInstanceInfo ===\ncount:%d\n \
	first uuid:%s, first profileName:%s, first devNodeName:%s, first ipcmDevNodeName:%s, first instanceID:%d\n \
	second uuid:%s, second profileName:%s, second devNodeName:%s, second ipcmDevNodeName:%s, second instanceID:%d\n \
	third uuid:%s, third profileName:%s, third devNodeName:%s, third ipcmDevNodeName:%s, third instanceID:%d\n",
	       count, miInfo->uuid, miInfo->profileName, miInfo->devNodeName,
	       miInfo->ipcmDevNodeName, miInfo->instanceId, (miInfo + 1)->uuid,
	       (miInfo + 1)->profileName, (miInfo + 1)->devNodeName,
	       (miInfo + 1)->ipcmDevNodeName, (miInfo + 1)->instanceId,
	       (miInfo + 2)->uuid, (miInfo + 2)->profileName,
	       (miInfo + 2)->devNodeName, (miInfo + 2)->ipcmDevNodeName,
	       (miInfo + 2)->instanceId);
	free(miInfo);
}

void Test_cndevGetAllSMluInstanceInfo(cndevDevice_t device) {
	int count;
	cndevSMluInfo_t *smluInfo;
	smluInfo = (cndevSMluInfo_t *)malloc(3 * sizeof(cndevSMluInfo_t));
	cndevGetAllSMluInstanceInfo(&count, smluInfo, device);
	printf("=== Test cndevGetAllSMluInstanceInfo ===\ncount:%d\n \
	first uuid:%s, first profileName:%s, first devNodeName:%s first instanceID:%d first profileID:%d\n \
	second uuid:%s, second profileName:%s, second devNodeName:%s second instanceID:%d second profileID:%d\n \
	third uuid:%s, third namprofileNamee:%s, third devNodeName:%s third instanceID:%d third profileID:%d\n",
	       count, smluInfo->uuid, smluInfo->profileName,
	       smluInfo->devNodeName, smluInfo->instanceId, smluInfo->profileId,
	       (smluInfo + 1)->uuid, (smluInfo + 1)->profileName,
	       (smluInfo + 1)->devNodeName, (smluInfo + 1)->instanceId,
	       (smluInfo + 1)->profileId, (smluInfo + 2)->uuid,
	       (smluInfo + 2)->profileName, (smluInfo + 2)->devNodeName,
	       (smluInfo + 2)->instanceId, (smluInfo + 2)->profileId);
	free(smluInfo);
}

void Test_cndevGetSMluProfileInfo(cndevDevice_t device) {
	cndevSMluProfileInfo_t profile_info;
	cndevGetSMluProfileInfo(&profile_info, 0, device);
	printf("=== Test cndevGetSMluProfileInfo ===\n \
	profileID:%d, name:%s, total:%d, remain:%d\n",
	       profile_info.profileId, profile_info.name,
	       profile_info.totalCapacity, profile_info.remainCapacity);
}

void Test_cndevGetSMluInstanceInfo(cndevDevice_t device) {
	cndevSMluInfo_t smluInfo;
	cndevGetSMluInstanceInfo(&smluInfo, device);
	printf("=== Test cndevGetSMluInstanceInfo ===\n \
	uuid:%s, profileName:%s, devNodeName:%s, instanceID:%d\n",
	       smluInfo.uuid, smluInfo.profileName, smluInfo.devNodeName,
	       smluInfo.instanceId);
}

int main() {
	Test_cndevInit();
	int num;
	num = Test_cndevGetDeviceCount();
	for (int i = 0; i < num; ++i) {
		printf("================ Test card id %d =============\n", i);
		cndevDevice_t device;
		cndevRet_t result;
		result = cndevGetDeviceHandleByIndex(i, &device);
		Test_cndevGetCardName(device);
		Test_cndevGetCardHealthState(device);
		Test_cndevGetCardSN(device);
		Test_cndevGetPCIeInfo(device);
		Test_cndevGetUUID(device);
		Test_cndevGetMemoryUsage(device);
		Test_cndevGetMLULinkRemoteInfo(device);
		Test_cndevGetMLULinkStatus(device);
		Test_cndevGetMLULinkPortNumber(device);
		if (i == 0 || i == 1) {
			Test_cndevGetMimMode(device);
			Test_cndevGetAllMluInstanceInfo(device);
			Test_cndevGetSMLUMode(device);
			Test_cndevGetAllSMluInstanceInfo(device);
		}
		if (i == 0) {
			Test_cndevGetSMluProfileInfo(device);
			Test_cndevGetSMluInstanceInfo(device);
		}
	}
	return 0;
}
