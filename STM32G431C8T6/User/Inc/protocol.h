/*
 * protocol.h
 *
 *  Created on: Mar 26, 2025
 *      Author: 莫海峰
 */

#ifndef INC_PROTOCOL_H_
#define INC_PROTOCOL_H_

#include "main.h"


typedef struct
{
	uint8_t head;         //头
	uint8_t add;          //地址
	uint8_t cmd;          //命令
	uint8_t code[4];			//密码
	uint8_t data[4];     //功能参数
	uint8_t end;          //尾
	uint16_t verify;      //校验
} SV_Protocol_Handle_t;

typedef struct
{
	uint8_t head;             //头
	uint8_t add;              //地址
	uint8_t cmd;       				//指令
	uint8_t data[4];        	//数据
	uint16_t verify;          //校验
} GeneralProtocol_Handle_t;

HAL_StatusTypeDef UART_ResolutionGeneral_Protocol(uint8_t *data, uint8_t len);

#endif /* INC_PROTOCOL_H_ */
