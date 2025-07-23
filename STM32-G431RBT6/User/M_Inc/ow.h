/*
 * ow.h
 *
 *  Created on: Jul 22, 2025
 *      Author: 莫海峰
 */

#ifndef M_INC_OW_H_
#define M_INC_OW_H_

#include "stm32g4xx_hal.h"

#define STABLE_TIME 5//光耦滤波时间5ms

typedef enum
{
	OW_IDLE,
	OW_TRIGGER,
	OW_STABLE,
} Ow_Statue;

extern Ow_Statue ow1_status_flag;
extern Ow_Statue ow2_status_flag;
extern volatile uint32_t ow1_trigger_stime;
extern volatile uint32_t ow2_trigger_stime;

void Ow1_Task();
void Ow2_Task();

#endif /* M_INC_OW_H_ */
