/*
 * led.c
 *
 *  Created on: Mar 8, 2025
 *      Author: 莫海峰
 */
#include "led.h"

extern uint8_t ow1_state;

void Led_Init(void)
{
	HAL_GPIO_WritePin(GPIOC,LED1_RED_Pin | LED2_GREEN_Pin,GPIO_PIN_RESET);
	HAL_GPIO_WritePin(LED3_BLUE_GPIO_Port,LED3_BLUE_Pin,GPIO_PIN_RESET);
	HAL_Delay(1000);
	HAL_GPIO_WritePin(GPIOC,LED1_RED_Pin | LED2_GREEN_Pin,GPIO_PIN_SET);
	HAL_GPIO_WritePin(LED3_BLUE_GPIO_Port,LED3_BLUE_Pin,GPIO_PIN_SET);
}

void Led_Color_Set(LED_COLOR_TYPEDEF idex)
{
	switch(idex)
		//红绿蓝
		{
		case 0:
			HAL_GPIO_WritePin(GPIOC,LED2_GREEN_Pin,GPIO_PIN_SET);
			HAL_GPIO_WritePin(LED3_BLUE_GPIO_Port,LED3_BLUE_Pin,GPIO_PIN_SET);
			HAL_GPIO_WritePin(GPIOC,LED1_RED_Pin,GPIO_PIN_RESET);
			break;
		case 1:
			HAL_GPIO_WritePin(LED3_BLUE_GPIO_Port,LED3_BLUE_Pin,GPIO_PIN_SET);
			HAL_GPIO_WritePin(GPIOC,LED1_RED_Pin,GPIO_PIN_SET);
			HAL_GPIO_WritePin(GPIOC,LED2_GREEN_Pin,GPIO_PIN_RESET);
			break;
		case 2:
			HAL_GPIO_WritePin(GPIOC,LED2_GREEN_Pin,GPIO_PIN_SET);
			HAL_GPIO_WritePin(GPIOC,LED1_RED_Pin,GPIO_PIN_SET);
			HAL_GPIO_WritePin(LED3_BLUE_GPIO_Port,LED3_BLUE_Pin,GPIO_PIN_RESET);
			break;
		default:
			HAL_GPIO_WritePin(GPIOC,LED2_GREEN_Pin,GPIO_PIN_SET);
			HAL_GPIO_WritePin(LED3_BLUE_GPIO_Port,LED3_BLUE_Pin,GPIO_PIN_SET);
			HAL_GPIO_WritePin(GPIOC,LED1_RED_Pin,GPIO_PIN_SET);
			break;
		}
}

void Ow_Init(void)
{
	ow1_state = HAL_GPIO_ReadPin(GPIOA,GPIO_PIN_5);
}

void Ow_Set(void)
{
	if(ow1_state == GPIO_PIN_RESET)
	{
		Led_Color_Set(RED);
	}
	else
	{
		Led_Color_Set(BLUE);
	}
}

