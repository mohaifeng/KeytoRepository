/*
 * cmd.c
 *
 *  Created on: May 27, 2025
 *      Author: 莫海峰
 */
#include "main.h"
#include "cmd.h"
#include "usart.h"
#include "protocol.h"
#include <string.h>
#include <ctype.h>
#include "objectdirectory.h"
#include "register.h"

DEV_STATUSTYPEDEF cmd_finish_flag;
void Fun_Init()
{

}

void Cmd_Response(UART_HandleTypeDef *huart, ProtocolType protocol_type, uint8_t *pdata, uint8_t datalen)
{
	if (huart->Instance == USART1)
	{
		switch (protocol_type)
		{
			case PROTOCOL_DT:
				dt_struct.addr = SysConfig.addr;
				dt_struct.dt_flag = 0x3C;
				dt_struct.state = cmd_finish_flag;
				dt_struct.cmd_len = datalen;
				if (datalen)
				{
					memcpy(dt_struct.data_buff, pdata, datalen);
				}
				Usart_SendData(huart, &dt_struct);
				break;
			case PROTOCOL_OEM:
				oem_struct.head = 0x55;
				oem_struct.addr = SysConfig.addr;
				oem_struct.state = cmd_finish_flag;
				oem_struct.cmd_len = datalen;
				if (datalen)
				{
					memcpy(oem_struct.data_buff, pdata, datalen);
				}
				Usart_SendData(huart, &oem_struct);
				break;
			default:
				break;
		}
	}
}

HAL_StatusTypeDef ControlCmd_LegalCheck_Callback(UART_HandleTypeDef *huart, uint16_t idex, const uint8_t *cmd_buff,
		uint8_t buff_size)
{
	uint8_t separator = ',';
	uint32_t num = 0;
	uint8_t subidex = 0;

	for (uint8_t i = CMD_LEN; i < buff_size; i++)
	{
		if (*(cmd_buff + i) == separator)
		{
			if (*(cmd_buff + i - 1) != separator)
			{
				if (OD_Write(idex, subidex, num) != HAL_OK)
				{
					Cmd_Response(huart, protocol_type, NULL, 0);
					return HAL_ERROR;
				}
			}
			num = 0;
			subidex++;
		}
		else
		{
			num = num * 10 + (*(cmd_buff + i) - 0x30);
		}
	}
	Cmd_Response(huart, protocol_type, NULL, 0);
	return HAL_OK;
}

//指令数据处理函数
void Cmd_Data_Config(UART_HandleTypeDef *huart, const uint8_t *cmd_buff, uint8_t buff_size)
{
	uint8_t separator = ',';
	uint32_t num = 0;
	uint8_t subidex = 0;
	if (cmd_buff == NULL || buff_size == 0)
	{
		return;
	}

	if (*cmd_buff == 'I' && *(cmd_buff + 1) == 't')
	{
		if (ControlCmd_LegalCheck_Callback(huart, 0x4000, cmd_buff, buff_size) == HAL_OK)
		{
			Fun_Init();
		}
	}
	if (*cmd_buff == 'W' && *(cmd_buff + 1) == 'r')
	{
		for (uint8_t i = CMD_LEN; i < buff_size; i++)
		{
			if (*(cmd_buff + i) == separator)
			{
				subidex = num;
				num = 0;
			}
			else
			{
				num = num * 10 + (*(cmd_buff + i) - 0x30);
			}
		}
		OD_Write(0x2000, subidex, num);
		Cmd_Response(huart, protocol_type, NULL, 0);
		return;

	}
	if (*cmd_buff == 'R' && *(cmd_buff + 1) == 'r')
	{
		for (uint8_t i = CMD_LEN; i < buff_size; i++)
		{
			if (*(cmd_buff + i) == separator)
			{
				subidex = num;
				num = 0;
			}
			else
			{
				num = num * 10 + (*(cmd_buff + i) - 0x30);
			}
		}
		if (subidex == 0)
		{
			subidex = num;
		}
		int32_t data = 0;
		if (OD_Read(0x2000, subidex, &data) == HAL_OK)
		{
			uint8_t ascii_buf[PROTOCOL_DATA_LEN] = { 0 };
			uint32_t datalen = Int_to_Ascii(data, ascii_buf);
			Cmd_Response(huart, protocol_type, ascii_buf, datalen);
			return;
		}
		else
		{
			Cmd_Response(huart, protocol_type, NULL, 0);
			return;
		}
	}
	if (*cmd_buff == 'S')
	{
		if (Save_Register() == HAL_OK)
		{
			Cmd_Response(huart, protocol_type, NULL, 0);
			return;
		}
	}
	cmd_finish_flag = INVALID_CMD;
	Cmd_Response(huart, protocol_type, NULL, 0);
}

HAL_StatusTypeDef Cmd_Execute()
{
	return HAL_OK;
}

