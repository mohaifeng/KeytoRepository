#include "protocol.h"

OEM_TYPEDEF *oem_send_data;//发送数据oem结构体变量
OEM_TYPEDEF *oem_rec_data;//接收到数据oem结构体变量
DT_TYPEDEF *dt_send_data;//发送数据dt结构体变量
DT_TYPEDEF *dt_rec_data;//接收到数据dt结构体变量

uint8_t protocol_type = 0; //0:DT协议;1:OEM协议

extern uint8_t rx_buffer[BUFFER_SIZE];  // 接收缓冲区
extern uint8_t tx_buffer[BUFFER_SIZE];  // 输出缓冲区
//功能：检查接收数据是否符合oem协议规范，符合写入oem_rec_data结构体，返回1，否则返回0
uint8_t OEM_Conf(uint8_t *rx_buff, uint16_t len)
{
	if (rx_buff[0] == 0xAA)
	{
		if (rx_buff[len - 1] == Checksum_8(rx_buff, len - 1))
		{
			protocol_type = 1;
			oem_rec_data->data_len = len;
			oem_rec_data->head = rx_buff[0];
			oem_rec_data->idex = rx_buff[1];
			oem_rec_data->addr = rx_buff[2];
			oem_rec_data->cmd_len = rx_buff[3];
			for (uint8_t i = 0; i < oem_rec_data->cmd_len; i++)
			{
				oem_rec_data->data[i] = rx_buff[i + 4];
			}
			return 1;
		}
	}
	return 0;
}
//将发送oem数据结构体转为数组，返回数据长度
void OEM_Send_Conf(void)
{
	tx_buffer[0] = oem_send_data->head;
	tx_buffer[1] = oem_send_data->idex;
	tx_buffer[2] = oem_send_data->addr;
	tx_buffer[3] = oem_send_data->state;
	tx_buffer[4] = oem_send_data->cmd_len;
	if (oem_send_data->cmd_len > 0)
	{
		for (uint8_t i = 0; i < oem_rec_data->cmd_len; i++)
		{
			tx_buffer[i + 5] = oem_send_data->data[i];
		}
	}
	tx_buffer[5 + oem_rec_data->cmd_len] = Checksum_8(tx_buffer,
			4 + oem_rec_data->cmd_len);
}
//协议解析
void Protocol_Analyze(uint8_t *rx_buff, uint8_t len)
{
	if (len > PROTOCOL_DATA_LEN)
	{
		len = PROTOCOL_DATA_LEN;
	}
	if (OEM_Conf(rx_buff, len))
	{
		oem_send_data->head = 0x55;
		oem_send_data->idex = oem_rec_data->idex;
		oem_send_data->addr = oem_rec_data->addr;
		oem_send_data->state = 0;
		oem_send_data->cmd_len = 0;
		oem_send_data->data_len = 6;
		OEM_Send_Conf();
	}
	else
	{
		oem_send_data->data_len = 0;
	}
}
