/*
 * dev.c
 *
 *  Created on: Aug 14, 2025
 *      Author: 莫海峰
 */
#include "dev.h"
#include "usart.h"
#include "user_usart.h"

SysConfig_t sysconfig;

uint32_t ValveControlTaskCnt = 0; //旋转阀控制任务时间计数

void ConfigInit(void)
{

}

void SysTick_Callback(void)
{
	ValveControlTaskCnt++;
}

void ConsoleControlTask(UART_HandleTypeDef *huart)
{
	uint8_t *pdata = NULL;
	uint8_t datalen = 0;
	if (huart->Instance == USART1)
	{
		Usart_GetData(&huart1, &pdata, &datalen);
		RealtimeResolutionBuff_Append(&huart1, pdata, datalen);
		ResolutionProtocol(&huart1);
	}
	if (huart->Instance == USART2)
	{
		Usart_GetData(&huart2, &pdata, &datalen);
		RealtimeResolutionBuff_Append(&huart2, pdata, datalen);
		ResolutionProtocol(&huart2);
	}
}
