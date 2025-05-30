/*
 * cmd.h
 *
 *  Created on: May 27, 2025
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


typedef struct
{
	uint8_t cmd_init[CMD_LEN];
	uint8_t cmd_aspirate[CMD_LEN];
	uint8_t cmd_dispense[CMD_LEN];
	uint8_t cmd_read_register[CMD_LEN];
	uint8_t cmd_write_register[CMD_LEN];
} CMD_Typedef;

typedef struct
{
	uint8_t cmd_arr[CMD_LEN];
	uint8_t cmd_par[CMD_PARLEN];
	uint8_t cmd_par_Len;
} CMD_Config_t;

typedef struct
{
	uint8_t cmd_arr[CMD_LEN];
	uint8_t cmd_par[CMD_PARLEN];
	uint8_t cmd_par_Len;
} CMD_OD_Map_t;

extern const CMD_Typedef cmd_stu;


void Cmd_Data_Config(const uint8_t *cmd_buff, uint8_t buff_size);

#endif /* INC_CMD_H_ */
