/*
 * led.h
 *
 *  Created on: Mar 8, 2025
 *      Author: 莫海峰
 */

#ifndef INC_LED_H_
#define INC_LED_H_

#include "stm32g4xx_hal.h"
// 颜色变量
typedef enum
{
	COLOR_RED, //红色
	COLOR_GREEN, //绿色
	COLOR_BLUE, //蓝色
	COLOR_YELLOW, //黄色
	COLOR_CYAN, //青色
	COLOR_MAGENTA, //紫色
	COLOR_WHITE, //白色
	COLOR_OFF //关闭
} ColorTypedef;

typedef enum
{
	Led_On = 1,
	Led_Off,
	Led_flick
} Led_Statue;

void Led_SetCorlor(ColorTypedef corlor);
void Led_Init(void);
#endif /* INC_LED_H_ */
