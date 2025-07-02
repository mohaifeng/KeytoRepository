#include "protocol.h"
#include "main.h"
#include <string.h>
#include "usart.h"
#include "verification.h"

OEM_TYPEDEF oem_struct; //oem结构体变量
DT_TYPEDEF dt_struct; //dt结构体变量
ProtocolType protocol_type = PROTOCOL_NULL;

//功能：检查接收数据是否符合oem/DT协议规范，，符合写入相应结构体变量，将协议类型置相应位
void Rxdata_Analyze(const uint8_t *rx_buff, uint16_t len)
{
	const uint8_t *tmp_dt_pdata = rx_buff;
	const uint8_t *tmp_oem_pdata = rx_buff;
	uint8_t tmp_flag = 0; //找到地址标志
	uint8_t start_data_idex = 0;
	DT_TYPEDEF tmp_dt_stu = { 0 };
	OEM_TYPEDEF tmp_oem_stu = { 0 };
	//DT协议解析
	for (uint16_t i = 0; i < len; i++) //找到0x3E
	{
		if ((*tmp_dt_pdata == 0x3E) && (~tmp_flag))
		{
			if (SysConfig.addr > 99)
			{
				if (i > 2)
				{
					tmp_dt_stu.addr = (*(tmp_dt_pdata - 1) - 0x30) + (*(tmp_dt_pdata - 2) - 0x30) * 10
							+ (*(tmp_dt_pdata - 3) - 0x30) * 100;
				}
				else
				{
					tmp_dt_stu.addr = 0;
				}
			}
			else if (SysConfig.addr > 9)
			{
				if (i > 1)
				{
					tmp_dt_stu.addr = (*(tmp_dt_pdata - 1) - 0x30) + (*(tmp_dt_pdata - 2) - 0x30) * 10;
				}
				else
				{
					tmp_dt_stu.addr = 0;
				}
			}
			else
			{
				if (i > 0)
				{
					tmp_dt_stu.addr = (*(tmp_dt_pdata - 1) - 0x30);
				}
				else
				{
					tmp_dt_stu.addr = 0;
				}
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
			if (len - i > 4 + tmp_oem_stu.cmd_len)
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
							memcpy(tmp_oem_stu.data_buff, tmp_oem_pdata + 4, tmp_oem_stu.cmd_len);
							memcpy(&oem_struct, &tmp_oem_stu, sizeof(OEM_TYPEDEF));
							protocol_type = PROTOCOL_OEM;
							return;
						}
					}
					return;
				}
			}
		}
		tmp_oem_pdata++;
	}
	protocol_type = PROTOCOL_NULL;
}

static void Reverse_String(uint8_t *str, uint8_t length)
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

void Send_Data_Conf(UART_HandleTypeDef *huart, const void *data_struct)
{
	uint8_t ascii_arr[ADDR_MAX_LEN] = { 0 };
	uint8_t count = 0;
	uint8_t *pdata_1 = usart1_tx_struct.tx_buffer;
	if (huart->Instance == USART1)
	{
		if (protocol_type == PROTOCOL_DT)
		{
			DT_TYPEDEF *dt_str = (DT_TYPEDEF*) data_struct;
			uint8_t num = Int_to_Ascii(dt_str->addr, ascii_arr);
			for (uint8_t i = 0; i < num; i++)
			{
				*pdata_1++ = ascii_arr[i];
				count++;
			}
			memset(ascii_arr, 0, num);  //清空
			*pdata_1++ = dt_str->dt_flag;
			count++;
			num = Int_to_Ascii(dt_str->state, ascii_arr);
			for (uint8_t i = 0; i < num; i++)
			{

				*pdata_1++ = ascii_arr[i];
				count++;
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
			count++;
			usart1_tx_struct.tx_len = count + dt_str->cmd_len;
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
					*pdata_1++ = oem_str->data_buff[i];
					count++;
				}
			}
			oem_str->checksum = Checksum_8(oem_str->data_buff, count);
			*pdata_1 = oem_str->checksum;
			count++;
			usart1_tx_struct.tx_len = count;
		}
	}
}
