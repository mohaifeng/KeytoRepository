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
  LED_OFF,
  LED_SOLID_RED,
  LED_SOLID_GREEN,
  LED_SOLID_BLUE,
  LED_BLINK_RED,
	LED_BLINK_MAGENTA,
  LED_PATTERN_ALTERNATE
} Led_Statue_t;

typedef enum
{
	EVENT_NONE,
	EVENT_OW_TRIGGER,
	EVENT_CMD_RUNNING,
	EVENT_CMD_FINISH,
	EVENT_SYSERROR,
	EVENT_WARNING,
} Led_Event_t;

extern Led_Statue_t led_state;

void Led_SetCorlor(ColorTypedef corlor);
void Led_Init(void);
#endif /* INC_LED_H_ */
