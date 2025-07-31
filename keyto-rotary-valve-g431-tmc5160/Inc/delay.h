#ifndef __TICK_DELAY_H
#define __TICK_DELAY_H
#include "stm32g4xx.h"

extern uint32_t DelayCont;

void TickDelay(uint32_t delay_time);
void SoftDelay(uint32_t delay_time);
#endif


