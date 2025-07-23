/*
 * ow.c
 *
 *  Created on: Jul 22, 2025
 *      Author: 莫海峰
 */
#include "ow.h"
#include "main.h"
#include "led.h"

Ow_Statue ow1_status_flag = OW_IDLE; //光耦1状态标志
Ow_Statue ow2_status_flag = OW_IDLE; //光耦2状态标志
volatile uint32_t ow1_trigger_stime = 0;    // 稳定计时
volatile uint32_t ow2_trigger_stime = 0;    // 稳定计时

void Ow1_Task(void)
{
	switch (ow1_status_flag)
	{
		case OW_IDLE:
			if (HAL_GPIO_ReadPin(OW1_GPIO_Port, OW1_Pin) == GPIO_PIN_RESET)
			{
				ow1_status_flag = OW_TRIGGER;
				ow1_trigger_stime = HAL_GetTick();
			}
			break;
		case OW_TRIGGER:
			if (HAL_GetTick() - ow1_trigger_stime >= STABLE_TIME)
			{
				ow1_status_flag = OW_STABLE;
			}
			break;
		case OW_STABLE:
			SysConfig.ow1_status = !HAL_GPIO_ReadPin(OW1_GPIO_Port, OW1_Pin);
			ow1_status_flag = OW_IDLE;
			break;
		default:
			break;
	}
}

void Ow2_Task(void)
{
	switch (ow2_status_flag)
	{
		case OW_IDLE:
			if (HAL_GPIO_ReadPin(OW2_GPIO_Port, OW2_Pin) == GPIO_PIN_RESET)
			{
				ow2_status_flag = OW_TRIGGER;
				ow2_trigger_stime = HAL_GetTick();
			}
			break;
		case OW_TRIGGER:
			if (HAL_GetTick() - ow2_trigger_stime >= STABLE_TIME)
			{
				ow2_status_flag = OW_STABLE;
			}
			break;
		case OW_STABLE:
			if (HAL_GPIO_ReadPin(OW2_GPIO_Port, OW2_Pin) == GPIO_PIN_RESET)
			{
				SysConfig.ow2_status = 1;
				Led_SetCorlor(COLOR_GREEN);
			}
			else
			{
				SysConfig.ow2_status = 0;
				Led_SetCorlor(COLOR_OFF);
			}
			ow2_status_flag = OW_IDLE;
			break;
		default:
			break;
	}
}
