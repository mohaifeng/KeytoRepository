/*
 * led.h
 *
 *  Created on: Mar 8, 2025
 *      Author: 莫海峰
 */

#ifndef INC_LED_H_
#define INC_LED_H_

#include "stm32g4xx_hal.h"

#define BLINK_INTERVAL 300    // 闪烁间隔300ms
#define PAUSE_INTERVAL 2000   // 组间隔2秒

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

typedef struct
{
	Led_Statue_t currentState;
	uint8_t blinkCount;       // 当前已闪烁次数
	uint32_t lastActionTime;  // 上次动作时间戳
	uint8_t blinkPhase;          // 当前闪烁相位(true=亮, false=灭)
} LedControl_t;

extern Led_Statue_t led_state;
extern volatile uint8_t target_blink_count;

void Led_SetCorlor(ColorTypedef corlor);
void Led_Init(void);
void Led_HandleEvent(Led_Event_t event);
void ApplyLedState(Led_Statue_t state);

#endif /* INC_LED_H_ */
