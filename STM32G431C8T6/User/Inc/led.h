/*
 * led.h
 *
 *  Created on: Mar 26, 2025
 *      Author: 莫海峰
 */

#ifndef INC_LED_H_
#define INC_LED_H_

#include "stm32g4xx_hal.h"
#include "gpio.h"

#define BLINK_INTERVAL 300    // 闪烁间隔300ms
#define PAUSE_INTERVAL 2000   // 组间隔2秒

#define LED_ON()  		HAL_GPIO_WritePin(LED_BLUE_GPIO_Port, LED_BLUE_Pin, GPIO_PIN_RESET)
#define LED_OFF()  		HAL_GPIO_WritePin(LED_BLUE_GPIO_Port, LED_BLUE_Pin, GPIO_PIN_SET)
#define LED_Toggle()  HAL_GPIO_TogglePin(LED_BLUE_GPIO_Port, LED_BLUE_Pin)

typedef enum
{
	LED_OFF,
	LED_ON,
	LED_BLINK,
} Led_Status_t;

typedef enum
{
	EVENT_NONE,
	EVENT_CMD_RUNNING,
	EVENT_CMD_FINISH,
	EVENT_SYSERROR,
} Led_Event_t;

typedef struct
{
	Led_Status_t currentState;
	uint8_t blinkCount;       // 当前已闪烁次数
	uint32_t lastActionTime;  // 上次动作时间戳
	uint8_t blinkPhase;          // 当前闪烁相位(true=亮, false=灭)
} LedControl_t;

extern Led_Status_t led_state;
extern volatile uint8_t target_blink_count;

void Led_Init(void);
void Led_HandleEvent(Led_Event_t event);
void Led_Task(Led_Status_t state);
void Led_Blink(void);

#endif /* INC_LED_H_ */
