/*
 * m_usart.h
 *
 *  Created on: Jul 22, 2025
 *      Author: 莫海峰
 */

#ifndef M_INC_M_USART_H_
#define M_INC_M_USART_H_

#include "stm32g4xx_hal.h"

#define BUFFER_SIZE       		256
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
} USART_RX_TYPEDEF;

typedef struct
{
	uint8_t tx_len;  //发送一帧数据的长度
	uint8_t tx_buffer[BUFFER_SIZE]; //发送缓存数组
} USART_TX_TYPEDEF;

extern volatile UsartState_t usart1_state;
extern volatile UsartState_t usart2_state;
extern USART_RX_TYPEDEF usart1_rx_struct; //串口1接收数据结构体
extern USART_TX_TYPEDEF usart1_tx_struct; //串口1发送数据结构体
extern USART_RX_TYPEDEF usart2_rx_struct; //串口2接收数据结构体
extern USART_TX_TYPEDEF usart2_tx_struct; //串口2发送数据结构体

void User_USART1_UART_Init(void);
void User_USART2_UART_Init(void);
void Usart_SendData(UART_HandleTypeDef *huart);
HAL_StatusTypeDef Usart_GetProcessedData(UART_HandleTypeDef *huart, uint8_t **data, uint16_t *length);
void Usart_Start_Receive(UART_HandleTypeDef *huart);
void Usart_Task(UART_HandleTypeDef *huart);
#endif /* M_INC_M_USART_H_ */
