/*
 * motor_control.c
 *
 *  Created on: May 19, 2025
 *      Author: 莫海峰
 */
#include "motor_control.h"

BMConfig_t BMConfig = { 0 };


// 回调函数示例
void setMotorDirCallback(void *reg_ptr, RegValue *new_value)
{
	// 电机方向设置逻辑
	*(uint8_t*) reg_ptr = new_value->u8v;
	// 可以在这里添加硬件控制代码
}
