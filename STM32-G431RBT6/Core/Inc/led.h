/*
 * led.h
 *
 *  Created on: Mar 8, 2025
 *      Author: 莫海峰
 */

#ifndef INC_LED_H_
#define INC_LED_H_

#include "stm32g4xx_hal.h"
// 颜色和闪烁状态变量
typedef enum
{
	COLOR_RED,//红色
	COLOR_GREEN,//绿色
	COLOR_BLUE,//蓝色
	COLOR_YELLOW,//黄色
	COLOR_CYAN,//青色
	COLOR_MAGENTA,//紫色
	COLOR_WHITE,//白色
	COLOR_OFF//关闭
} ColorTypedef;

void Led_SetCorlor(ColorTypedef corlor);

#endif /* INC_LED_H_ */
