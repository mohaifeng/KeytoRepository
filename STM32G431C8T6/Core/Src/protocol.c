/*
 * protocol.c
 *
 *  Created on: Mar 26, 2025
 *      Author: 莫海峰
 */
#include "protocol.h"
#include "main.h"
#include "string.h"
#include "usart.h"
#include "verification.h"

OEM_TYPEDEF oem_struct; //oem结构体变量
DT_TYPEDEF dt_struct; //dt结构体变量
volatile ProtocolType protocol_type = PROTOCOL_NULL;
//功能：检查接收数据是否符合DT协议规范，符合写入dt_struct结构体变量，返回1，否则返回0
uint8_t DT_Rxdata_Analyze(const uint8_t *rx_buff, uint16_t len)
{
	uint16_t rx_flag_idex = 0;
	if (rx_buff[len - 1] == 0x0D)
	{
		for (uint16_t i = 0; i < len - 2; i++)
		{
			if (rx_buff[i] == 0x3E)
			{
				protocol_type = PROTOCOL_DT;
				rx_flag_idex = i;
				break;
			}
		}
		if (rx_flag_idex)
		{
			dt_struct.cmd_len = len - rx_flag_idex - 2;
			dt_struct.dt_flag = rx_buff[rx_flag_idex];
			if (rx_flag_idex > 1)
			{
				dt_struct.addr = (rx_buff[0] - 0x30) * 10 + (rx_buff[1] - 0x30);
			}
			else
			{
				dt_struct.addr = rx_buff[0] - 0x30;
			}
			for (uint8_t i = 0; i < dt_struct.cmd_len; i++)
			{
				dt_struct.data_buff[i] = rx_buff[rx_flag_idex + 1 + i];
			}
			return 1;
		}

	}
	protocol_type = PROTOCOL_NULL;
	return 0;
}

//功能：检查接收数据是否符合oem/DT协议规范，，符合写入相应结构体变量，将协议类型置相应位
void Rxdata_Analyze(const uint8_t *rx_buff, uint16_t len)
{
	const uint8_t *tmp_dt_pdata = rx_buff;
	const uint8_t *tmp_oem_pdata = rx_buff;
	uint8_t tmp_flag = 0;
	uint8_t start_data_idex = 0;
	DT_TYPEDEF tmp_dt_stu = { 0 };
	OEM_TYPEDEF tmp_oem_stu = { 0 };
	//DT协议解析
	for (uint16_t i = 0; i < len; i++) //找到0x3E
	{
		if ((*tmp_dt_pdata == 0x3E) && (~tmp_flag))
		{
			if (SysConfig.addr > 9)
			{
				if (i > 1)
				{
					tmp_dt_stu.addr = (*(tmp_dt_pdata - 1) - 0x30) + (*(tmp_dt_pdata - 2) - 0x30) * 10;
				}
			}
			else
			{
				tmp_dt_stu.addr = (*(tmp_dt_pdata - 1) - 0x30);
			}
			if (tmp_dt_stu.addr == SysConfig.addr)
			{
				tmp_flag = 1;
				start_data_idex = i;
			}
		}
		if ((*tmp_dt_pdata == 0x0D) && (tmp_flag))
		{
			tmp_dt_stu.cmd_len = i - start_data_idex - 1;
			memcpy(tmp_dt_stu.data_buff, tmp_dt_pdata - tmp_dt_stu.cmd_len, tmp_dt_stu.cmd_len);
			memcpy(&dt_struct, &tmp_dt_stu, sizeof(DT_TYPEDEF));
			protocol_type = PROTOCOL_DT;
			tmp_flag = 0;
			return;
		}
		else
		{
			tmp_dt_pdata++;
		}
	}
	//OEM协议解析
	for (uint16_t i = 0; i < len; i++) //找到0xAA帧头
	{
		if ((*tmp_oem_pdata) == 0xAA)
		{
			tmp_oem_stu.idex = *(tmp_oem_pdata + 1);
			tmp_oem_stu.addr = *(tmp_oem_pdata + 2);
			tmp_oem_stu.cmd_len = *(tmp_oem_pdata + 3);
			if (tmp_oem_stu.cmd_len < len - i - 5)
			{
				if (*(tmp_oem_pdata + 4 + tmp_oem_stu.cmd_len) == Checksum_8(tmp_oem_pdata, 4 + tmp_oem_stu.cmd_len))
				{
					if (tmp_oem_stu.addr == SysConfig.addr)
					{
						if (tmp_oem_stu.idex == oem_struct.idex)
						{
							protocol_type = PROTOCOL_IdexSame;
							return;
						}
						if (tmp_oem_stu.idex > 0x80)
						{
							tmp_oem_stu.checksum = *(tmp_oem_pdata + 4 + tmp_oem_stu.cmd_len);
							memcpy(tmp_oem_stu.data, tmp_oem_pdata + 4, tmp_oem_stu.cmd_len);
							memcpy(&oem_struct, &tmp_oem_stu, sizeof(OEM_TYPEDEF));
							protocol_type = PROTOCOL_OEM;
							return;
						}
					}

				}
			}
		}
		else
		{
			tmp_oem_pdata++;
		}
	}
	protocol_type = PROTOCOL_NULL;
}

