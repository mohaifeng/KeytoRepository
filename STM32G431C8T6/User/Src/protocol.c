/*
 * protocol.c
 *
 *  Created on: Mar 26, 2025
 *      Author: 莫海峰
 */
#include "protocol.h"
#include <string.h>
#include "user_usart.h"
#include "usart.h"
#include "cmd.h"
#include "dev.h"
#include "verification.h"

RealTimeResolution_t usart1_rtimeresol_stu;
RealTimeResolution_t usart2_rtimeresol_stu;
//OemIdex_t oem_idex = { .now_oem_idex = 0, .old_oem_idex = 0 };
volatile uint8_t old_oem_idex = 0;
uint8_t resolution_flag = 0;
//清除实时解析数组
void Clear_RealtimeResolution_Buff(UART_HandleTypeDef *huart)
{
	if (huart->Instance == USART1)
	{
		memset(usart1_rtimeresol_stu.rsl_buff, 0, RESOLUTION_BUFF_MAX);
		usart1_rtimeresol_stu.datalen = 0;
		usart1_rtimeresol_stu.head = 0;
		usart1_rtimeresol_stu.tail = 0;
	}
	if (huart->Instance == USART2)
	{
		memset(usart2_rtimeresol_stu.rsl_buff, 0, RESOLUTION_BUFF_MAX);
		usart2_rtimeresol_stu.datalen = 0;
		usart2_rtimeresol_stu.head = 0;
		usart2_rtimeresol_stu.tail = 0;
	}
}
//将接收到的数据加到实时解析数组当中
void RealtimeResolutionBuff_Append(UART_HandleTypeDef *huart, uint8_t *data, uint8_t len)
{
	uint8_t buff_siez = len;
	if (huart->Instance == USART1)
	{
		while (buff_siez--)
		{
			usart1_rtimeresol_stu.rsl_buff[usart1_rtimeresol_stu.tail % RESOLUTION_BUFF_MAX] = *data;
			usart1_rtimeresol_stu.tail++;
			data++;
		}
		usart1_rtimeresol_stu.datalen = (usart1_rtimeresol_stu.datalen + len) % RESOLUTION_BUFF_MAX;
	}
	if (huart->Instance == USART2)
	{
		while (buff_siez--)
		{
			usart2_rtimeresol_stu.rsl_buff[usart2_rtimeresol_stu.tail % RESOLUTION_BUFF_MAX] = *data;
			usart2_rtimeresol_stu.tail++;
			data++;
		}
		usart2_rtimeresol_stu.datalen = (usart2_rtimeresol_stu.datalen + len) % RESOLUTION_BUFF_MAX;
	}
}
//协议解析
void ResolutionProtocol(UART_HandleTypeDef *huart)
{
	uint8_t i = 0;

	if (huart->Instance == USART1)
	{
		while (i < usart1_rtimeresol_stu.datalen)
		{
			if (usart1_rtimeresol_stu.rsl_buff[i] == 0xAA) //OEM协议
			{
				if (UART_ResolutionKT_OEM_Protocol(huart, &usart1_rtimeresol_stu.rsl_buff[i], usart1_rtimeresol_stu.datalen - i)
						== HAL_OK)
				{
					resolution_flag++;
				}
			}
			if (usart1_rtimeresol_stu.rsl_buff[i] >= 0x30 && usart1_rtimeresol_stu.rsl_buff[i] <= 0x39) //DT协议
			{
				if (UART_ResolutionKT_DT_Protocol(huart, &usart1_rtimeresol_stu.rsl_buff[i], usart1_rtimeresol_stu.datalen - i)
						== HAL_OK)
				{
					resolution_flag++;
				}
			}
			i++;
		}
		if (resolution_flag > 0)
		{
			Clear_RealtimeResolution_Buff(&huart1);
			resolution_flag = 0;
		}
		else
		{
			usart1_state = USART_RECEIVING;
		}

	}
	if (huart->Instance == USART2)
	{
		while (i < usart2_rtimeresol_stu.datalen)
		{
			if (usart2_rtimeresol_stu.rsl_buff[i] == 0xAA) //OEM协议
			{
				UART_ResolutionKT_OEM_Protocol(huart, &usart2_rtimeresol_stu.rsl_buff[i], usart2_rtimeresol_stu.datalen - i);
			}
			if (usart2_rtimeresol_stu.rsl_buff[i] >= 0x30 && usart2_rtimeresol_stu.rsl_buff[i] <= 0x39) //DT协议
			{
				UART_ResolutionKT_DT_Protocol(huart, &usart2_rtimeresol_stu.rsl_buff[i], usart2_rtimeresol_stu.datalen - i);
			}
			i++;
		}
		Clear_RealtimeResolution_Buff(&huart2);
		usart2_state = USART_RECEIVING;
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
			return Resolution_flag;;
		memcpy(protocol.cmd, &data[4], protocol.cmd_len);
		protocol.checksum_8 = data[4 + protocol.cmd_len];
		if (protocol.checksum_8 != Checksum_8(data, protocol.cmd_len + 4))
			return Resolution_flag;;
		Cmd_Config(&cmd_par, protocol.cmd, protocol.cmd_len);
		cmd_par.idex = protocol.idex;
		if (cmd_par.idex == old_oem_idex)
		{
			cmd_par.protocol = PROT_KT_OEM_SAMEIDEX;
		}
		else
		{
			old_oem_idex = cmd_par.idex;
			cmd_par.protocol = PROT_KT_OEM;
		}
		if (huart->Instance == USART1)
			cmd_par.port_num = 1;
		if (huart->Instance == USART2)
			cmd_par.port_num = 2;
		Cmd_List_Append(&cmd_par);
		Resolution_flag = HAL_OK;
	}
	return Resolution_flag;
}

HAL_StatusTypeDef UART_ResolutionKT_DT_Protocol(UART_HandleTypeDef *huart, uint8_t *data, uint8_t len)
{
	return HAL_OK;
}
//HAL_StatusTypeDef UART_ResolutionGeneral_Protocol(uint8_t *data, uint8_t len)
//{
//	uint8_t sum = 0;
//	int32_t value = 0;
//	GeneralProtocol_Handle_t protocol;
//	Cmd_Par_t temp;
//	if (len < 8)
//		return HAL_ERROR;
//	protocol.head = data[0];
//	if (protocol.head != 0xAA)
//		return HAL_ERROR;
//	protocol.add = data[1];
//	if (protocol.add != sysconfig.CommunicationConfig.Add)
//		return HAL_ERROR;
//	protocol.cmd = data[2];
//	memcpy(protocol.data, &(data[3]), 4);
//	value = ((uint32_t) (protocol.data[0]) << 24) | ((uint32_t) (protocol.data[1]) << 16)
//			| ((uint32_t) (protocol.data[2]) << 8) | ((uint32_t) (protocol.data[3]));
//	protocol.verify = data[7];
//	sum = Checksum_8(data, 7);
//	if (sum != protocol.verify)
//		return HAL_ERROR;
//	temp.cmd = protocol.cmd;
//	temp.value = value;
//	Cmd_List_Append(temp);
//	return HAL_OK;
//}
