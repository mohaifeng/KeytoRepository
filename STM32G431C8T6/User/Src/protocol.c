/*
 * protocol.c
 *
 *  Created on: Mar 26, 2025
 *      Author: 莫海峰
 */
#include "user_usart.h"
#include "protocol.h"
#include <string.h>
#include "usart.h"
#include "cmd.h"
#include "dev.h"
#include "verification.h"

RealTimeResolution_t usart1_rtimeresol_stu; //串口1实时解析缓存数组
RealTimeResolution_t usart2_rtimeresol_stu; //串口2实时解析缓存数组
uint8_t resolution_flag = 0; //解析成功标志位
//清除实时解析数组
void Clear_RealtimeResolution_Buff(UART_HandleTypeDef *huart)
{
	RealTimeResolution_t *tmp_stu;
	if (huart->Instance == USART1)
	{
		tmp_stu = &usart1_rtimeresol_stu;
	}
	else if (huart->Instance == USART2)
	{
		tmp_stu = &usart2_rtimeresol_stu;
	}
	else
		return;
	memset(tmp_stu->rsl_buff, 0, RESOLUTION_BUFF_MAX);
	tmp_stu->datalen = 0;
	tmp_stu->head = 0;
	tmp_stu->tail = 0;
}

//将接收到的数据加到实时解析数组当中
void RealtimeResolutionBuff_Append(UART_HandleTypeDef *huart, uint8_t *data, uint8_t len)
{
	uint8_t buff_siez = len;
	RealTimeResolution_t *tmp;
	if (huart->Instance == USART1)
	{
		tmp = &usart1_rtimeresol_stu;
	}
	else if (huart->Instance == USART2)
	{
		tmp = &usart2_rtimeresol_stu;
	}
	else
		return;
	while (buff_siez--)
	{
		tmp->rsl_buff[tmp->tail % RESOLUTION_BUFF_MAX] = *data;
		tmp->tail++;
		data++;
	}
	tmp->datalen = (tmp->datalen + len) % RESOLUTION_BUFF_MAX;

}
//协议解析
void ResolutionProtocol(UART_HandleTypeDef *huart)
{
	uint8_t i = 0;
	RealTimeResolution_t *tmp;
	if (huart->Instance == USART1)
	{
		tmp = &usart1_rtimeresol_stu;
	}
	else if (huart->Instance == USART2)
	{
		tmp = &usart2_rtimeresol_stu;
	}
	else
		return;
	while (i < tmp->datalen)
	{
		if (tmp->rsl_buff[i] == 0xAA) //OEM协议
		{
			if (UART_ResolutionKT_OEM_Protocol(huart, &tmp->rsl_buff[i], tmp->datalen - i) == HAL_OK)
			{
				resolution_flag++;
			}
		}
		else if (tmp->rsl_buff[i] >= 0x30 && tmp->rsl_buff[i] <= 0x39) //DT协议
		{
			if (UART_ResolutionKT_DT_Protocol(huart, &tmp->rsl_buff[i], tmp->datalen - i) == HAL_OK)
			{
				resolution_flag++;
			}
		}
		i++;
	}
	if (resolution_flag > 0)
	{
		Clear_RealtimeResolution_Buff(huart);
		resolution_flag = 0;
	}
}

HAL_StatusTypeDef UART_ResolutionKT_OEM_Protocol(UART_HandleTypeDef *huart, uint8_t *data, uint8_t len)
{
	HAL_StatusTypeDef Resolution_flag = HAL_ERROR;
	KT_OEM_Handle_t protocol;
	Cmd_Par_t cmd_par;
	if (len > 5)
	{
		protocol.idex = data[1];
		protocol.add = data[2];
		protocol.cmd_len = data[3];
		if (protocol.idex < 0x80 || protocol.add != sysconfig.CommunicationConfig.Add || (protocol.cmd_len + 5) > len)
			return Resolution_flag;
		memcpy(protocol.cmd, &data[4], protocol.cmd_len);
		protocol.checksum_8 = data[4 + protocol.cmd_len];
		if (protocol.checksum_8 != Checksum_8(data, protocol.cmd_len + 4))
			return Resolution_flag;
		Resolution_flag = HAL_OK;
		cmd_par.idex = protocol.idex;
		if (huart->Instance == USART1)
		{
			cmd_par.port_num = 1;
			if (cmd_par.idex == usart1_idex_stu.idex)
			{
				Usart_TxBuffer_Append(&usart1_idex_stu.tx_buff);
				return Resolution_flag;
			}
			else
			{
				cmd_par.protocol = PROT_KT_OEM;
			}
		}
		if (huart->Instance == USART2)
		{
			cmd_par.port_num = 2;
			if (cmd_par.idex == usart2_idex_stu.idex)
			{
				Usart_TxBuffer_Append(&usart2_idex_stu.tx_buff);
				return Resolution_flag;
			}
			else
			{
				cmd_par.protocol = PROT_KT_OEM;
			}
		}
		Cmd_Config(&cmd_par, protocol.cmd, protocol.cmd_len);
		Cmd_List_Append(&cmd_par);
	}
	return Resolution_flag;
}

HAL_StatusTypeDef UART_ResolutionKT_DT_Protocol(UART_HandleTypeDef *huart, uint8_t *data, uint8_t len)
{
	uint8_t separator_idex = 0;
	uint8_t end_idex = 0;
	KT_DT_Handle_t protocol;
	Cmd_Par_t cmd_par;
	for (uint8_t i = 0; i < len; i++)
	{
		if (data[i] == 0x3E)
		{
			separator_idex = i;
		}
		else if (data[i] == 0x0D)
		{
			end_idex = i;
		}
	}
	if (!(separator_idex && end_idex && (separator_idex >= end_idex)))
		return HAL_ERROR;
	protocol.add = 0;
	for (uint8_t i = 0; i < separator_idex; i++)
	{
		protocol.add = (protocol.add * 10) + (data[i] - 0x30);
	}
	if (protocol.add != sysconfig.CommunicationConfig.Add)
		return HAL_ERROR;
	protocol.cmd_len = end_idex - separator_idex - 1;
	memcpy(protocol.cmd, &data[separator_idex + 1], protocol.cmd_len);
	Cmd_Config(&cmd_par, protocol.cmd, protocol.cmd_len);
	cmd_par.protocol = PROT_KT_DT;
	if (huart->Instance == USART1)
		cmd_par.port_num = 1;
	else if (huart->Instance == USART2)
		cmd_par.port_num = 2;
	Cmd_List_Append(&cmd_par);
	return HAL_OK;
}
