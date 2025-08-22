/*
 * tmc5160.h
 *
 *  Created on: Mar 28, 2025
 *      Author: 莫海峰
 */

#ifndef INC_TMC5160_H_
#define INC_TMC5160_H_

#include "main.h"

#define TMC5160_CLK						12000000	//默认频率12MHz
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
#define Convert_usteps_To_ustept(usteps) ((uint64_t)usteps*0x1000000/TMC5160_CLK)
//将ustep/t速度转换为ustep/s速度,v[Hz] = v[5160] * ( fCLK[Hz]/2 / 2^23 )
#define Convert_ustept_To_usteps(ustept) (((uint64_t)ustept*TMC5160_CLK)/0x1000000)
//将ustep/s^2加减速转换为ustep/ta^2加减速，ustep acceleration a[Hz/s]= a[5160] * fCLK[Hz]^2 / (512*256) / 2^24
#define ConvertACC_usteptt_To_ustepss(usteptt) ((((uint64_t)usteptt*TMC5160_CLK*TMC5160_CLK)/((uint64_t)0x20000000000)))
//将ustep/ta^2加减速转换为ustep/s^2加减速，ustep acceleration a[Hz/s]= a[5160] * fCLK[Hz]^2 / (512*256) / 2^24
#define ConvertACC_ustepss_To_usteptt(ustepss) ((((uint64_t)ustepss*(uint64_t)0x20000000000))/(TMC5160_CLK*TMC5160_CLK))
//#define ConvertACC_ustepss_To_usteptt(ustepss) (ustepss*0.01527099483)

typedef enum
{
	POSITIONINGMODE, //位置模式
	VELOCITYMODETOPOSITIVEVMAX, //速度模式到正 VMAX
	VELOCITYMODETONEGATIVEVMAX, //速度模式至负 VMAX
	HOLDMODE, //保持模式
} RAMPMODE;

typedef struct
{
	uint16_t StopMode;								//0:HardStop,1:SoftStop
	uint16_t MicroStep;								//细分
	uint16_t StartPhaseCurrent;       //起始相电流
	uint16_t PhaseCurrent;						//额定电流
	uint16_t PhaseVoltage;						//额定电压
	uint32_t Acceleration;						//加速度
	uint32_t Deceleration;						//减速度
	uint32_t RunSpeed;								//运行速度(用来保存速度模式的运行速度)
	uint32_t MaxSpeed;								//最大速度(用来保存位置模式下的最大速度)
	uint32_t MinSpeed;								//最小速度
	uint8_t PositionDir;							//方向
	uint8_t ParaUseMicroStep;					//参数计算采用细分
	uint8_t StepClockModeEn;					//位置模式是否采用stepclock模式
} StepMotorPara_t;

