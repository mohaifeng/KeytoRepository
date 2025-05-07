#include "protocol.h"

OEM_TYPEDEF oem_send_data_instance; //发送数据oem结构体变量
OEM_TYPEDEF oem_rec_data_instance; //接收到数据oem结构体变量
DT_TYPEDEF dt_send_data_instance; //发送数据dt结构体变量
DT_TYPEDEF dt_rec_data_instance; //接收到数据dt结构体变量
volatile uint8_t protocol_type = 0; //0:DT;1:OEM
extern uint8_t rx_buffer[BUFFER_SIZE];  // 接收缓冲区
extern uint8_t tx_buffer[BUFFER_SIZE];  // 输出缓冲区

//功能：检查接收数据是否符合DT协议规范，符合写入dt_rec_data_instance结构体变量，返回1，否则返回0
uint8_t DT_Rec_Conf(uint8_t *rx_buff,uint32_t len)
{
	uint32_t rx_flag_idex = 0;
	if(rx_buff[len - 1] == 0x0D)
	{
		for(uint32_t i = 0; i < len - 2; i++)
		{
			if(rx_buff[i] == 0x3E)
			{
				rx_flag_idex = i;
				break;
			}
		}
		if(rx_flag_idex)
		{
			dt_rec_data_instance.data_len = len;
			dt_rec_data_instance.dt_flag = rx_buff[rx_flag_idex];
			if(rx_flag_idex > 1)
			{
				dt_rec_data_instance.addr = (rx_buff[0] - 0x30) * 10 + (rx_buff[1] - 0x30);
			}
			else
			{
				dt_rec_data_instance.addr = rx_buff[0] - 0x30;
			}
			for(uint32_t i = 0; i < len - rx_flag_idex - 2; i++)
			{
				dt_rec_data_instance.data[i] = rx_buff[rx_flag_idex + 1 + i];
			}
			return 1;
		}

	}
	return 0;
}

//功能：检查接收数据是否符合oem协议规范，符合写入oem_rec_data_instance结构体变量，返回1，否则返回0
uint8_t OEM_Rec_Conf(uint8_t *rx_buff,uint32_t len)
{
	if(rx_buff[0] == 0xAA)
	{
		if(rx_buff[len - 1] == Checksum_8(rx_buff,len - 1))
		{
			oem_rec_data_instance.data_len = len;
			oem_rec_data_instance.head = rx_buff[0];
			oem_rec_data_instance.idex = rx_buff[1];
			oem_rec_data_instance.addr = rx_buff[2];
			oem_rec_data_instance.cmd_len = rx_buff[3];
			for(uint8_t i = 0; i < oem_rec_data_instance.cmd_len; i++)
			{
				oem_rec_data_instance.data[i] = rx_buff[i + 4];
			}
			return 1;
		}
	}
	return 0;
}

void DT_Send_Conf(void)
{
	uint8_t tmp_buff[BUFFER_SIZE] = { 0 };
	uint8_t *buf_p = tmp_buff;
	tmp_buff[0] = (dt_send_data_instance.addr / 10) + 0x30;
	tmp_buff[1] = (dt_send_data_instance.addr % 10) + 0x30;
	tmp_buff[2] = dt_send_data_instance.dt_flag;
	tmp_buff[3] = dt_send_data_instance.state;
	tmp_buff[4] = 0x0D;
	if(tmp_buff[0] == 0x30)
	{
		buf_p = tmp_buff + 1;
	}
	for(uint8_t i = 0; i < dt_send_data_instance.data_len; i++)
	{
		tx_buffer[i] = buf_p[i];
	}
}

//将发送oem数据结构体转为数组
void OEM_Send_Conf(void)
{
	tx_buffer[0] = oem_send_data_instance.head;
	tx_buffer[1] = oem_send_data_instance.idex;
	tx_buffer[2] = oem_send_data_instance.addr;
	tx_buffer[3] = oem_send_data_instance.state;
	tx_buffer[4] = oem_send_data_instance.cmd_len;
	if(oem_send_data_instance.cmd_len > 0)
	{
		for(uint8_t i = 0; i < oem_rec_data_instance.cmd_len; i++)
		{
			tx_buffer[i + 5] = oem_send_data_instance.data[i];
		}
	}
	tx_buffer[5 + oem_send_data_instance.cmd_len] = Checksum_8(tx_buffer,4 + oem_send_data_instance.cmd_len);
}
//协议解析
void Protocol_Analyze(uint8_t *rx_buff,uint8_t len)
{
	if(len > PROTOCOL_DATA_LEN)
	{
		len = PROTOCOL_DATA_LEN;
	}
	if(OEM_Rec_Conf(rx_buff,len))
	{
		protocol_type = 1;
		oem_send_data_instance.head = 0x55;
		oem_send_data_instance.idex = oem_rec_data_instance.idex;
		oem_send_data_instance.addr = oem_rec_data_instance.addr;
		oem_send_data_instance.state = 0;
		oem_send_data_instance.cmd_len = 0;
		oem_send_data_instance.data_len = 6;
		OEM_Send_Conf();
	}
	else
	{
		oem_send_data_instance.data_len = 0;
	}
	if(DT_Rec_Conf(rx_buff,len))
	{
		protocol_type = 0;
		dt_send_data_instance.addr = dt_rec_data_instance.addr;
		dt_send_data_instance.dt_flag = 0x3C;
		dt_send_data_instance.state = 2 + 0x30;
		dt_send_data_instance.data_len = 4;
		DT_Send_Conf();
	}
	else
	{
		dt_send_data_instance.data_len = 0;
	}
}
