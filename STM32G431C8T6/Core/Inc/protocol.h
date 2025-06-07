/*
 * protocol.h
 *
 *  Created on: Mar 26, 2025
 *      Author: 莫海峰
 */

#ifndef INC_PROTOCOL_H_
#define INC_PROTOCOL_H_

#include "stm32g4xx_hal.h"
#define PROTOCOL_DATA_LEN 255

typedef enum
{
	PROTOCOL_NULL = 0,
	PROTOCOL_DT,
	PROTOCOL_OEM,
	PROTOCOL_IdexSame
} ProtocolType;

typedef struct
{
	uint8_t addr;
	uint8_t state;
	uint8_t dt_flag;
	uint8_t data_buff[PROTOCOL_DATA_LEN];
	uint8_t cmd_len; //数据数组长度
	uint8_t end_flag;
} DT_TYPEDEF;

typedef struct
{
	uint8_t head;
	uint8_t idex;
	uint8_t addr;
	uint8_t state;
	uint8_t cmd_len;
	uint8_t data_buff[PROTOCOL_DATA_LEN];
	uint8_t data_len; //数据帧长度
	uint8_t checksum;
} OEM_TYPEDEF;

extern OEM_TYPEDEF oem_struct; //oem结构体变量
extern DT_TYPEDEF dt_struct; //dt结构体变量
extern ProtocolType protocol_type;

void Rxdata_Analyze(const uint8_t *rx_buff, uint16_t len);
uint32_t Int_to_Ascii(int32_t num, uint8_t *buffer);
void Send_Data_Conf(UART_HandleTypeDef *uartHandle, const void *data_struct);
#endif /* INC_PROTOCOL_H_ */
