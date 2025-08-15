/*
 * tmc5160.h
 *
 *  Created on: Mar 28, 2025
 *      Author: 莫海峰
 */

#ifndef INC_TMC5160_H_
#define INC_TMC5160_H_

#include "main.h"

#define TMC5160_SD_MODE				0		//0：内部斜坡,DIR,STEP用作外部停止,1：外部步进信号，DIR,STEP用作外部方向和步进信号
#if TMC5160_SD_MODE == 0
#define TMC5160_STOP_MODE			1		//0:内部停止指令 1:REFR_DIR作为外部停止信号
#endif
#if TMC5160_SD_MODE == 0
#define TMC_EXT_STOP()			HAL_GPIO_WritePin(TMC_DIR_GPIO_Port, TMC_DIR_Pin, GPIO_PIN_SET)
#define TMC_EXT_RELEASE()		HAL_GPIO_WritePin(TMC_DIR_GPIO_Port, TMC_DIR_Pin, GPIO_PIN_RESET)
#endif

#define TMC_DRV_ON()			HAL_GPIO_WritePin(DRV_ENN_GPIO_Port, DRV_ENN_Pin, GPIO_PIN_RESET)
#define TMC_DRV_OFF()			HAL_GPIO_WritePin(DRV_ENN_GPIO_Port, DRV_ENN_Pin, GPIO_PIN_SET)
#define TMC_SPI_MAX_DELAY 200

//将ustep/s速度转换为ustep/t速度，其中t= 2^24 / fclk,fclk=12M
#define Convert_usteps_To_ustept(usteps) ((uint64_t)usteps*0x1000000/12000000)
//将ustep/t速度转换为ustep/s速度,v[Hz] = v[5160] * ( fCLK[Hz]/2 / 2^23 )
#define Convert_ustept_To_usteps(ustept) (((uint64_t)ustept*12000000)/0x1000000)
//将ustep/s^2加减速转换为ustep/ta^2加减速，ustep acceleration a[Hz/s]= a[5160] * fCLK[Hz]^2 / (512*256) / 2^24
#define ConvertACC_usteptt_To_ustepss(usteptt) ((((uint64_t)usteptt*12000000*12000000)/((uint64_t)0x20000000000)))

//将ustep/ta^2加减速转换为ustep/s^2加减速，ustep acceleration a[Hz/s]= a[5160] * fCLK[Hz]^2 / (512*256) / 2^24
//0.01527099483
//#define ConvertACC_ustepss_To_usteptt(ustepss) ((((uint64_t)ustepss*(uint64_t)0x20000000000))/(12000000*12000000))
#define ConvertACC_ustepss_To_usteptt(ustepss) (ustepss*0.01527099483)


// TMC5160 寄存器地址定义
#define TMC5160_REG_GCONF        0x00  // 通用配置
#define TMC5160_REG_IHOLD_IRUN   0x10  // 电流控制
#define TMC5160_REG_VACTUAL      0x22  // 实际速度
#define TMC5160_REG_XACTUAL      0x21  // 实际位置
#define TMC5160_REG_XTARGET      0x2D  // 目标位置
#define TMC5160_REG_VSTART       0x23  // 起始速度
#define TMC5160_REG_V1        	 0x25  // 加速度阈值速度
#define TMC5160_REG_VMAX         0x27  // 最大速度
#define TMC5160_REG_A1           0x2A  // 加速度
#define TMC5160_REG_VSTOP        0x2B  // 停止速度
#define TMC5160_REG_AMAX         0x29  // 最大加速度
#define TMC5160_REG_D1           0x2B  // 减速度
#define TMC5160_REG_CHOPCONF     0x6C  // 斩波器配置
#define TMC5160_REG_TPOWERDOWN   0x11  // 待机电流延时
#define TMC5160_REG_TPWMTHRS     0x13  // 速度阈值
#define TMC5160_REG_RAMPMODE     0x20  // 斜坡模式

// TMC5160 细分定义
#define MRRS_1		0x00
#define MRRS_2		0x01
#define MRRS_4		0x02
#define MRRS_8		0x03
#define MRRS_16		0x04
#define MRRS_32		0x05
#define MRRS_64		0x06
#define MRRS_128	0x07
#define MRRS_256  0x08

void TMC_Delay(uint32_t delay_time);
int32_t TMC5160_SPIReadData(uint8_t address);
void TMC5160_SPIWriteInt(uint8_t address, uint32_t value);
void TMC5160_Init(void);
void TMC5160_Enable(void);
void TMC5160_Disable(void);
void TMC5160_MoveTo(int32_t position);
void TMC5160_Move(int32_t relative);
void TMC5160_SetSpeed(uint32_t speed);
#endif /* INC_TMC5160_H_ */
