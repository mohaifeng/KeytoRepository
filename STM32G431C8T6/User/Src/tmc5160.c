/*
 * tmc5160.c
 *
 *  Created on: Mar 28, 2025
 *      Author: 莫海峰
 */

#include "tmc5160.h"
#include "spi.h"

/***********************
 函数功能：通过软件延时，单位us
 函数说明：
 ************************/
void TMC_Delay(uint32_t delay_time)
{
	delay_time = delay_time * 5;
	while (delay_time--)
		__NOP();
}

int32_t TMC5160_SPIReadData(uint8_t address)
{
	uint8_t txData[5] = { 0 };
	uint8_t rxData[5] = { 0 };
	int32_t value = 0;
	txData[0] = address & 0x7F; //读地址最高位为0
	HAL_SPI_TransmitReceive(&hspi1, txData, rxData, 5, TMC_SPI_MAX_DELAY);
	TMC_Delay(1);
	HAL_SPI_TransmitReceive(&hspi1, txData, rxData, 5, TMC_SPI_MAX_DELAY);
	value = (rxData[1] << 24) | (rxData[2] << 16) | (rxData[3] << 8) | rxData[4];
	return value;
}

void TMC5160_SPIWriteInt(uint8_t address, int32_t value)
{
	uint8_t txData[5] = { 0 };
	txData[0] = address | 0x80; // 写入地址最高位为1
	txData[1] = (value >> 24) & 0xFF;
	txData[2] = (value >> 16) & 0xFF;
	txData[3] = (value >> 8) & 0xFF;
	txData[4] = value & 0xFF;
	if (HAL_SPI_Transmit(&hspi1, txData, 5, TMC_SPI_MAX_DELAY) != HAL_OK)
	{
		Error_Handler();
	}
}

void TMC5160_Init(void)
{
	/**********************,MOTOR_VALVE*********************/
	TMC5160_SPIWriteInt(0x10, 0x00020F00); //Power_Set.Power_Config); 	// IHOLD_IRUN: IHOLD=9, IRUN=15 , IHOLDDELAY=2
	TMC5160_SPIWriteInt(0x11, 0x00000007);	// TPOWERDOWN=10:
	TMC5160_SPIWriteInt(0x12, 0x0000000A);	// TPOWERDOWN=10:
	TMC5160_SPIWriteInt(0x13, Convert_usteps_To_ustept(1500*SysConfig.MotorValveConfig.MicroStep));
	//TMC5160_SPIWriteInt(0x13, 	0);
	//TMC5160_SPIWriteInt(0x14, 	0x00004189); 		// writing value 0x00004189 = 16777 = 0.0 to address 11 = 0x14(TCOOLTHRS)
	TMC5160_SPIWriteInt(0x15, 0x00000000); 		// writing value 0x00000000 = 0 = 0.0 to address 12 = 0x15(THIGH)

	TMC5160_SPIWriteInt(0x00, 0x00000002);	//faststandstill，0x00000002

	TMC5160_SPIWriteInt(0x20, 0);
	TMC5160_SPIWriteInt(0x21, 0); 		// writing value 0xFFCC12F0 = 0 = 0.0 to address 14 = 0x21(XACTUAL)
	TMC5160_SPIWriteInt(0x23, 500);					//VSTART
	TMC5160_SPIWriteInt(0x24, ConvertACC_ustepss_To_usteptt(SysConfig.MotorValveConfig.Acceleration));//A1 VSTART-V1 速度模式不用配置
	TMC5160_SPIWriteInt(0x25, (SysConfig.MotorValveConfig.MicroStep * 100));				//第一阶段加减速阀值
	TMC5160_SPIWriteInt(0x26, ConvertACC_ustepss_To_usteptt(SysConfig.MotorValveConfig.Acceleration));//AMAX V1-VMAX  500
	TMC5160_SPIWriteInt(0x27, Convert_usteps_To_ustept(SysConfig.MotorValveConfig.MaxSpeed));		//VMAX   200*ustep*RPM/60
	TMC5160_SPIWriteInt(0x28, ConvertACC_ustepss_To_usteptt(SysConfig.MotorValveConfig.Deceleration));	//DMAX VMAX-V1 700
	TMC5160_SPIWriteInt(0x2A, ConvertACC_ustepss_To_usteptt(SysConfig.MotorValveConfig.Deceleration));//D1 V1-VSTOP速度模式不用配置
	TMC5160_SPIWriteInt(0x2B, Convert_usteps_To_ustept(SysConfig.MotorValveConfig.MinSpeed));					//VSTOP
	//TMC5160_SPIWriteInt(0x2B, 	Convert_usteps_To_ustept(SysConfig.MotorValveConfig.MaxSpeed));

	TMC5160_SPIWriteInt(0x33, 0x00000000); 		// writing value 0x00000000 = 0 = 0.0 to address 25 = 0x33(VDCMIN)
	TMC5160_SPIWriteInt(0x34, 0x00000400); 		// writing value 0x00000400 = 1024 = 0.0 to address 26 = 0x34(SW_MODE)

	TMC5160_SPIWriteInt(0x60, 0xAAAAB554); 		// writing value 0xAAAAB554 = 0 = 0.0 to address 31 = 0x60(MSLUT[0])
	TMC5160_SPIWriteInt(0x61, 0x4A9554AA); 	// writing value 0x4A9554AA = 1251300522 = 0.0 to address 32 = 0x61(MSLUT[1])
	TMC5160_SPIWriteInt(0x62, 0x24492929); 		// writing value 0x24492929 = 608774441 = 0.0 to address 33 = 0x62(MSLUT[2])
	TMC5160_SPIWriteInt(0x63, 0x10104222); 		// writing value 0x10104222 = 269500962 = 0.0 to address 34 = 0x63(MSLUT[3])
	TMC5160_SPIWriteInt(0x64, 0xFBFFFFFF); 		// writing value 0xFBFFFFFF = 0 = 0.0 to address 35 = 0x64(MSLUT[4])
	TMC5160_SPIWriteInt(0x65, 0xB5BB777D); 		// writing value 0xB5BB777D = 0 = 0.0 to address 36 = 0x65(MSLUT[5])
	TMC5160_SPIWriteInt(0x66, 0x49295556); 	// writing value 0x49295556 = 1227445590 = 0.0 to address 37 = 0x66(MSLUT[6])
	TMC5160_SPIWriteInt(0x67, 0x00404222); 		// writing value 0x00404222 = 4211234 = 0.0 to address 38 = 0x67(MSLUT[7])
	TMC5160_SPIWriteInt(0x68, 0xFFFF8056); 		// writing value 0xFFFF8056 = 0 = 0.0 to address 39 = 0x68(MSLUTSEL)
	TMC5160_SPIWriteInt(0x69, 0x00F70000); 	// writing value 0x00F70000 = 16187392 = 0.0 to address 40 = 0x69(MSLUTSTART)

	TMC5160_SPIWriteInt(0x6C, 0x10410153); 		// writing value 0x00410153 = 4260179 = 0.0 to address 41 = 0x6C(CHOPCONF)
	TMC5160_SPIWriteInt(0x6D, 0x00030000); 		// 电流下降速率0,最小电流: 1/2 电流设置(IRUN)
	TMC5160_SPIWriteInt(0x6E, 0x00000000); 		// writing value 0x00000000 = 0 = 0.0 to address 43 = 0x6E(DCCTRL)
	//TMC5160_SPIWriteInt(0x70, 	0xC40C001E); 		// writing value 0xC40C001E = 0 = 0.0 to address 44 = 0x70(PWMCONF)
	TMC5160_SPIWriteInt(0x70, 0xC40C2f1f);

	Motor_SetMicrostep(SysConfig.MotorValveConfig.MicroStep);
	Motor_SetPhaseCurrent(SysConfig.MotorValveConfig.PhaseCurrent);
	Motor_SetDirection(SysConfig.MotorValveConfig.PositionDir);
#if TMC5160_STOP_MODE
	TMC5160_SPIWriteInt(0x34, 0x00000082);			//REFR高电平停止电机, 朝着有效电平跳变时位置锁存
	TMC_EXT_STOP();
#endif
}

