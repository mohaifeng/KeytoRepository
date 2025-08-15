/*
 * led.c
 *
 *  Created on: Mar 26, 2025
 *      Author: 莫海峰
 */
#include "led.h"

Led_Status_t led_state = LED_OFF;
volatile uint8_t target_blink_count = 0; //目标LED闪烁次数
LedControl_t led_control = { .currentState = LED_OFF, .blinkCount = 0, .lastActionTime = 0, .blinkPhase = 0 };

void Led_Init(void)
{
	HAL_GPIO_WritePin(LED_BLUE_GPIO_Port, LED_BLUE_Pin, GPIO_PIN_RESET);
	HAL_Delay(1000);
	HAL_GPIO_WritePin(LED_BLUE_GPIO_Port, LED_BLUE_Pin, GPIO_PIN_SET);
}

//事件触发改变LED状态函数
void Led_HandleEvent(Led_Event_t event)
{
	switch (event)
	{
		case EVENT_CMD_RUNNING:
			led_state = LED_ON;
			break;
		case EVENT_CMD_FINISH:
			led_state = LED_OFF;
			break;
		case EVENT_SYSERROR:
			led_state = LED_BLINK;
			break;
		default:
			break;
	}
}
//LED状态函数，在main循环中根据LED状态调用不同功能函数
void Led_Task(Led_Status_t state)
{
	switch (state)
	{
		case LED_OFF:
			LED_OFF();
			break;
		case LED_ON:
			LED_ON();
			break;
		case LED_BLINK:
			Led_Blink();
			break;
		default:
			break;
	}

}

void Led_Blink(void)
{
	uint32_t currentTime = HAL_GetTick();
	if (led_control.currentState != LED_BLINK)
	{
		led_control.blinkCount = 0; // 当前已闪烁次数
		led_control.lastActionTime = currentTime; // 上次动作时间戳
		led_control.blinkPhase = 1; //1:亮；0：灭
		led_control.currentState = LED_BLINK;
		LED_ON();
		return;
	}
	if ((currentTime - led_control.lastActionTime)
			>= (led_control.blinkPhase ?
			BLINK_INTERVAL :
																		(led_control.blinkCount < target_blink_count ? BLINK_INTERVAL : PAUSE_INTERVAL)))
	{
		if (led_control.blinkCount < target_blink_count)
		{
			// 正常闪烁周期
			led_control.blinkPhase = !led_control.blinkPhase;
			LED_Toggle();
			if (!led_control.blinkPhase)
			{
				// 完成一次完整闪烁(亮+灭)
				led_control.blinkCount++;
			}
		}
		else
		{
			// 已完成一组闪烁，检查状态是否仍为LED_BLINK_RED
			if (led_control.currentState == LED_BLINK)
			{
				led_control.blinkCount = 0;
				led_control.blinkPhase = 1;
				LED_ON();
			}
		}
		led_control.lastActionTime = currentTime;
	}
}
