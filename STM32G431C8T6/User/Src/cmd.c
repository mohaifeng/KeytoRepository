/*
 * cmd.c
 *
 *  Created on: May 27, 2025
 *      Author: 莫海峰
 */
#include "main.h"
#include "cmd.h"
#include "dev.h"
#include "protocol.h"
#include <string.h>
#include <ctype.h>
#include "objectdirectory.h"

Cmd_list_t cmd_list;

void Cmd_List_Clear(void)
{
	cmd_list.head = 0;
	cmd_list.tail = 0;
	cmd_list.num = 0;
	for (uint8_t i = 0; i < CMD_LIST_MAX; i++)
	{
		cmd_list.cmd[i].cmd = 0;
		cmd_list.cmd[i].value = 0;
	}
}

HAL_StatusTypeDef Cmd_List_Append(Cmd_Par_t cmd)
{
	if (cmd_list.num >= CMD_LIST_MAX)
		return HAL_ERROR;
	cmd_list.cmd[cmd_list.tail % CMD_LIST_MAX].cmd = cmd.cmd;
	cmd_list.cmd[cmd_list.tail % CMD_LIST_MAX].value = cmd.value;
	cmd_list.tail = (cmd_list.tail + 1) % CMD_LIST_MAX;
	cmd_list.num = (cmd_list.num + 1) % CMD_LIST_MAX;
	return HAL_OK;
}

void Cmd_List_Execute(void)
{
	Cmd_Par_t temp;
	if (cmd_list.num == 0)
		return;
	temp.cmd = cmd_list.cmd[cmd_list.head].cmd;
	temp.value = cmd_list.cmd[cmd_list.head].value;
	Cmd_List_Append(temp);


}

