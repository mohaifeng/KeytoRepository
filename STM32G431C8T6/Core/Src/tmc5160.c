/*
 * tmc5160.c
 *
 *  Created on: Mar 28, 2025
 *      Author: 莫海峰
 */

#include "tmc5160.h"

extern SPI_HandleTypeDef hspi1;

uint32_t TMC5160_ReadRegister(uint8_t address)
{
	uint8_t txData[5] = { 0 };
	uint8_t rxData[5] = { 0 };
	uint32_t value = 0;

	// TMC5160读取格式：地址 | 0x80
	txData[0] = address | 0x80;
	HAL_SPI_TransmitReceive(&hspi1, txData, rxData, 5, HAL_MAX_DELAY);
	value = (rxData[1] << 24) | (rxData[2] << 16) | (rxData[3] << 8) | rxData[4];
	return value;
}

void TMC5160_WriteRegister(uint8_t address, uint32_t value)
{
	uint8_t txData[5] = { 0 };

	txData[0] = address & 0x7F; // 写入地址最高位为0
	txData[1] = (value >> 24) & 0xFF;
	txData[2] = (value >> 16) & 0xFF;
	txData[3] = (value >> 8) & 0xFF;
	txData[4] = value & 0xFF;

	HAL_SPI_Transmit(&hspi1, txData, 5, HAL_MAX_DELAY);
}

void TMC5160_Init(void)
{
	// 复位驱动器
	TMC5160_WriteRegister(TMC5160_REG_GCONF, 0x00000000);
	HAL_Delay(10);

	// 配置驱动器模式
	TMC5160_WriteRegister(TMC5160_REG_GCONF, 0x00000008); // 使用SPI控制

	// 配置电机参数
	TMC5160_WriteRegister(TMC5160_REG_CHOPCONF, (8 << 24) | // MRES: 256微步
			(3 << 15) | // TOFF: 3
			(4 << 12)); // HSTRT: 4

	// 配置电流
	TMC5160_WriteRegister(TMC5160_REG_IHOLD_IRUN, (10<<16)|(10<<8)|(5<<0));  // // IRUN=10, IHOLD=5

	// 配置速度和加速度
	TMC5160_WriteRegister(TMC5160_REG_TPOWERDOWN, 10); // 待机电流延时
	TMC5160_WriteRegister(TMC5160_REG_TPWMTHRS, 500);  // 速度阈值
	TMC5160_WriteRegister(TMC5160_REG_VSTART, 10);     // 起始速度
	TMC5160_WriteRegister(TMC5160_REG_A1, 1000);       // 加速度
	TMC5160_WriteRegister(TMC5160_REG_AMAX, 1000);     // 最大加速度
	TMC5160_WriteRegister(TMC5160_REG_VMAX, 50000);    // 最大速度
	TMC5160_WriteRegister(TMC5160_REG_D1, 500);        // 减速度
}

void TMC5160_Enable(void)
{
	TMC5160_WriteRegister(TMC5160_REG_GCONF, 0x00000008); // 启用驱动器
}

void TMC5160_Disable(void)
{
	TMC5160_WriteRegister(TMC5160_REG_GCONF, 0x00000000); // 禁用驱动器
}
void TMC5160_MoveTo(int32_t position)
{
	TMC5160_WriteRegister(TMC5160_REG_RAMPMODE, 0); // 定位模式
	TMC5160_WriteRegister(TMC5160_REG_XTARGET, position);
}

void TMC5160_Move(int32_t relative)
{
	int32_t current = TMC5160_ReadRegister(TMC5160_REG_XACTUAL);
	TMC5160_MoveTo(current + relative);
}

void TMC5160_SetSpeed(uint32_t speed)
{
	TMC5160_WriteRegister(TMC5160_REG_VMAX, speed);
}
