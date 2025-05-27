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

void Led_Init(void);
void LED_Toggle(void);
void Led_Twinkle(uint8_t num);

#endif /* INC_LED_H_ */
