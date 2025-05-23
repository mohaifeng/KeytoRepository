/*
 * protocol.c
 *
 *  Created on: Mar 26, 2025
 *      Author: 莫海峰
 */
#include "protocol.h"

OEM_TYPEDEF oem_struct; //oem结构体变量
DT_TYPEDEF dt_struct; //dt结构体变量
volatile ProtocolType protocol_type = PROTOCOL_NULL;
extern USART_RX_TYPEDEF usart1_rx_struct;
extern USART_TX_TYPEDEF usart1_tx_struct;
extern SysConfig_t SysConfig;
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
			dt_struct.data_buff_len = len - rx_flag_idex - 2;
			dt_struct.dt_flag = rx_buff[rx_flag_idex];
			if (rx_flag_idex > 1)
			{
				dt_struct.addr = (rx_buff[0] - 0x30) * 10 + (rx_buff[1] - 0x30);
			}
			else
			{
				dt_struct.addr = rx_buff[0] - 0x30;
			}
			for (uint8_t i = 0; i < dt_struct.data_buff_len; i++)
			{
				dt_struct.data_buff[i] = rx_buff[rx_flag_idex + 1 + i];
			}
			return 1;
		}

	}
	protocol_type = PROTOCOL_NULL;
	return 0;
}

//功能：检查接收数据是否符合oem协议规范，地址是否为设备地址，符合写入oem_struct结构体变量，将协议类型置相应位
uint8_t OEM_Rxdata_Analyze(const uint8_t *rx_buff, uint16_t len)
{
//	uint8_t *pdata = rx_buff;
	uint16_t data_len = 0;
	for (uint16_t i = 0; i < len; i++) //找到0xAA帧头
	{
		if ((*rx_buff) == 0xAA)
		{
			data_len = len - i;
			for (uint8_t j = 0; j < data_len; j++) //校验数据是否合法
			{
				if (*(rx_buff + j + 1) == Checksum_8(rx_buff, j + 1))
				{
					protocol_type = PROTOCOL_OEM;
					oem_struct.data_len = j + 2;
					oem_struct.head = *rx_buff;
					oem_struct.idex = *(rx_buff + 1);
					oem_struct.addr = *(rx_buff + 2);
					oem_struct.cmd_len = *(rx_buff + 3);
					for (uint8_t k = 0; k < oem_struct.cmd_len; k++)
					{
						oem_struct.data[k] = *(rx_buff + 4 + k);
					}
					return 1;
				}
			}
		}
		else
		{
			rx_buff++;
		}
	}
	protocol_type = PROTOCOL_NULL;
	return 0;
}

//协议解析
uint8_t Protocol_Analyze(const uint8_t *rx_buff, uint16_t len)
{
	if (len > BUFFER_SIZE)
	{
		len = BUFFER_SIZE;
	}
	if (OEM_Rxdata_Analyze(rx_buff, len) || DT_Rxdata_Analyze(rx_buff, len))
	{
		switch (protocol_type)
		{
			case PROTOCOL_DT:
				if (dt_struct.addr == SysConfig.addr)
				{
					return 1;
				}
			case PROTOCOL_OEM:
				if (oem_struct.addr == SysConfig.addr)
				{
					return 1;
				}
			default:
				return 0;
		}
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
			if (dt_str->data_buff_len > 0)
			{
				*pdata_1++ = 0x3A;
				count++;
				for (uint8_t i = 0; i < dt_str->data_buff_len; i++)
				{
					*pdata_1++ = dt_str->data_buff[i];
				}
			}
			*pdata_1 = 0x0D;
			usart1_tx_struct.tx_len = count + dt_str->data_buff_len + 3;
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
