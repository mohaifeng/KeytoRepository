/*
 * cmd.h
 *
 *  Created on: Jun 13, 2025
 *      Author: 莫海峰
 */

#ifndef INC_CMD_H_
#define INC_CMD_H_

#include "stm32g4xx_hal.h"

#define CMD_LEN 							2
#define CMD_PARLEN 						10

#define CMD_INTI							"It"
#define CMD_ASPIRATE					"Ia"
#define CMD_DISPENSE					"Da"
#define CMD_READ_REGISTER			"Rr"
#define CMD_WRITE_REGISTER		"Wr"

typedef enum
{
	RWRegister,
	Init,
	Aspire,
	Dispense,
	AbsoluteMove,
	EjectTip,
	PLLD,
} CmdControlFlag;

extern DEV_STATUSTYPEDEF cmd_finish_flag;

HAL_StatusTypeDef ControlCmd_LegalCheck_Callback(UART_HandleTypeDef *huart, uint16_t idex, const uint8_t *cmd_buff,
		uint8_t buff_size);
void Cmd_Data_Config(UART_HandleTypeDef *huart, const uint8_t *cmd_buff, uint8_t buff_size);


#endif /* INC_CMD_H_ */
