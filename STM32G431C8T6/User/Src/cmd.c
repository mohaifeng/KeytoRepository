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
#include <string.h>
#include <ctype.h>

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

static void Response_KTOEM_Config(ResponseHandle_t *resp_data)
{
	Usart_TX_t tx_stu;
	char pdata_byte[12];
	uint8_t data_size_byte = 0;  //将32位数据转为ascii码时字节个数
	tx_stu.tx_buffer[0] = 0x55;
	tx_stu.tx_buffer[1] = resp_data->idex;
	tx_stu.tx_buffer[2] = sysconfig.CommunicationConfig.Add;
	tx_stu.tx_buffer[3] = resp_data->state;
	if (resp_data->is_data)
	{
		tx_stu.tx_buffer[4] = 0;
		for (uint8_t i = 0; i < resp_data->buff_size; i++)
		{
			switch (resp_data->res_buff[i].val_type)
			{
				case REG_U8:
					sprintf(pdata_byte, "%u", (uint8_t) resp_data->res_buff[i].value.u8v);
					break;
				case REG_U16:
					sprintf(pdata_byte, "%u", (uint16_t) resp_data->res_buff[i].value.u16v);
					break;
				case REG_U32:
					sprintf(pdata_byte, "%lu", (uint32_t) resp_data->res_buff[i].value.u32v);
					break;
				case REG_I8:
					sprintf(pdata_byte, "%d", (int8_t) resp_data->res_buff[i].value.i8v);
					break;
				case REG_I16:
					sprintf(pdata_byte, "%d", (int16_t) resp_data->res_buff[i].value.i16v);
					break;
				case REG_I32:
					sprintf(pdata_byte, "%ld", (int32_t) resp_data->res_buff[i].value.i32v);
					break;
				case REG_FLOAT:
					sprintf(pdata_byte, "%.2f", (float) resp_data->res_buff[i].value.fv);
					break;
				default:
					break;
			}
			data_size_byte = strlen(pdata_byte);
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
	if (resp_data->port_num == 1)
	{
		Usart_TxBuffer_Append(&huart1, &tx_stu);
	}
	if (resp_data->port_num == 2)
	{
		Usart_TxBuffer_Append(&huart2, &tx_stu);
	}

}
static void Response_KTDT_Config(ResponseHandle_t *resp_data)
{

}
static void Response_KTOEMSameIdex_Config(ResponseHandle_t *resp_data)
{
	Usart_TX_t tx_stu;
	if (resp_data->port_num == 1)
	{
		memcpy(&tx_stu, &urt1_tx_stu.tx_buff_list[abs(urt1_tx_stu.head - 1) % TX_LIST_MAX], sizeof(Usart_TX_t));
		Usart_TxBuffer_Append(&huart1, &tx_stu);
	}
	if (resp_data->port_num == 2)
	{
		memcpy(&tx_stu, &urt2_tx_stu.tx_buff_list[abs(urt2_tx_stu.head - 1) % TX_LIST_MAX], sizeof(Usart_TX_t));
		Usart_TxBuffer_Append(&huart2, &tx_stu);
	}
}
void UART_ConfigReturnData(ResponseHandle_t *resp_data)
{

	switch (resp_data->protocol)
	{
		case PROT_KT_OEM_SAMEIDEX:
			Response_KTOEMSameIdex_Config(resp_data);
			break;
		case PROT_KT_OEM:
			Response_KTOEM_Config(resp_data);
			break;
		case PROT_KT_DT:
			Response_KTDT_Config(resp_data);
			break;
		default:
			break;
	}
	if (resp_data->port_num == 1)
		usart1_state = USART_SENDING;
	if (resp_data->port_num == 2)
		usart2_state = USART_SENDING;
}

static void Reverse_String(uint8_t *str, uint8_t length) //将字符数组顺序反转
{
	uint8_t start = 0;
	uint8_t end = length - 1;
	while (start < end)
	{
		uint8_t temp = str[start];
		str[start] = str[end];
		str[end] = temp;
		start++;
		end--;
	}
}

uint32_t Int_to_Ascii(int32_t num, uint8_t *buffer)
{
	uint32_t buf_size = 0;
	uint8_t is_negative = 0;
	if (num < 0)
	{
		is_negative = 1;
		num = -num;
	}
// 处理 0 的情况
	if (num == 0)
	{
		buffer[buf_size++] = '0';
	}
// 逐位提取数字（反向存储）
	while (num != 0)
	{
		buffer[buf_size++] = '0' + (num % 10);
		num /= 10;
	}
// 如果是负数，添加 '-'
	if (is_negative)
	{
		buffer[buf_size] = '-';
	}
// 反转字符串（因为数字是反向存储的）
	Reverse_String(buffer, buf_size);
	return buf_size;
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
	if (pcmd->protocol == PROT_KT_OEM_SAMEIDEX)
	{
		return;
	}
	if (strcmp((char*) pcmd->cmd, "Rr") == 0)
	{
		Valve_ReadRegister(pcmd, resp);
		return;
	}

}

