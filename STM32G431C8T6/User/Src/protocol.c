/*
 * protocol.c
 *
 *  Created on: Mar 26, 2025
 *      Author: 莫海峰
 */
#include "protocol.h"
#include <string.h>
#include "user_usart.h"
#include "cmd.h"
#include "dev.h"
#include "verification.h"

RealTimeResolution_t usart1_rtimeresol_stu;
RealTimeResolution_t usart2_rtimeresol_stu;
//OemIdex_t oem_idex = { .now_oem_idex = 0, .old_oem_idex = 0 };
volatile uint8_t old_oem_idex = 0;

void Clear_RealtimeResolution_Buff(UART_HandleTypeDef *huart)
{
	if (huart->Instance == USART1)
	{
		memset(usart1_rtimeresol_stu.rsl_buff, 0, RESOLUTION_BUFF_MAX);
		usart1_rtimeresol_stu.datalen = 0;
		usart1_rtimeresol_stu.end_idex = 0;
	}
	if (huart->Instance == USART2)
	{
		memset(usart2_rtimeresol_stu.rsl_buff, 0, RESOLUTION_BUFF_MAX);
		usart2_rtimeresol_stu.datalen = 0;
		usart2_rtimeresol_stu.end_idex = 0;
	}
}

void RealtimeResolutionBuff_Append(UART_HandleTypeDef *huart, uint8_t *data, uint8_t len)
{
	uint8_t buff_siez = len;
	if (huart->Instance == USART1)
	{
		while (buff_siez--)
		{
			usart1_rtimeresol_stu.rsl_buff[usart1_rtimeresol_stu.end_idex % RESOLUTION_BUFF_MAX] = *data;
			usart1_rtimeresol_stu.end_idex++;
			data++;
		}
		usart1_rtimeresol_stu.datalen = (usart1_rtimeresol_stu.datalen + len) % RESOLUTION_BUFF_MAX;
	}
	if (huart->Instance == USART2)
	{
		while (buff_siez--)
		{
			usart2_rtimeresol_stu.rsl_buff[usart2_rtimeresol_stu.end_idex % RESOLUTION_BUFF_MAX] = *data;
			usart2_rtimeresol_stu.end_idex++;
			data++;
		}
		usart2_rtimeresol_stu.datalen = (usart2_rtimeresol_stu.datalen + len) % RESOLUTION_BUFF_MAX;
	}
}

void ResolutionProtocol(UART_HandleTypeDef *huart)
{
	uint8_t i = 0;
	if (huart->Instance == USART1)
	{
		while (i < usart1_rtimeresol_stu.datalen)
		{
			if (usart1_rtimeresol_stu.rsl_buff[i] == 0xAA) //OEM协议
			{
				UART_ResolutionKT_OEM_Protocol(huart, &usart1_rtimeresol_stu.rsl_buff[i], usart1_rtimeresol_stu.datalen - i);
			}
			if (usart1_rtimeresol_stu.rsl_buff[i] >= 0x30 && usart1_rtimeresol_stu.rsl_buff[i] <= 0x39) //DT协议
			{
				UART_ResolutionKT_DT_Protocol(huart, &usart1_rtimeresol_stu.rsl_buff[i], usart1_rtimeresol_stu.datalen - i);
			}
			i++;
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
	}

}

HAL_StatusTypeDef UART_ResolutionKT_OEM_Protocol(UART_HandleTypeDef *huart, uint8_t *data, uint8_t len)
{
	HAL_StatusTypeDef Resolution_flag = HAL_ERROR;
	KT_OEM_Handle_t protocol;
	Cmd_Par_t cmd_par;
	if (len < 6)
		return Resolution_flag;
	for (uint8_t i = 0; i < len; i++)
	{
		if ((data[i] == 0xAA) && ((len - i) > 5))
		{
			protocol.idex = data[i + 1];
			if (protocol.idex < 0x80)
				continue;
			protocol.add = data[i + 2];
			if (protocol.add != sysconfig.CommunicationConfig.Add)
				continue;
			protocol.cmd_len = data[i + 3];
			if (protocol.cmd_len + i + 5 > len)
				continue;
			memcpy(protocol.cmd, &data[i + 4], protocol.cmd_len);
			protocol.checksum_8 = data[i + 4 + protocol.cmd_len];
			if (protocol.checksum_8 != Checksum_8(&data[i], protocol.cmd_len))
				continue;
			Cmd_Config(&cmd_par, protocol.cmd, protocol.cmd_len);
			cmd_par.protocol = PROT_KT_OEM;
			cmd_par.idex = protocol.idex;
			if (huart->Instance == USART1)
				cmd_par.port_num = 1;
			else if (huart->Instance == USART2)
				cmd_par.port_num = 2;
			Cmd_List_Append(&cmd_par);
			Resolution_flag = HAL_OK;
		}
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
