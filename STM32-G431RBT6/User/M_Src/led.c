/*
 * led.c
 *
 *  Created on: Mar 8, 2025
 *      Author: 莫海峰
 */
#include "led.h"
#include "tim.h"

Led_Statue_t led_state = LED_OFF;
volatile uint8_t target_blink_count = 0; //目标LED闪烁次数
LedControl_t led_control = { .currentState = LED_OFF, .blinkCount = 0, .lastActionTime = 0, .blinkPhase = 0 };
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

void Led_HandleEvent(Led_Event_t event)
{
	switch (event)
	{
		case EVENT_OW_TRIGGER:
			led_state = LED_SOLID_GREEN;
			break;
		case EVENT_CMD_RUNNING:
			led_state = LED_SOLID_BLUE;
			break;
		case EVENT_CMD_FINISH:
			led_state = LED_OFF;
			break;
		case EVENT_SYSERROR:
			led_state = LED_BLINK_MAGENTA;
			break;
		case EVENT_WARNING:
			led_state = LED_BLINK_RED;
			break;
		default:
			break;
	}
}

static void Led_Blink(ColorTypedef color)
{
	uint32_t currentTime = HAL_GetTick();
	if (led_control.currentState != LED_BLINK_RED)
	{
		led_control.blinkCount = 0; // 当前已闪烁次数
		led_control.lastActionTime = currentTime; // 上次动作时间戳
		led_control.blinkPhase = 1; //1:亮；0：灭
		led_control.currentState = LED_BLINK_RED;
		Led_SetCorlor(color);
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
			Led_SetCorlor(led_control.blinkPhase ? color : COLOR_OFF);
			if (!led_control.blinkPhase)
			{
				// 完成一次完整闪烁(亮+灭)
				led_control.blinkCount++;
			}
		}
		else
		{
			// 已完成一组闪烁，检查状态是否仍为LED_BLINK_RED
			if (led_control.currentState == LED_BLINK_RED)
			{
				led_control.blinkCount = 0;
				led_control.blinkPhase = 1;
				Led_SetCorlor(color);
			}
		}
		led_control.lastActionTime = currentTime;
	}
}

void ApplyLedState(Led_Statue_t state)
{
	switch (state)
	{
		case LED_OFF:
			Led_SetCorlor(COLOR_OFF);
			break;
		case LED_SOLID_RED:
			Led_SetCorlor(COLOR_RED);
			break;
		case LED_SOLID_GREEN:
			Led_SetCorlor(COLOR_GREEN);
			break;
		case LED_SOLID_BLUE:
			Led_SetCorlor(COLOR_BLUE);
			break;
		case LED_BLINK_RED:
			Led_Blink(COLOR_RED);
			break;
		case LED_BLINK_MAGENTA:
			Led_Blink(COLOR_MAGENTA);
			break;
		case LED_PATTERN_ALTERNATE:
			break;
		default:
			break;
	}

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
