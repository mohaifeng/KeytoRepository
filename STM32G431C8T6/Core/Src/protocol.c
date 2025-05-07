/*
 * protocol.c
 *
 *  Created on: Mar 26, 2025
 *      Author: 莫海峰
 */
#include "protocol.h"

OEM_TYPEDEF oem_struct; //oem结构体变量
DT_TYPEDEF dt_struct; //dt结构体变量
volatile uint8_t protocol_type = 0; //-1:未通过协议解析;0:DT;1:OEM
volatile uint8_t protocol_pass_flag = 0; //协议解析成功标志位
extern USART_RX_TYPEDEF usart_rx_struct;
extern DEV_TYPEDEF dev_msg;
//功能：检查接收数据是否符合DT协议规范，符合写入dt_struct结构体变量，返回1，否则返回0
uint8_t DT_Rec_Conf(const uint8_t *rx_buff,uint8_t len)
{
	uint8_t rx_flag_idex = 0;
	if(rx_buff[len - 1] == 0x0D)
	{
		for(uint8_t i = 0; i < len - 2; i++)
		{
			if(rx_buff[i] == 0x3E)
			{
				protocol_type = 0;
				rx_flag_idex = i;
				break;
			}
		}
		if(rx_flag_idex)
		{
			dt_struct.data_buff_len = len - rx_flag_idex - 2;
			dt_struct.dt_flag = rx_buff[rx_flag_idex];
			if(rx_flag_idex > 1)
			{
				dt_struct.addr = (rx_buff[0] - 0x30) * 10 + (rx_buff[1] - 0x30);
			}
			else
			{
				dt_struct.addr = rx_buff[0] - 0x30;
			}
			for(uint8_t i = 0; i < dt_struct.data_buff_len; i++)
			{
				dt_struct.data_buff[i] = rx_buff[rx_flag_idex + 1 + i];
			}
			return 1;
		}

	}
	return 0;
}

//功能：检查接收数据是否符合oem协议规范，符合写入oem_struct结构体变量，返回1，否则返回0
uint8_t OEM_Rec_Conf(const uint8_t *rx_buff,uint8_t len)
{
	if((rx_buff[0] == 0xAA) && (rx_buff[len - 1] == Checksum_8(rx_buff,len - 1)))
	{
		protocol_type = 1;
		oem_struct.data_len = len;
		oem_struct.head = rx_buff[0];
		oem_struct.idex = rx_buff[1];
		oem_struct.addr = rx_buff[2];
		oem_struct.cmd_len = rx_buff[3];
		for(uint8_t i = 0; i < oem_struct.cmd_len; i++)
		{
			oem_struct.data[i] = rx_buff[i + 4];
		}
		return 1;
	}
	return 0;
}

//协议解析
void Protocol_Analyze(const uint8_t *rx_buff,uint8_t len)
{
	if(len > BUFFER_SIZE)
	{
		len = BUFFER_SIZE;
	}
	if(OEM_Rec_Conf(rx_buff,len) || DT_Rec_Conf(rx_buff,len))
	{
		switch(protocol_type)
			{
			case 0:
				if(dt_struct.addr == dev_msg.addr)
				{
					protocol_pass_flag = 1;
				}
				else
				{
					protocol_pass_flag = 0;
				}
				break;
			case 1:
				if(oem_struct.addr == dev_msg.addr)
				{
					protocol_pass_flag = 1;
				}
				else
				{
					protocol_pass_flag = 0;
				}
				break;
			default:
				protocol_pass_flag = 0;
				break;
			}
	}
	else
	{
		protocol_pass_flag = 0;
	}
}

