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

cJSON *readJsonFile()
{
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
	if (!json)
	{
		printf("Error before: [%s]\n", cJSON_GetErrorPtr());
	}
	return json;
}
cndevRet_t cndevGetDeviceCount(cndevCardInfo_t *cardNum)
{
	cJSON *config;
	unsigned numMLU;
	config = readJsonFile();
	cJSON *numObj = cJSON_GetObjectItem(config, "num");
	numMLU = numObj->valueint;
	cardNum->Number = numMLU;
	return CNDEV_SUCCESS;
}
cndevRet_t cndevInit(int reserved)
{
	cJSON *config;
	cndevRet_t result;
	config = readJsonFile();
	cJSON *ret = cJSON_GetObjectItem(config, "ret");
	int result_int = ret->valueint;
	result = CNDEV_SUCCESS;
	for (int i = 0; i <= 9; i++)
	{
		if (i == result_int)
		{
			return result;
		}
		result++;
	}
}
cndevRet_t cndevGetCardHealthState(cndevCardHealthState_t *cardHealthState, int devId)
{
	cJSON *config;
	cndevRet_t result;
	config = readJsonFile();

	cJSON *health_node = cJSON_GetObjectItem(config, "health");
	int health_int = cJSON_GetArrayItem(health_node, devId)->valueint;
	cardHealthState->health = health_int;

	cJSON *ret = cJSON_GetObjectItem(config, "health_ret");
	int result_int = cJSON_GetArrayItem(ret, devId)->valueint;
	result = CNDEV_SUCCESS;
	for (int i = 0; i <= 9; i++)
	{
		if (i == result_int)
		{
			return result;
		}
		result++;
	}
}
cndevRet_t cndevGetCardSN(cndevCardSN_t *cardSN, int devId)
{
	cJSON *config;
	cndevRet_t result;
	config = readJsonFile();

	cJSON *sn_node = cJSON_GetObjectItem(config, "MLU");
	__int64_t sn_int = cJSON_GetArrayItem(sn_node, devId)->valueint;
	cardSN->sn = sn_int;

	cJSON *ret = cJSON_GetObjectItem(config, "MLU_ret");
	int result_int = cJSON_GetArrayItem(ret, devId)->valueint;
	result = CNDEV_SUCCESS;
	for (int i = 0; i <= 9; i++)
	{
		if (i == result_int)
		{
			return result;
		}
		result++;
	}
}
cndevRet_t cndevRelease()
{
	return CNDEV_SUCCESS;
}
cndevRet_t cndevGetCardName(cndevCardName_t *cardName, int devId)
{
	cJSON *config;
	cndevRet_t result;
	config = readJsonFile();

	cJSON *card_type_node = cJSON_GetObjectItem(config, "card_type");
	int card_type = cJSON_GetArrayItem(card_type_node, devId)->valueint;

	if (card_type == 0)
	{
		cardName->id = MLU100;
	}
	else if (card_type == 1)
	{
		cardName->id = MLU270;
	}
	else if (card_type == 16)
	{
		cardName->id = MLU220_M2;
	}
	else if (card_type == 18)
	{
		cardName->id = MLU220_EVB;
	}
	else if (card_type == 19)
	{
		cardName->id = MLU220_M2i;
	}
	else if (card_type == 20)
	{
		cardName->id = MLU290;
	}
	return CNDEV_SUCCESS;
}

const char *cndevGetErrorString(cndevRet_t errorId)
{
	return "mock return value of cndev get error string";
}

cndevRet_t cndevGetPCIeInfo(cndevPCIeInfo_t *deviceInfo, int devId)
{
	cJSON *config;
	cndevRet_t result;
	config = readJsonFile();

	cJSON *pcie_info = cJSON_GetObjectItem(config, "pcie_info");
	cJSON *pcie_node = cJSON_GetArrayItem(pcie_info, devId);

	deviceInfo->domain = cJSON_GetArrayItem(pcie_node, 0)->valueint;
	deviceInfo->bus = cJSON_GetArrayItem(pcie_node, 1)->valueint;
	deviceInfo->device = cJSON_GetArrayItem(pcie_node, 2)->valueint;
	deviceInfo->function = cJSON_GetArrayItem(pcie_node, 3)->valueint;

	return CNDEV_SUCCESS;
}
