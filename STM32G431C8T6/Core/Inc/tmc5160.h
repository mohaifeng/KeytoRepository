/*
 * tmc5160.h
 *
 *  Created on: Mar 28, 2025
 *      Author: 莫海峰
 */

#ifndef INC_TMC5160_H_
#define INC_TMC5160_H_

#include "main.h"

// TMC5160 寄存器地址定义
#define TMC5160_REG_GCONF        0x00  // 通用配置
#define TMC5160_REG_IHOLD_IRUN   0x10  // 电流控制
#define TMC5160_REG_VACTUAL      0x22  // 实际速度
#define TMC5160_REG_XACTUAL      0x21  // 实际位置
#define TMC5160_REG_XTARGET      0x2D  // 目标位置
#define TMC5160_REG_VSTART       0x23  // 起始速度
#define TMC5160_REG_VMAX         0x27  // 最大速度
#define TMC5160_REG_A1           0x2A  // 加速度
#define TMC5160_REG_AMAX         0x29  // 最大加速度
#define TMC5160_REG_D1           0x2B  // 减速度
#define TMC5160_REG_CHOPCONF     0x6C  // 斩波器配置
#define TMC5160_REG_TPOWERDOWN   0x11  // 待机电流延时
#define TMC5160_REG_TPWMTHRS     0x13  // 速度阈值
#define TMC5160_REG_RAMPMODE     0x20  // 斜坡模式


uint32_t TMC5160_ReadRegister(uint8_t address);
void TMC5160_WriteRegister(uint8_t address, uint32_t value);
void TMC5160_Init(void);
void TMC5160_Enable(void);
void TMC5160_Disable(void);
void TMC5160_MoveTo(int32_t position);
void TMC5160_Move(int32_t relative);
void TMC5160_SetSpeed(uint32_t speed);
#endif /* INC_TMC5160_H_ */
