/*
 * cmd.h
 *
 *  Created on: May 27, 2025
 *      Author: 莫海峰
 */

#ifndef INC_CMD_H_
#define INC_CMD_H_

#include "stm32g4xx_hal.h"

#define CMD_LIST_MAX 32

typedef struct
{
	/*通用部分*/
	uint8_t cmd;
	int32_t value;
} Cmd_Par_t;

typedef struct
{
	Cmd_Par_t cmd[CMD_LIST_MAX];
	uint8_t num;
	uint8_t head;
	uint8_t tail;
} Cmd_list_t;

void Cmd_List_Clear(void);
HAL_StatusTypeDef Cmd_List_Append(Cmd_Par_t cmd);

#endif /* INC_CMD_H_ */
