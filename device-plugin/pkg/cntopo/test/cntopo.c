/*
 * Copyright 2022 Cambricon, Inc.
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

#include "../include/cntopo.h"
#include "cJSON.h"
#include <stdio.h>
#include <stdlib.h>

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

cntopoResult_t cntopoInitContext(cntopoContext_t *ctx) {
	return CNTOPO_RET_SUCCESS;
}

cntopoResult_t cntopoGetLocalMachineInfo(cntopoContext_t ctx,
					 cntopoMachineInfo_t *node_info,
					 size_t *size_bytes) {
	return CNTOPO_RET_SUCCESS;
}

cntopoResult_t cntopoAddMachineInfo(cntopoContext_t ctx,
				    cntopoMachineInfo_t node_info,
				    const char *machine_label) {
	return CNTOPO_RET_SUCCESS;
}

cntopoResult_t cntopoCreateQuery(cntopoContext_t ctx,
				 cntopoQuery_t *query_handle) {
	return CNTOPO_RET_SUCCESS;
}

cntopoResult_t cntopoDestroyQuery(cntopoQuery_t query_handle) {
	return CNTOPO_RET_SUCCESS;
}

cntopoResult_t cntopoSetDevNumFilter(cntopoQuery_t query_handle,
				     const char *machine_label,
				     uint32_t num_dev) {
	return CNTOPO_RET_SUCCESS;
}

cntopoResult_t cntopoSetWhitelistDevOrdinal(cntopoQuery_t query_handle,
					    const char *machine_label,
					    size_t dev_ordinal) {
	return CNTOPO_RET_SUCCESS;
}

cntopoResult_t cntopoFindDevSets(cntopoQuery_t query_handle,
				 cntopoTopoType_t topo_type,
				 size_t max_topo_num, cntopoDevSet_t **dev_sets,
				 size_t *num_dev_set) {
	cJSON *config;
	config = readJsonFile();
	cJSON *devsets = cJSON_GetObjectItem(config, "dev_sets");
	size_t p = (size_t)cJSON_GetArraySize(devsets);
	cJSON *devs = cJSON_GetArrayItem(devsets, 0);
	size_t n = (size_t)cJSON_GetArraySize(devs);
	size_t u;
	size_t **ds = malloc(p * n * sizeof(size_t *));
	for (size_t i = 0; i < p; i++) {
		ds[i] = malloc(n * sizeof(size_t));
	}
	for (size_t i = 0; i < p; i++) {
		for (size_t j = 0; j < n; j++) {
			cJSON *s = cJSON_GetArrayItem(devsets, (int)i);
			u = (size_t)cJSON_GetArrayItem(s, (int)j)->valueint;
			ds[i][j] = u;
		}
	}
	*dev_sets = (cntopoDevSet_t *)ds;
	cJSON_Delete(config);
	*num_dev_set = p;
	return CNTOPO_RET_SUCCESS;
}

cntopoResult_t cntopoGetDevSetSize(cntopoDevSet_t dev_set, size_t *size) {
	cJSON *config;
	config = readJsonFile();
	cJSON *devsets = cJSON_GetObjectItem(config, "dev_sets");
	size_t p = (size_t)cJSON_GetArraySize(devsets);
	cJSON *devs = cJSON_GetArrayItem(devsets, 0);
	size_t n = (size_t)cJSON_GetArraySize(devs);
	cJSON_Delete(config);
	*size = n;
	return CNTOPO_RET_SUCCESS;
}

cntopoResult_t cntopoFindTopos(cntopoDevSet_t dev_set,
			       cntopoTopoType_t topo_type, cntopoTopo_t **topos,
			       size_t *num_topo) {
	*num_topo = (size_t)2;
	return CNTOPO_RET_SUCCESS;
}

cntopoResult_t cntopoGetDevInfoFromDevSet(cntopoDevSet_t dev_set,
					  size_t dev_index,
					  cntopoDevInfo_t *dev_info) {
	dev_info->dev_ordinal = *((size_t *)(dev_set) + dev_index);
	return CNTOPO_RET_SUCCESS;
}
