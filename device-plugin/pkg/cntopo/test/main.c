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
#include <stdio.h>
#include <stdbool.h>

void Test_cntopo() {
	cntopoContext_t ctx;
	cntopoDevSet_t* dev_sets;
	cntopoMachineInfo_t node_info;
	cntopoQuery_t query;
	cntopoTopo_t *topos;

	size_t max_ring_num = 100;
	size_t num_dev_set;
	size_t num_topo;
	size_t target[4][2] = {
		{0,1},
		{4,5},
		{2,3},
		{6,7},
	};
	size_t *size_bytes;

	bool test_failed = false;

	const char *machine_label = "localhost";

	dev_sets = (cntopoDevSet_t *)malloc(4 * 2 * sizeof(cntopoDevSet_t));

	cntopoInitContext(&ctx);
	cntopoGetLocalMachineInfo(ctx, &node_info, size_bytes);
	cntopoAddMachineInfo(ctx, node_info, machine_label);
	cntopoCreateQuery(ctx, &query);
	cntopoSetDevNumFilter(query, machine_label, 4);
	cntopoFindDevSets(query, RING, max_ring_num, &dev_sets, &num_dev_set);

	for (size_t i = 0; i < num_dev_set; i++) {
		size_t length;
		cntopoGetDevSetSize(dev_sets[i], &length);
		cntopoFindTopos(dev_sets[i], RING, &topos, &num_topo);
		for (size_t j = 0; j < length; j++) {
			cntopoDevInfo_t dev_info;
			cntopoGetDevInfoFromDevSet(dev_sets[i], j, &dev_info);
			if (dev_info.dev_ordinal != target[i][j]) {
				printf("Test failed\n");
				test_failed = true;
			}
		}
	}

	if (!test_failed) {
		printf("Test succeeded\n");
	}

	free(dev_sets);
}

int main() {
	Test_cntopo();
	return 0;
}
