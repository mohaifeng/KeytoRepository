/*
 * cmd.c
 *
 *  Created on: May 27, 2025
 *      Author: 莫海峰
 */
#include "main.h"
#include "cmd.h"
#include "usart.h"
#include "valve_control.h"
#include "verification.h"
#include <stdio.h>
#include <stdlib.h>

Cmd_list_t cmd_list;
//将命令字符串写入到命令参数结构体中方便后续调用
HAL_StatusTypeDef Cmd_Config(Cmd_Par_t *cmd_buf, uint8_t *pdata, uint8_t len)
{
	uint8_t count = 0;
	int32_t num = 0;
	memcpy(cmd_buf->cmd, pdata, CMD_LEN); //命令传到结构体
	cmd_buf->cmd[CMD_LEN] = '\0';
	len -= CMD_LEN;
	for (uint8_t i = 0; i < len; i++)
	{
		if (pdata[i + CMD_LEN] == 0x2C)
		{
			cmd_buf->value_buf[count++] = num;
			num = 0;
		}
		else
		{
			num = num * 10 + (pdata[i + CMD_LEN] - 0x30);
		}
	}
	cmd_buf->value_buf[count++] = num;
	cmd_buf->value_num = count;
	return HAL_OK;
}
//清除命令数组
void Cmd_List_Clear(void)
{
	cmd_list.head = 0;
	cmd_list.tail = 0;
	cmd_list.num = 0;
	for (uint8_t i = 0; i < CMD_LIST_MAX; i++)
	{
		memset(cmd_list.cmd_buff[i].cmd, 0, sizeof(cmd_list.cmd_buff[i].cmd));
		memset(cmd_list.cmd_buff[i].value_buf, 0, sizeof(cmd_list.cmd_buff[i].value_buf));
		cmd_list.cmd_buff[i].value_num = 0;
	}
}
//将命令加入到命令数组中
HAL_StatusTypeDef Cmd_List_Append(Cmd_Par_t *cmd)
{
	if (cmd_list.num >= CMD_LIST_MAX)
		return HAL_ERROR;
	memcpy(&cmd_list.cmd_buff[cmd_list.tail % CMD_LIST_MAX], cmd, sizeof(Cmd_Par_t));
	cmd_list.tail = (cmd_list.tail + 1) % CMD_LIST_MAX;
	cmd_list.num = (cmd_list.num + 1) % CMD_LIST_MAX;
	return HAL_OK;
}

static void RegInt_To_Ascii(char *char_buf, RegValue_Handle_t *pdata)
{
	switch (pdata->val_type)
	{
		case REG_U8:
			sprintf(char_buf, "%u", (uint8_t) pdata->value.u8v);
			break;
		case REG_U16:
			sprintf(char_buf, "%u", (uint16_t) pdata->value.u16v);
			break;
		case REG_U32:
			sprintf(char_buf, "%lu", (uint32_t) pdata->value.u32v);
			break;
		case REG_I8:
			sprintf(char_buf, "%d", (int8_t) pdata->value.i8v);
			break;
		case REG_I16:
			sprintf(char_buf, "%d", (int16_t) pdata->value.i16v);
			break;
		case REG_I32:
			sprintf(char_buf, "%ld", (int32_t) pdata->value.i32v);
			break;
		default:
			break;
	}
}

