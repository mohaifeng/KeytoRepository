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
#include "protocol.h"

#define SYS_FACILITY_ID			  0X00010011
#define SYS_SOFT_VERSIONS		  230927118 //SYSTERM_VERSION
#define ENC_SPEED_ARRAY_SIZE  16
#define Max_PortNum						30

#define USART_TASK_TIME						5 //单位ms,等待xms无新数据则开始解析
#define VALVE_CONTROL_TASK_TIME		5 //单位ms
#define CONSOLE_CONTROL_TASK_TIME	2

typedef enum
{
	CONSOLE_IDLE,
	CONSOLE_READY,
	CONSOLE_WAIT,
} Console_Status_t;

typedef enum
{
	DEV_IDLE = 0, //空闲
	DEV_BUSY, //忙
	EXECUTE_SUCCESS, //执行成功
	COMPLETE, //上报完成
	OVER_LIMIT = 10, //超限
	PARAMETER_ERROR, //参数错误
	SYNTAX_ERROR, //语法错误
	INVALID_CMD, //非法指令
	REG_ERROR, //寄存器地址错误
	READ_WRITE_ONLY, //只读/写
	CMD_OVERFLOW, //指令溢出
	NO_INIT, //未初始化
	DRIVER_ERROR = 51,
	OW1_ERROR,
	OW2_ERROR,
	SENSOR_ERROR,
	FLASH_ERROR,
	OEM_IDEX_SAME = 255,
} Sys_Status_t;

typedef struct
{
	uint8_t Port;                //通道编号
	int32_t MotorPosition;       		 //电机位置
} Valve_Port_t;

typedef struct
{
	uint8_t InitPort;       		 //复位通道
	uint16_t InitSpeed;                              //复位速度（单位 转/分）  5-350转/分
	uint32_t OptocouplerGapStep;             //光耦挡片缺口宽度
	uint32_t OptPosition[Max_PortNum];              	//位置光耦触发位置
	uint32_t OptInterval;										//位置光耦间隔
	uint8_t OptGapNum;											//光耦触发缺口个数
	Valve_Port_t Port_Buff[Max_PortNum];             //每个端口数组
	uint8_t PortNum;                       //通道个数
	uint8_t ErrorAutoResetAllowCont;
	int32_t ClockwiseCompensationStep;    	//顺时针补偿值
	int32_t AnticlockwiseCompensationStep; //逆时针补偿值
	uint16_t MaxStep;	//行程
	uint8_t MaxGapStep;
} Valve_Config_t;

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
	Protocol_t protocol_type;     //协议类型
	uint32_t RS232_BaudRate;      //RS232波特率
	uint32_t RS485_BaudRate;      //RS485波特率
	uint16_t CAN_BaudRate;        //CAN波特率
	uint8_t CAN_TargetAdd;        //CAN目标地址
} Communication_Config_t;

typedef struct
{
	uint8_t IntoBootMode;        //进入下载模式
	Sys_Status_t RunStatus;
	uint32_t InitFlag;			//第一次上电
	uint8_t WorkMode;      //电机工作模式，0:步进电机微步模式，1：流体控制模式
	uint8_t AutoResetZeroEn;
	uint8_t AutoAgingEnable;
	Valve_Config_t ValveConfig;
	StepMotorPara_t MotorValveConfig;
	Communication_Config_t CommunicationConfig;   //通讯配置
	EncoderPara_t EncoderConfig;			//编码器配置
	uint32_t Password;
	uint8_t IIC_Addr;
	uint8_t i2c_valve_profile;
	uint8_t i2c_valvecmd;
	uint8_t UserData[50];
	uint32_t model;
	uint32_t version;
} SysConfig_t;

extern SysConfig_t sysconfig;
extern uint32_t USART1_RxTimeCnt;														//串口1接收数据的时间计数
extern uint32_t USART2_RxTimeCnt;														//串口2接收数据的时间计数
extern uint32_t ValveControlTaskCnt;														//旋转阀控制任务时间计数
extern Console_Status_t console1_status;
extern Console_Status_t console2_status;

void ConfigInit(void);
void SysTick_Callback(void);
void ConsoleControlTask(UART_HandleTypeDef *huart);

#endif /* INC_DEV_H_ */
