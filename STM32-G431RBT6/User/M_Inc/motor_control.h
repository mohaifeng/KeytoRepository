/*
 * motor_control.h
 *
 *  Created on: Jun 19, 2025
 *      Author: 莫海峰
 */

#ifndef INC_MOTOR_CONTROL_H_
#define INC_MOTOR_CONTROL_H_

#include "register.h"
#include "stm32g4xx_hal.h"

typedef struct
{
	int32_t PWMFre;
	uint8_t Dir;
	uint16_t ReverseDelay;
	uint8_t PWMMaxPulse;
// 其他电机配置...
} BMConfig_t;

extern BMConfig_t BMConfig;

void setMotorDirCallback(void *reg_ptr, RegValue *new_value);

#endif /* INC_MOTOR_CONTROL_H_ */
