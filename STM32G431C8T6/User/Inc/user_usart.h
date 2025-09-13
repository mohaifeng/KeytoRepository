/*
 * user_usart.h
 *
 *  Created on: Aug 14, 2025
 *      Author: 莫海峰
 */

#ifndef INC_USER_USART_H_
#define INC_USER_USART_H_

#include "main.h"

#define USER_USART_INIT_FLAG 	0
#define BUFFER_SIZE 					255
#define TX_LIST_MAX 					10
#define RX_BUFFER_DIMENSION   2

typedef struct
{
	uint8_t rx_len;  //接收一帧数据的长度
	uint8_t rx_buffer[RX_BUFFER_DIMENSION][BUFFER_SIZE]; // 双缓冲数组
	volatile uint8_t active_buffer;  // 当前活动缓冲区索引(0或1)
	volatile uint8_t dataready;        // 数据就绪标志(0或1)
} Usart_RX_t;

typedef struct
{
	uint8_t tx_len;  //发送一帧数据的长度
	uint8_t tx_buffer[BUFFER_SIZE]; //接收缓存数组
} Usart_TX_t;

typedef struct
{
	Usart_TX_t tx_buff_list[TX_LIST_MAX];
	uint8_t num;
	uint8_t head;
	uint8_t tail;
} Usart_TX_list_t;

extern Usart_RX_t usart1_rx_struct; //串口1接收数据结构体
extern Usart_TX_list_t urt1_tx_stu; //串口1发送数据结构体
extern Usart_RX_t usart2_rx_struct; //串口2接收数据结构体
extern Usart_TX_list_t urt2_tx_stu; //串口2发送数据结构体

void User_USART1_Init(void);
void User_USART2_Init(void);
void Usart_Transmit(UART_HandleTypeDef *huart);
void Clear_Usart_TxList(UART_HandleTypeDef *huart);
HAL_StatusTypeDef Usart_TxBuffer_Append(UART_HandleTypeDef *huart, Usart_TX_t *tx_stu);
HAL_StatusTypeDef Usart_GetData(UART_HandleTypeDef *huart, uint8_t **data, uint8_t *length);
void Usart_Start_Receive(UART_HandleTypeDef *huart);
void USER_UART_IDLECallback(UART_HandleTypeDef *huart);

#endif /* INC_USER_USART_H_ */
