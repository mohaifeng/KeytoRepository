/*
 * protocol.h
 *
 *  Created on: Mar 26, 2025
 *      Author: 莫海峰
 */

#ifndef INC_PROTOCOL_H_
#define INC_PROTOCOL_H_

#include "verification.h"
#define PROTOCOL_DATA_LEN 255

typedef enum
{
	PROTOCOL_NULL = 0,
	PROTOCOL_DT,
	PROTOCOL_OEM
} ProtocolType;

typedef struct
{
	uint8_t addr;
	uint8_t state;
	uint8_t dt_flag;
	uint8_t data_buff[PROTOCOL_DATA_LEN];
	uint8_t data_buff_len; //数据数组长度
} DT_TYPEDEF;

typedef struct
{
	uint8_t head;
	uint8_t idex;
	uint8_t addr;
	uint8_t state;
	uint8_t cmd_len;
	uint8_t data[PROTOCOL_DATA_LEN];
	uint8_t data_len; //数据帧长度
	uint8_t checksum;
} OEM_TYPEDEF;

uint8_t DT_Rec_Conf(const uint8_t *rx_buff, uint8_t len);
uint8_t OEM_Rxdata_Analyze(const uint8_t *rx_buff, uint16_t len);
uint8_t Protocol_Analyze(const uint8_t *rx_buff, uint16_t len);
void Send_Data_Conf(UART_HandleTypeDef *uartHandle, const void *data_struct);
#endif /* INC_PROTOCOL_H_ */