static void Response_KTOEM_Config(ResponseHandle_t *resp_data)
{
	Usart_TX_t tx_stu;
	char pdata_byte[12];
	uint8_t data_size_byte = 0;  //将32位数据转为ascii码时字节个数
	tx_stu.port = resp_data->port_num;
	tx_stu.tx_buffer[0] = 0x55;
	tx_stu.tx_buffer[1] = resp_data->idex;
	tx_stu.tx_buffer[2] = sysconfig.CommunicationConfig.Add;
	tx_stu.tx_buffer[3] = resp_data->state;
	if (resp_data->is_data)
	{
		tx_stu.tx_buffer[4] = 0;
		for (uint8_t i = 0; i < resp_data->buff_size; i++)
		{
			RegInt_To_Ascii(pdata_byte, &resp_data->res_buff[i]);
			data_size_byte = strlen(pdata_byte);  //数据ascii码长度
			memcpy(&tx_stu.tx_buffer[5 + tx_stu.tx_buffer[4]], pdata_byte, data_size_byte);
			tx_stu.tx_buffer[4] = tx_stu.tx_buffer[4] + data_size_byte;
			if (i != resp_data->buff_size - 1)
			{
				tx_stu.tx_buffer[4 + tx_stu.tx_buffer[4] + 1] = ',';
				tx_stu.tx_buffer[4]++;
			}
		}
	}
	else
	{
		tx_stu.tx_buffer[4] = 0;
	}
	tx_stu.tx_len = 5 + tx_stu.tx_buffer[4];
	tx_stu.tx_buffer[tx_stu.tx_len] = Checksum_8(tx_stu.tx_buffer, tx_stu.tx_len);
	tx_stu.tx_len++;
	if (tx_stu.port == 1)
	{
		usart1_idex_stu.idex = resp_data->idex;
		memcpy(&usart1_idex_stu.tx_buff, &tx_stu, sizeof(Usart_TX_t));
	}
	if (tx_stu.port == 2)
	{
		usart2_idex_stu.idex = resp_data->idex;
		memcpy(&usart2_idex_stu.tx_buff, &tx_stu, sizeof(Usart_TX_t));
	}
	Usart_TxBuffer_Append(&tx_stu);

}
static void Response_KTDT_Config(ResponseHandle_t *resp_data)
{
	Usart_TX_t tx_stu;
	char tmp_char[12];
	uint8_t head = 0;
	uint8_t num = 0;
	tx_stu.port = resp_data->port_num;
	num = sprintf(tmp_char, "%u", (uint8_t) sysconfig.CommunicationConfig.Add);  //地址转ascii
	memcpy(&tx_stu.tx_buffer[head], tmp_char, strlen(tmp_char));
	head += num;
	tx_stu.tx_buffer[head++] = 0x3C;
	num = sprintf(tmp_char, "%u", (uint8_t) resp_data->state);
	memcpy(&tx_stu.tx_buffer[head], tmp_char, strlen(tmp_char));
	head += num;
	if (resp_data->is_data)
	{
		tx_stu.tx_buffer[head++] = 0x3A;
		for (uint8_t i = 0; i < resp_data->buff_size; i++)
		{
			RegInt_To_Ascii(tmp_char, &resp_data->res_buff[i]);
			num = strlen(tmp_char);  //数据ascii码长度
			memcpy(&tx_stu.tx_buffer[head], tmp_char, num);
			head += num;
			if (i < resp_data->buff_size - 1)
			{
				tx_stu.tx_buffer[head++] = ',';
			}
		}
	}
	tx_stu.tx_buffer[head] = 0x0D;
	tx_stu.tx_len = head + 1;
	if (tx_stu.port == 1)
	{
		memcpy(&usart1_idex_stu.tx_buff, &tx_stu, sizeof(Usart_TX_t));
	}
	else if (tx_stu.port == 2)
	{
		memcpy(&usart2_idex_stu.tx_buff, &tx_stu, sizeof(Usart_TX_t));
	}
	Usart_TxBuffer_Append(&tx_stu);
}

void UART_ConfigReturnData(ResponseHandle_t *resp_data)
{
	switch (resp_data->protocol)
	{
		case PROT_KT_OEM:
			Response_KTOEM_Config(resp_data);
			break;
		case PROT_KT_DT:
			Response_KTDT_Config(resp_data);
			break;
		default:
			break;
	}
}

void Cmd_List_Execute(void)
{
	ResponseHandle_t resp_data;
	if (cmd_list.num == 0)
		return;
	Resolution_Cmd(&cmd_list.cmd_buff[cmd_list.head], &resp_data);
	UART_ConfigReturnData(&resp_data);
	cmd_list.head = (cmd_list.head + 1) % CMD_LIST_MAX;
	cmd_list.num--;
}
//执行命令并将应答数据写入到应答结构体
void Resolution_Cmd(const Cmd_Par_t *pcmd, ResponseHandle_t *resp)
{
	resp->port_num = pcmd->port_num;
	resp->protocol = pcmd->protocol;
	resp->idex = pcmd->idex;
	if (strcmp((char*) pcmd->cmd, "Rr") == 0)
	{
		Valve_ReadRegister(pcmd, resp);
		return;
	}
	else if (strcmp((char*) pcmd->cmd, "Wr") == 0)
	{
		Valve_WriteRegister(pcmd, resp);
		return;
	}
	else if (strcmp((char*) pcmd->cmd, "Sm") == 0)
	{
		Valve_SpeedMove(pcmd, resp);
		return;
	}
	else if (strcmp((char*) pcmd->cmd, "Rt") == 0)
	{
		Valve_Stop(pcmd, resp);
		return;
	}
}

