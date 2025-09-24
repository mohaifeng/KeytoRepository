/*
 * dev.c
 *
 *  Created on: Aug 14, 2025
 *      Author: 莫海峰
 */
#include "dev.h"
#include "register.h"
#include "flash.h"
#include "usart.h"
#include "user_usart.h"

SysConfig_t sysconfig;
uint32_t USART1_RxTimeCnt = 0;														//串口1接收数据的时间计数
uint32_t USART2_RxTimeCnt = 0;														//串口2接收数据的时间计数
uint32_t ValveControlTaskCnt = 0; //旋转阀控制任务时间计数
uint32_t CAN_HeartCnt = 0; //旋转阀控制任务时间计数

void ConfigInit(void)
{
	FlashData_t flash_data;
	HAL_StatusTypeDef state;
	state = Read_Config_Flash(&flash_data);
	if (state != HAL_OK || flash_data.flash_sysconfig.InitFlag != FLASH_INIT_FLAG)
	{
		sysconfig.InitFlag = FLASH_INIT_FLAG;
		Reset_Register();
	}
	else
	{
		memcpy(&sysconfig, &flash_data.flash_sysconfig, sizeof(SysConfig_t));
	}
}

void SysTick_Callback(void)
{
	USART1_RxTimeCnt++;
	USART2_RxTimeCnt++;
	ValveControlTaskCnt++;
	CAN_HeartCnt++;
}

void ConsoleControlTask(UART_HandleTypeDef *huart)
{
	uint8_t *pdata = NULL;
	uint8_t datalen = 0;
	Usart_GetData(huart, &pdata, &datalen);
	RealtimeResolutionBuff_Append(huart, pdata, datalen);
	ResolutionProtocol(huart);

}