//协议解析,通过返回1，否则返回0，
uint8_t Protocol_Analyze(const uint8_t *rx_buff, uint16_t len)
{
	if (len > BUFFER_SIZE)
	{
		len = BUFFER_SIZE;
	}
	Rxdata_Analyze(rx_buff, len);
	switch (protocol_type)
	{
		case PROTOCOL_DT:
				return 1;
		case PROTOCOL_OEM:
				return 1;
		case PROTOCOL_IdexSame:

		default:
			return 0;
	}
	return 0;
}

static void Hex_To_Ascii_Byte(uint8_t num, uint8_t *output)
{
	if (num >= 100)
	{
		output[0] = '0' + (num / 100);        // 百位
		output[1] = '0' + ((num / 10) % 10);  // 十位
		output[2] = '0' + (num % 10);        // 个位
	}
	else if (num >= 10)
	{
		output[0] = '0' + (num / 10);        // 十位
		output[1] = '0' + (num % 10);        // 个位
	}
	else
	{
		output[0] = '0' + num;               // 个位
	}
}

void Send_Data_Conf(UART_HandleTypeDef *uartHandle, const void *data_struct)
{
	uint8_t ascii_arr[3] = { 0 };
	uint8_t count = 0;
	uint8_t *pdata_1 = usart1_tx_struct.tx_buffer;
	if (uartHandle->Instance == USART1)
	{
		if (protocol_type == PROTOCOL_DT)
		{
			DT_TYPEDEF *dt_str = (DT_TYPEDEF*) data_struct;
			Hex_To_Ascii_Byte(dt_str->addr, ascii_arr);
			for (uint8_t i = 0; i < 3; i++)
			{
				if (ascii_arr[i] != 0)
				{
					*pdata_1++ = ascii_arr[i];
					count++;
				}
				else
				{
					memset(ascii_arr, 0, 3);  //清空
					break;
				}

			}
			*pdata_1++ = dt_str->dt_flag;
			Hex_To_Ascii_Byte(dt_str->state, ascii_arr);
			for (uint8_t i = 0; i < 3; i++)
			{
				if (ascii_arr[i] != 0)
				{
					*pdata_1++ = ascii_arr[i];
					count++;
				}
				else
				{
					memset(ascii_arr, 0, 3);  //清空
					break;
				}
			}
			if (dt_str->cmd_len > 0)
			{
				*pdata_1++ = 0x3A;
				count++;
				for (uint8_t i = 0; i < dt_str->cmd_len; i++)
				{
					*pdata_1++ = dt_str->data_buff[i];
				}
			}
			*pdata_1 = 0x0D;
			usart1_tx_struct.tx_len = count + dt_str->cmd_len + 3;
		}
		else if (protocol_type == PROTOCOL_OEM)
		{
			OEM_TYPEDEF *oem_str = (OEM_TYPEDEF*) data_struct;
			*pdata_1++ = oem_str->head;
			*pdata_1++ = oem_str->idex;
			*pdata_1++ = oem_str->addr;
			*pdata_1++ = oem_str->state;
			*pdata_1++ = oem_str->cmd_len;
			count = 5;
			if (oem_str->cmd_len)
			{
				for (uint8_t i = 0; i < oem_str->cmd_len; i++)
				{
					*pdata_1++ = oem_str->data[i];
					count++;
				}
			}
			oem_str->checksum = Checksum_8(oem_str->data, count);
			*pdata_1 = oem_str->checksum;
			count++;
			usart1_tx_struct.tx_len = count;
		}
	}
}
