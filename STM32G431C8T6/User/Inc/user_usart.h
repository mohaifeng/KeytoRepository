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
#define RX_BUFFER_DIMENSION   2

// 定义状态
typedef enum
{
	USART_RECEIVING,
	USART_PROCESSING,
	USART_SENDING,
	USART_WAITTXFINISH,
} UsartState_t;

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

extern Usart_RX_t usart1_rx_struct; //串口1接收数据结构体
extern Usart_TX_t usart1_tx_struct; //串口1发送数据结构体
extern Usart_RX_t usart2_rx_struct; //串口2接收数据结构体
extern Usart_TX_t usart2_tx_struct; //串口2发送数据结构体
extern volatile UsartState_t usart1_state;
extern volatile UsartState_t usart2_state;

void User_USART1_Init(void);
void User_USART2_Init(void);
void Usart_SendData(UART_HandleTypeDef *huart);
HAL_StatusTypeDef Usart_GetData(UART_HandleTypeDef *huart, uint8_t **data, uint8_t length);
void Usart_Start_Receive(UART_HandleTypeDef *huart);
void USER_UART_IDLECallback(UART_HandleTypeDef *huart);
void Usart_Task(UART_HandleTypeDef *huart);

#endif /* INC_USER_USART_H_ */
