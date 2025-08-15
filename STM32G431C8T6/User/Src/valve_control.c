/*
 * valve_control.c
 *
 *  Created on: Aug 14, 2025
 *      Author: 莫海峰
 */
#include "valve_control.h"
#include "led.h"
#include "dev.h"


Valve_Handle_t ValveControl =
		{
				.bState = VALVE_IDLE,
				.CurrentMotorPosition = 0,
				.CurrentPort = 0,
				.hFaultNow = 0,
				.TargetPositon = 0,
				.TargetPort = 0xff,
				.autoZeroMode = 0,
				.AgingTestMode = 0,
				.ZeroMarkFlag = 0,
				.MoveFinishCallBackExe = 0,
				.ErrorAutoResetCont = 0,
				.ErrorAutoResetFlag = 0,
				.ErrorResetChannel = 0,
		};

/*************************************************
 设置阀下一个运行状态
 *************************************************/
void Valve_NextState(ValveState_t bState)
{
	if (ValveControl.ZeroMarkFlag != 1 && (bState == VALVE_START || bState == VALVE_DIR_START || bState == VALVE_RUN))
		return;
	ValveControl.bState = bState;
//	ValveControlTaskCont = VALVE_CONTROL_TASK_TIME;
}

uint8_t Valve_GetZeroMark(void)
{
	return ValveControl.ZeroMarkFlag;
}

/*************************************************
 函数功能：获取阀当前错误
 *************************************************/
uint8_t Valve_GetFaultFlag(void)
{
	return ValveControl.hFaultNow;
}

void Value_ClearFault(void)
{
	ValveControl.hFaultNow = 0;
	LED_OFF();
	Valve_NextState(VALVE_IDLE);
}

/**********获取当前位置*************/
int32_t Get_CurrentPosition(void)
{
	int32_t CurrentPos;
	if (sysconfig.WorkMode == 1)
	{

	}
	else
	{
		CurrentPos = Get_MotorPosition();
	}
	return CurrentPos;
}

