/*
 * dev.h
 *
 *  Created on: Aug 12, 2025
 *      Author: 莫海峰
 */

#ifndef INC_DEV_H_
#define INC_DEV_H_

#include "main.h"
#include "tmc5160.h"

#define SYS_FACILITY_ID			0X00010011
#define SYS_SOFT_VERSIONS		230927118 //SYSTERM_VERSION
#define ENC_SPEED_ARRAY_SIZE  16

typedef struct
{
	uint8_t Port;                //通道编号
	int32_t MotorPosition;       		 //电机位置
} Valve_Port_t;

typedef struct
{
	uint8_t ResetPort;
	uint16_t ResetSpeed;                              //复位速度（单位 转/分）  5-350转/分
	uint32_t FWD_ValveOptocouplerGapStep;             //光耦挡片缺口宽度
	uint32_t OptPosition[30];              	//位置光耦触发位置
	uint32_t OptInterval;										//位置光耦间隔
	uint8_t OptGapNum;											//光耦触发缺口个数
	Valve_Port_t Port[30];                 //端口
	uint8_t PortNum;                       //端口个数
	uint8_t ErrorAutoResetAllowCont;
	int32_t FlowClockwisePhaseCompensation;    	//顺时针补偿值
	int32_t FlowAnticlockwisePhaseCompensation; //逆时针补偿值
	uint8_t Using_ReductionBox;
	float Reduction_Ratio;
	uint16_t MaxStep;
	uint8_t MaxGapStep;
} Flow_Config_t;

typedef struct
{
	uint16_t PulseNumber; /*!< Number of pulses per revolution, provided by each of the two encoder signals, multiplied by 4 */
	uint8_t Direction;						//编码器方向
	uint8_t EncoderEn;						//是否启用编码器
} EncoderPara_t;

typedef struct
{
	uint8_t ContDir;
	int32_t EncAvrSpeed;									//编码器平均速度,单位为cont/s,其中cont为编码器计数值
	/* HW Settings */
	uint16_t TimerOverflowNb; /*!< Number of overflows occurred since
	 last speed measurement event*/
	int32_t PreviousCapture; /*!< Timer counter value captured during
	 previous speed measurement event*/
	int32_t DeltaCapturesBuffer[ENC_SPEED_ARRAY_SIZE]; /*!< Buffer used to store	 captured variations of timer counter*/

	volatile uint8_t DeltaCapturesIndex; /*! <Buffer index*/
} ENCODER_Handle_t;

typedef struct
{
	uint8_t Add;                  //地址
	uint8_t RS232_BaudRate;      //RS232波特率
	uint8_t RS485_BaudRate;      //RS485波特率
	uint16_t CAN_BaudRate;        //CAN波特率
	uint8_t CAN_TargetAdd;        //CAN目标地址
} Communication_Config_t;

typedef struct
{
	uint8_t IntoBootMode;        //进入下载模式
	uint32_t InitFlag;
	uint8_t WorkMode;      //电机工作模式，0:步进电机微步模式，1：流体控制模式
	uint8_t AutoResetZeroEn;
	uint8_t AutoAgingEnable;
	Flow_Config_t ValveConfig;
	StepMotorPara_t MotorValveConfig;
	Communication_Config_t CommunicationConfig;   //通讯配置
	EncoderPara_t EncoderConfig;			//编码器配置
	uint32_t Password;
	uint8_t Not_Change;														//不适用装换10通模式
	uint8_t IIC_Addr;
	uint8_t i2c_valve_profile;
	uint8_t i2c_valvecmd;
	uint8_t UserData[50];
} SysConfig_t;

extern SysConfig_t sysconfig;

#endif /* INC_DEV_H_ */
