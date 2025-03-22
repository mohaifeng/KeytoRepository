/*
 * protocol.h
 *
 *  Created on: Feb 27, 2025
 *      Author: 莫海峰
 */

#ifndef INC_PROTOCOL_H_
#define INC_PROTOCOL_H_
#include "main.h"
#include "stm32g4xx_hal.h"
#include "usart.h"
#include "verification.h"

#define PROTOCOL_DATA_LEN 255


typedef struct
{
	uint8_t addr;
	uint8_t state;
	uint8_t rx_flag;
	uint8_t data[PROTOCOL_DATA_LEN];
	uint8_t data_len;
} DT_TYPEDEF;

typedef struct
{
	uint8_t head;
	uint8_t idex;
	uint8_t addr;
	uint8_t state;
	uint8_t cmd_len;
	uint8_t data[PROTOCOL_DATA_LEN];
	uint8_t data_len;
	uint8_t checksum;
} OEM_TYPEDEF;

//OEM_TYPEDEF *oem_send_data;//发送数据oem结构体变量
//OEM_TYPEDEF *oem_rec_data;//接收到数据oem结构体变量
//DT_TYPEDEF *dt_send_data;//发送数据dt结构体变量
//DT_TYPEDEF *dt_rec_data;//接收到数据dt结构体变量


uint8_t OEM_Conf(uint8_t *rx_buff, uint16_t len);
void OEM_Send_Conf(void);
void Protocol_Analyze(uint8_t *rx_buff, uint8_t len);
#endif /* INC_PROTOCOL_H_ */

