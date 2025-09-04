/*
 * cmd.h
 *
 *  Created on: May 27, 2025
 *      Author: 莫海峰
 */

#ifndef INC_CMD_H_
#define INC_CMD_H_

#include "stm32g4xx_hal.h"
#include "protocol.h"

#define CMD_LEN       2
#define CMD_PAR_MAX   10
#define CMD_LIST_MAX  32

typedef struct
{
	uint8_t port_num;
	Protocol_t protocol;
	uint8_t state;
	uint8_t res_buff[CMD_PAR_MAX];
	uint8_t is_data;
} ResponseHandle_t;

typedef struct
{
	/*通用部分*/
	uint8_t port_num;
	uint8_t idex;
	Protocol_t protocol;
	uint8_t cmd[CMD_LEN + 1];
	int32_t value_buf[CMD_PAR_MAX];
	uint8_t value_num;
} Cmd_Par_t;

typedef struct
{
	Cmd_Par_t cmd_buff[CMD_LIST_MAX];
	uint8_t num;
	uint8_t head;
	uint8_t tail;
} Cmd_list_t;

void Cmd_List_Clear(void);
HAL_StatusTypeDef Cmd_List_Append(Cmd_Par_t *cmd);
HAL_StatusTypeDef Cmd_Config(Cmd_Par_t *cmd_buf, uint8_t *pdata, uint8_t len);
int8_t Resolution_Cmd(const Cmd_Par_t *cmd);
void UART_ConfigReturnData(uint8_t ret);
#endif /* INC_CMD_H_ */