void TMC5160_Init(void)
{
	// 复位驱动器
	TMC5160_WriteRegister(TMC5160_REG_GCONF, 0x00000000);
	HAL_Delay(10);

	// 配置驱动器模式
//	TMC5160_WriteRegister(TMC5160_REG_GCONF, 0x00000008); // 使用SPI控制

	// 配置电机参数
	TMC5160_WriteRegister(TMC5160_REG_CHOPCONF, (MRRS_32 << 24) | // MRES=0x4 (1/16微步)
			(0x3 << 15) | // TBL=0x3 (空白时间)
			(0x3 << 0)); // TOFF=0x1 (斩波使能)

	// 配置电流
	TMC5160_WriteRegister(TMC5160_REG_IHOLD_IRUN, (12 << 8) | (5 << 0));  // // IRUN=12, IHOLD=5

	// 配置速度和加速度
	TMC5160_WriteRegister(TMC5160_REG_TPOWERDOWN, 10); // 待机电流延时
	TMC5160_WriteRegister(TMC5160_REG_TPWMTHRS, 500);  // 速度阈值
	TMC5160_WriteRegister(TMC5160_REG_VSTART, 10);     // 起始速度
	TMC5160_WriteRegister(TMC5160_REG_A1, 1000);       // 加速度
	TMC5160_WriteRegister(TMC5160_REG_V1, 50000);       // 加速度阈值速度
	TMC5160_WriteRegister(TMC5160_REG_AMAX, 500);     // 最大加速度
	TMC5160_WriteRegister(TMC5160_REG_VSTOP, 10);     // 最大加速度
	TMC5160_WriteRegister(TMC5160_REG_VMAX, 200000);    // 最大速度
	TMC5160_WriteRegister(TMC5160_REG_D1, 1400);        // 减速度
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
