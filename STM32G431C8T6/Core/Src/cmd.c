/*
 * cmd.c
 *
 *  Created on: May 27, 2025
 *      Author: 莫海峰
 */

#include "cmd.h"
#include <string.h>
#include <ctype.h>

const CMD_Typedef cmd_stu =
		{
				.cmd_init = CMD_INTI,
				.cmd_aspirate = CMD_ASPIRATE,
				.cmd_dispense = CMD_DISPENSE,
				.cmd_read_register = CMD_READ_REGISTER,
				.cmd_write_register = CMD_WRITE_REGISTER,
		};

//指令数据处理函数
void Cmd_Data_Config(const uint8_t *cmd_buff, uint8_t buff_size)
{
	CMD_Config_t cmd_par = { 0 };
	cmd_par.cmd_par_Len = 0;
	uint8_t delimiter = ',';
	if (cmd_buff == NULL || buff_size == 0)
	{
		return;
	}
	memcpy(cmd_par.cmd_arr, cmd_buff, CMD_LEN);

	uint8_t num = 0, index = 0;
	for (uint8_t i = CMD_LEN; i < buff_size; i++)
	{
		if (isdigit(cmd_buff[i]))
		{
			num = num * 10 + (cmd_buff[i] - '0');
		}
		else if (cmd_buff[i] == delimiter)
		{
			cmd_par.cmd_par[index++] = num;
			cmd_par.cmd_par_Len++;
			num = 0;
		}
	}
	cmd_par.cmd_par[index] = num; // 最后一个数字
	cmd_par.cmd_par_Len++;
	//检测参数是否符合规范，是否可以运行
	switch (*(uint16_t*) cmd_par.cmd_arr)
	{
		case *(uint16_t*) cmd_stu.cmd_init:
			break;
		case *(uint16_t*) cmd_stu.cmd_aspirate:
			break;
		case *(uint16_t*) cmd_stu.cmd_dispense:
			break;
		case *(uint16_t*) cmd_stu.cmd_read_register:
			break;
		case *(uint16_t*) cmd_stu.cmd_write_register:
			break;
	}
}

HAL_StatusTypeDef Cmd_Execute()
{
	return HAL_OK;
}

