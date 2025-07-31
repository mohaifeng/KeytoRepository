#include "stm32g4xx.h"
#include "delay.h"

uint32_t DelayCont=0;

/***********************
函数功能：通过滴答定时器延时，单位ms
函数说明：不能用于中断处理中，否则会出现死机
************************/
void TickDelay(uint32_t delay_time)
{
  DelayCont=delay_time;
  while(DelayCont);
}
/***********************
函数功能：通过软件延时，单位us
函数说明：
************************/
void SoftDelay(uint32_t delay_time)
{
	delay_time=delay_time*5;
	while(delay_time--)
		__NOP();
}
