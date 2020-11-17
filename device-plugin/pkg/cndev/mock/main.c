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
#include <stdio.h>
int Test_cndevGetDeviceCount()
{
	cndevCardInfo_t *cardNum;
	cardNum = (cndevCardInfo_t *)malloc(sizeof(cndevCardInfo_t));
	cardNum->version = 1;
	cndevRet_t result;
	result = cndevGetDeviceCount(cardNum);
	printf("=== Test cndevGetDeviceCount() ===\nnumMLU:%d\nretMLU:%d\n", cardNum->Number, result);
	return cardNum->Number;
}
void Test_cndevInit()
{
	cndevRet_t result;
	result = cndevInit(0);
	printf("=== Test cndevInit() ===\nretMLU:%d\n", result);
}
void Test_cndevGetCardHealthState(int id)
{
	cndevCardHealthState_t *cardHealthState;
	cardHealthState = (cndevCardHealthState_t *)malloc(sizeof(cndevCardHealthState_t));
	cardHealthState->version = 3;
	cndevRet_t result;
	result = cndevGetCardHealthState(cardHealthState, id);
	printf("=== Test cndevGetCardHealthState() ===\nhealthMLU:%d\nretMLU:%d\n", cardHealthState->health, result);
}
void Test_cndevGetCardSN(int id)
{
	cndevCardSN_t *cardSN;
	cardSN = (cndevCardSN_t *)malloc(sizeof(cndevCardSN_t));
	cardSN->version = 3;
	cndevRet_t result;
	result = cndevGetCardSN(cardSN, id);
	printf("=== Test cndevGetCardSN() ===\nsnMLU:%x\nmotherBoard:%x\nretsnMLU:%d\n", cardSN->sn, cardSN->mother_board_sn, result);
}
void Test_cndevGetPCIeInfo(int id)
{
	cndevPCIeInfo_t *cardPcie;
	cardPcie = (cndevPCIeInfo_t *)malloc(sizeof(cndevPCIeInfo_t));
	cardPcie->version = 3;
	cndevRet_t result;
	result = cndevGetPCIeInfo(cardPcie, id);
	printf("=== Test Test_cndevGetPcieInfo() ===\ndomain:%d\nbus:%d\ndevice:%d\nfunction:%d\nretMLU:%d\n", cardPcie->domain, cardPcie->bus, cardPcie->device, cardPcie->function, result);
}

int main()
{
	Test_cndevInit();
	int num;
	num = Test_cndevGetDeviceCount();
	for (int i = 0; i < num; ++i)
	{
		printf("================ Test card id %d =============\n", i);
		Test_cndevGetCardHealthState(i);
		Test_cndevGetCardSN(i);
		Test_cndevGetPCIeInfo(i);
	}
	return 0;
}
