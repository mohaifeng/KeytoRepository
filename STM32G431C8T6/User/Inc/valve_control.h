/*
 * valve_control.h
 *
 *  Created on: Aug 14, 2025
 *      Author: 莫海峰
 */

#ifndef INC_VALVE_CONTROL_H_
#define INC_VALVE_CONTROL_H_

#include "main.h"
#include "cmd.h"

#define VALVE_CATCH_GAP_NUM     	10  //挡片缺口个数
#define VALVE_MAX_STEP  					200 //VALVE full step 模式最大值

#define MIN_VALUE(x,y) 						x<y?x:y
#define MAX_VALUE(x,y) 						x>y?x:y


typedef enum
{
	VALVE_INIT = 0,
	VALVE_IDLE,
	VALVE_AUTO_ZERO,
	VALVE_AUTO_ZERO_RUN,
	VALVE_START,
	VALVE_DIR_START,
	VALVE_RUN,
	VALVE_COMPLETE,
	VALVE_DELAY_STOP,
	VALVE_STOP,
	VALVE_FAULT,
	VALVE_FAULT_IDLE
} ValveState_t;

typedef struct
{

	ValveState_t bState;
	uint32_t MotorHardStatus;			//电机硬件状态
	uint32_t MotorRAMPStatus;
	uint8_t hFaultNow;						//错误标志,0：无错误，3：位置错误，4，丢步
	uint8_t hOldFaultNow;					//旧错误标志
	uint8_t CurrentPort;          //当前端口
	uint8_t TargetPort;           //目标端口
	uint8_t PositionMoveFlag;			//位置模式移动标志
	uint8_t MoveDirection;        //移动方向,0:顺时针，1:逆时
	uint8_t OldMoveDirection;			//上一次移动方向
	int32_t OldOptTrigeMotorPosition;        //上一次光耦触发电机位置
	int32_t TriggerMotroPosition[25];
	int32_t CurrentMotorPosition;     //当前电机位置
	int32_t OldPosition;              //单位step
	int32_t StartPosition;
	int32_t TargetPositon;        //目标位置
	int32_t Move_uStep;						//触发位置光耦后移动微步
	uint8_t CurrentTriggerOptCont;				//运动到指定通道当前已经触发光耦个数
	uint8_t TargetTriggerOptCont;					//运动到指定通道目标触发光耦计数
	uint8_t CurrentOptocoupler2Cont;   		//当前位置光耦触发计数
	uint8_t StartBeforeOptocoupler2Cont;	//启动前光耦2计数值
	uint8_t OneOptTrigeFlagNoCont;        //第一个光耦触发不计数
	uint8_t StartOptTriggerFlag;					//启动时光耦触发状态
	uint8_t TriggerOptFlag;           		//转动开始后触发光耦标志
	/*****Encoder*****/
	uint16_t uCurrentSpeed;
	int32_t uTargetPos;
	uint8_t EncoderStopFlag;
	/**********************/
	uint8_t autoZeroMode;                //自动归零模式
	uint8_t ZeroMarkFlag;								//已经归零标志
	uint8_t AgingTestMode;               //老化测试模式
	void (*MoveFinishCallBackExe)(void);  //执行切换阀完成后的回调函数

	uint32_t ErrorAutoResetCont;      //错误自动复位计数
	uint8_t ErrorAutoResetFlag;       //错误复位标志
	uint8_t ErrorResetChannel;				//错误自动复位通道

	uint32_t ResetTimeCont;
} Valve_Handle_t;

void ValveControlTask(void);
void Valve_SpeedMove(const Cmd_Par_t *pcmd, ResponseHandle_t *resp);
void Valve_Stop(const Cmd_Par_t *pcmd, ResponseHandle_t *resp);
void Valve_ReadRegister(const Cmd_Par_t *pcmd, ResponseHandle_t *resp);
void Valve_WriteRegister(const Cmd_Par_t *pcmd, ResponseHandle_t *resp);
#endif /* INC_VALVE_CONTROL_H_ */
