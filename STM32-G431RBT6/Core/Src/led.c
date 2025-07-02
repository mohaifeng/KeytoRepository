/*
 * led.c
 *
 *  Created on: Mar 8, 2025
 *      Author: 莫海峰
 */
#include "led.h"

extern TIM_HandleTypeDef htim8;
// 设置RGB颜色 (0-255)
static void RGBLED_SetColor(uint8_t r, uint8_t g, uint8_t b)
{
	// 将8位颜色值映射到PWM占空比
	uint16_t pulse_r = (uint16_t) ((htim8.Init.Period + 1) * (255 - r) / 255);
	uint16_t pulse_g = (uint16_t) ((htim8.Init.Period + 1) * (255 - g) / 255);
	uint16_t pulse_b = (uint16_t) ((htim8.Init.Period + 1) * (255 - b) / 255);

	__HAL_TIM_SET_COMPARE(&htim8, TIM_CHANNEL_1, pulse_r);
	__HAL_TIM_SET_COMPARE(&htim8, TIM_CHANNEL_2, pulse_g);
	__HAL_TIM_SET_COMPARE(&htim8, TIM_CHANNEL_3, pulse_b);
}

void Led_SetCorlor(ColorTypedef corlor)
{
	switch (corlor)
	{
		case COLOR_RED: //红色
			RGBLED_SetColor(100, 0, 0);
			break;
		case COLOR_GREEN: //绿色
			RGBLED_SetColor(0, 100, 0);
			break;
		case COLOR_BLUE: //蓝色
			RGBLED_SetColor(0, 0, 100);
			break;
		case COLOR_YELLOW: //黄色
			RGBLED_SetColor(100, 100, 0);
			break;
		case COLOR_CYAN: //青色
			RGBLED_SetColor(0, 100, 100);
			break;
		case COLOR_MAGENTA: //紫色
			RGBLED_SetColor(100, 0, 100);
			break;
		case COLOR_WHITE: //白色
			RGBLED_SetColor(100, 100, 100);
			break;
		case COLOR_OFF: //关闭
			RGBLED_SetColor(0, 0, 0);
			break;
		default:
			break;
	}
}
void Led_Init(void)
{
	Led_SetCorlor(COLOR_WHITE);
	HAL_Delay(1000);
	Led_SetCorlor(COLOR_OFF);
}