// TMC5160 寄存器地址定义
#define TMC5160_REG_GCONF        0x00  // 通用配置
#define TMC5160_REG_GSTAT        0x01  // 全局状态标志
#define TMC5160_REG_IFCNT        0x02  // 接口传输计数器
#define TMC5160_REG_SLAVECONF    0x03  // SLAVEADDR
#define TMC5160_REG_IOIN         0x04  // 读取所有可用输入引脚状态
#define TMC5160_REG_OUTPUT       0x04  // 在 UART 模式下设置输出引脚极性
#define TMC5160_REG_X_COMPARE    0x05  // 位置比较寄存器用于运动控制器位置探测，位置脉冲输出在引脚SWP_DIAG1.
#define TMC5160_REG_OTP_PROG     0x06  // OTP_PROGRAM – OTP 编程
#define TMC5160_REG_OTP_READ     0x07  // OTP_READ (访问 OTP 内存结果和更新)
#define TMC5160_REG_FACTORY_CONF 0x08  // FCLKTRIM
#define TMC5160_REG_SHORT_CONF   0x09  // SHORT_CONF
#define TMC5160_REG_DRV_CONF     0x0A  // DRV_CONF
#define TMC5160_REG_GLOBALSCALER 0x0B  // 电机电流的全局缩放
#define TMC5160_REG_OFFSET_READ  0x0C  // 偏移校准结果
#define TMC5160_REG_IHOLD_IRUN   0x10  // 驱动电流控制
#define TMC5160_REG_TPOWERDOWN   0x11  // 设置电机静止后到电机开始降电流的之间的延时时间
#define TMC5160_REG_TSTEP        0x12  // 以 1 / fCLK 为单位的步进输入信号上的两个 1 / 256 微步之间的实际测量时间
#define TMC5160_REG_TPWMTHRS     0x13  // stealthChop 电压 PWM 模式的上限速度
#define TMC5160_REG_TCOOLTHRS    0x14  // 使能 coolStep 和 stallGuard 功能的下限速度(无符号)。
#define TMC5160_REG_THIGH        0x15  // stealthChop 电压 PWM 模式的上限速度
#define TMC5160_REG_RAMPMODE     0x20  // 斜坡模式
#define TMC5160_REG_XACTUAL      0x21  // 实际位置
#define TMC5160_REG_VACTUAL      0x22  // 实际速度
#define TMC5160_REG_VSTART       0x23  // 起始速度
#define TMC5160_REG_A1       		 0x24  // VSTART 和 V1 之间的加速度(无符号)
#define TMC5160_REG_V1        	 0x25  // 第一加速/减速阶段阈值速度(无符号)
#define TMC5160_REG_AMAX         0x26  // V1 和 VMAX 之间的加速度(无符号)
#define TMC5160_REG_VMAX         0x27  // 运动斜坡目标速度
#define TMC5160_REG_DMAX         0x28  // VMAX 和 V1 之间的减速度(无符号)
#define TMC5160_REG_D1           0x2A  // V1 和 VSTOP 之间的减速度(无符号)
#define TMC5160_REG_VSTOP        0x2B  // 电机停止速度
#define TMC5160_REG_TZEROWAIT    0x2C  // 定义下一次移动或方向反转开始前，下降到零速度后的等待时间
#define TMC5160_REG_XTARGET      0x2D  // 目标位置

#define TMC5160_REG_VDCMIN       0x33  // 在 VDCMIN (无符号)以上使能自动换向 dcStep
#define TMC5160_REG_SW_MODE      0x34  // 开关模式配置
#define TMC5160_REG_RAMP_STAT    0x35  // 斜坡状态和开关事件状态
#define TMC5160_REG_XLATCH       0x36  // 斜坡发生器锁存位置，在可编程开关事件时锁存 XACTUAL

#define TMC5160_REG_CHOPCONF     0x6C  // 斩波器配置

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
void TMC5160_SPIWriteInt(uint8_t address, int32_t value);
void TMC5160_Init(void);
void Motor_HardStop(void);
void Motor_SoftStop(void);
void Motor_SpeedMove(uint8_t Dir, uint32_t uSpeed);
void Motor_MovePosition(int32_t pos);
void Motor_SetDirection(uint8_t dir);
void Motor_SetMicrostep(uint16_t step);
void Motor_SetPhaseCurrent(uint16_t current);
void Motor_SetVSTART(uint32_t uspeed);
void Motor_SetA1(uint32_t uspeed);
void Motor_SetV1(uint32_t uspeed);
void Motor_SetAMAX(uint32_t uspeed);
void Motor_SetVMAX(uint32_t uspeed);
void Motor_SetDMAX(uint32_t uspeed);
void Motor_SetD1(uint32_t uspeed);
void Motor_SetVSTOP(uint32_t uspeed);
int32_t Get_MotorPosition(void);
void Set_MotorPosition(int32_t position);
int32_t Get_MotorSpeed(void);
uint32_t Get_RAMP_Status(void);
uint32_t Get_Motor_Status(void);
void Motor_ClearReachPosition_Status(void);

#endif /* INC_TMC5160_H_ */
