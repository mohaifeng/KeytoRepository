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

extern DEV_STATUSTYPEDEF cmd_execute_status;
HAL_StatusTypeDef Cmd_LegalCheck_Callback(const uint8_t *cmd_buff, uint8_t buff_size);
HAL_StatusTypeDef CmdPar_LegalCheck_Callback(uint16_t idex, const uint8_t *cmd_buff,
		uint8_t buff_size);
void Cmd_Task(UART_HandleTypeDef *huart, const uint8_t *cmd_buff, uint8_t buff_size);

#endif /* INC_CMD_H_ */
