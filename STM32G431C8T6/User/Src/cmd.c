/*
 * cmd.c
 *
 *  Created on: May 27, 2025
 *      Author: 莫海峰
 */
#include "main.h"
#include "cmd.h"
#include "dev.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

Cmd_list_t cmd_list;

HAL_StatusTypeDef Cmd_Config(Cmd_Par_t *cmd_buf, uint8_t *pdata, uint8_t len)
{
	char *par_tmp;
	char *token;
	char *end;
	uint8_t count = 0;
	int32_t num = 0;
	memcpy(cmd_buf->cmd, pdata, CMD_LEN); //命令传到结构体
	cmd_buf->cmd[CMD_LEN] = '\0';
	par_tmp = strdup((char*) (pdata + CMD_LEN)); // 创建副本，因为strtok会修改原字符串
	if (par_tmp == NULL)
	{
		return HAL_ERROR;
	}
	token = strtok(par_tmp, ",");
	while (token != NULL && count < CMD_PAR_MAX)
	{
		// 去除token两端的空白字符
		while (*token == ' ')
			token++;  // 去除前导空格
		end = token + strlen(token) - 1;
		while (end > token && *end == ' ')
			end--;  // 去除尾部空格
		*(end + 1) = '\0';
		if (*token != '\0')
		{  // 非空字符串
			char *endptr;
			num = strtol(token, &endptr, 10);  //字符串转10进制数字
			if (*endptr == '\0')
			{  // 成功转换
				cmd_buf->value_buf[count++] = num;
			}
		}
		token = strtok(NULL, ",");
	}
	cmd_buf->value_num = count;
	free(par_tmp);
	return HAL_OK;
}

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

HAL_StatusTypeDef Cmd_List_Append(Cmd_Par_t *cmd)
{
	if (cmd_list.num >= CMD_LIST_MAX)
		return HAL_ERROR;
	memcpy(&cmd_list.cmd_buff[cmd_list.tail % CMD_LIST_MAX], cmd, sizeof(Cmd_Par_t));
	cmd_list.tail = (cmd_list.tail + 1) % CMD_LIST_MAX;
	cmd_list.num = (cmd_list.num + 1) % CMD_LIST_MAX;
	return HAL_OK;
}

void Cmd_List_Execute(void)
{
	int8_t ret = 0;
	if (cmd_list.num == 0)
		return;
	ret = Resolution_Cmd(&cmd_list.cmd_buff[cmd_list.head]);
	UART_ConfigReturnData(ret);
	cmd_list.head = (cmd_list.head + 1) % CMD_LIST_MAX;
	cmd_list.num--;
}

void Resolution_Cmd(const Cmd_Par_t *pcmd, ResponseHandle_t *resp)
{
	if ((pcmd->protocol == PROT_KT_OEM) && (pcmd->idex == old_oem_idex))
		resp->state = OEM_IDEX_SAME;
	return;
	if (strcmp(pcmd->cmd, 'Rr')==0)
	{
		Valve_ReadRegister(pcmd);
	}

	return 0;
}
void UART_ConfigReturnData(uint8_t ret)
{

}
