/*
 * valve_control.c
 *
 *  Created on: Aug 14, 2025
 *      Author: 莫海峰
 */
#include "valve_control.h"
#include "register.h"
#include "led.h"
#include "dev.h"

Valve_Handle_t ValveControl =
		{
				.ValveState = VALVE_IDLE,
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

void Valve_SpeedMove(const Cmd_Par_t *pcmd, ResponseHandle_t *resp)
{
	if (pcmd->value_num > 2)
	{
		resp->state = PARAMETER_ERROR;
		resp->is_data = 0;
		return;
	}
	Motor_SpeedMove(pcmd->value_buf[0], pcmd->value_buf[1]);
	resp->state = EXECUTE_SUCCESS;
	resp->is_data = 0;
}

void Valve_Stop(const Cmd_Par_t *pcmd, ResponseHandle_t *resp)
{
	if (pcmd->value_num > 1)
	{
		resp->state = PARAMETER_ERROR;
		resp->is_data = 0;
		return;
	}
	if (pcmd->value_buf[0] == 1)
		Motor_SoftStop();
	else
		Motor_HardStop();
	resp->state = EXECUTE_SUCCESS;
	resp->is_data = 0;
}

void ValveControlTask(void)
{

}

/*************************************************
 设置阀下一个运行状态
 *************************************************/
void Valve_NextState(ValveState_t bState)
{
	if (ValveControl.ZeroMarkFlag != 1 && (bState == VALVE_START || bState == VALVE_DIR_START || bState == VALVE_RUN))
		return;
	ValveControl.ValveState = bState;
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

void Valve_ReadRegister(const Cmd_Par_t *pcmd, ResponseHandle_t *resp)
{
	Sys_Status_t status;
	RegValue_Handle_t resp_data;
	resp->buff_size = 0;
	switch (pcmd->value_num)
	{
		case 1:
			if (pcmd->value_buf[0] > 0xFF || pcmd->value_buf[0] < 0)
			{
				resp->state = OVER_LIMIT;
				resp->is_data = 0;
				return;
			}
			status = Read_Register(pcmd->value_buf[0], &resp_data);
			if (status != EXECUTE_SUCCESS)
			{
				resp->state = status;
				resp->is_data = 0;
				return;
			}
			resp->res_buff[0] = resp_data;
			resp->buff_size = 1;
			break;
		case 2:
			if (pcmd->value_buf[0] > 0xFF || pcmd->value_buf[1] > 0xFF || pcmd->value_buf[0] < 0 || pcmd->value_buf[1] < 0)
			{
				resp->state = OVER_LIMIT;
				resp->is_data = 0;
				return;
			}
			for (uint8_t i = 0; i < pcmd->value_buf[1]; i++)
			{
				status = Read_Register(pcmd->value_buf[0] + i, &resp_data);
				if (status != EXECUTE_SUCCESS)
				{
					resp->state = status;
					resp->is_data = 0;
					return;
				}
				resp->res_buff[i] = resp_data;
				resp->buff_size++;
			}
			break;
		default:
			resp->state = PARAMETER_ERROR;
			resp->is_data = 0;
			return;
	}
	resp->state = status;
	resp->is_data = 1;
}

void Valve_WriteRegister(const Cmd_Par_t *pcmd, ResponseHandle_t *resp)
{
	if (pcmd->value_num != 2)
	{
		resp->state = PARAMETER_ERROR;
		resp->is_data = 0;
		return;
	}
	resp->state = Write_Register((uint16_t) pcmd->value_buf[0], pcmd->value_buf[1]);
	resp->is_data = 0;
}
/**********获取当前位置*************/

