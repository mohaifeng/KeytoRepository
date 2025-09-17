/*
 * cmd.h
 *
 *  Created on: May 27, 2025
 *      Author: 莫海峰
 */

#ifndef INC_CMD_H_
#define INC_CMD_H_

#include "stm32g4xx_hal.h"
#include "user_usart.h"
#include "protocol.h"
#include "dev.h"
#include "register.h"

#define CMD_LEN       2
#define CMD_PAR_MAX   32
#define CMD_LIST_MAX  32

typedef struct
{
	uint8_t port_num; //串口号：1：串口1（RS232）；2：串口2（RS485）
	uint8_t idex;
	Protocol_t protocol;
	Sys_Status_t state;
	RegValue_Handle_t res_buff[CMD_PAR_MAX];
	uint8_t buff_size;
	uint8_t is_data;
} ResponseHandle_t;

typedef struct
{
	uint8_t port_num;
	uint8_t idex;
	Protocol_t protocol;
	uint8_t cmd[CMD_LEN + 1];
	int32_t value_buf[CMD_PAR_MAX];
	uint8_t value_num;//参数个数
} Cmd_Par_t;

typedef struct
{
	Cmd_Par_t cmd_buff[CMD_LIST_MAX];
	uint8_t num;
	uint8_t head;
	uint8_t tail;
} Cmd_list_t;

void Cmd_List_Clear(void);
void Cmd_List_Execute(void);
HAL_StatusTypeDef Cmd_List_Append(Cmd_Par_t *cmd);
uint32_t Int_to_Ascii(int32_t num, uint8_t *buffer);
HAL_StatusTypeDef Cmd_Config(Cmd_Par_t *cmd_buf, uint8_t *pdata, uint8_t len);
void Resolution_Cmd(const Cmd_Par_t *pcmd, ResponseHandle_t *resp);
void UART_ConfigReturnData(ResponseHandle_t *resp_data);
#endif /* INC_CMD_H_ */
