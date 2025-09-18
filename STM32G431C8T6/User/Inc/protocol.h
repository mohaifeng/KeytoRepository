/*
 * protocol.h
 *
 *  Created on: Mar 26, 2025
 *      Author: 莫海峰
 */

#ifndef INC_PROTOCOL_H_
#define INC_PROTOCOL_H_

#include "main.h"

#define RESOLUTION_BUFF_MAX  255

typedef enum
{
	PROT_KT_DT,
	PROT_KT_OEM,
	PROT_KT_CAN,
} Protocol_t;

typedef struct
{
	uint8_t rsl_buff[RESOLUTION_BUFF_MAX];         //协议解析缓存数组
	uint8_t datalen;
	volatile uint16_t head; // 写指针
	volatile uint16_t tail; // 读指针
} RealTimeResolution_t;

typedef struct
{
	uint8_t head;         //头
	uint8_t idex;          //索引
	uint8_t add;          //地址
	uint8_t cmd_len;          //命令长度
	uint8_t cmd[RESOLUTION_BUFF_MAX];			//命令
	uint8_t checksum_8;      //校验
} KT_OEM_Handle_t;

typedef struct
{
	uint8_t add;          //地址
	uint8_t tx_flag;          //发送标志
	uint8_t cmd[RESOLUTION_BUFF_MAX];			//命令
	uint8_t cmd_len;      //结尾
	uint8_t end;      //结尾
} KT_DT_Handle_t;

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

extern RealTimeResolution_t usart1_rtimeresol_stu;
extern RealTimeResolution_t usart2_rtimeresol_stu;

void Clear_RealtimeResolution_Buff(UART_HandleTypeDef *huart);
void RealtimeResolutionBuff_Append(UART_HandleTypeDef *huart, uint8_t *data, uint8_t len);
void ResolutionProtocol(UART_HandleTypeDef *huart);
HAL_StatusTypeDef UART_ResolutionKT_OEM_Protocol(UART_HandleTypeDef *huart, uint8_t *data, uint8_t len);
HAL_StatusTypeDef UART_ResolutionKT_DT_Protocol(UART_HandleTypeDef *huart, uint8_t *data, uint8_t len);

#endif /* INC_PROTOCOL_H_ */
