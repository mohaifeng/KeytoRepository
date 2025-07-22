/*
 * m_usart.h
 *
 *  Created on: Jul 22, 2025
 *      Author: 莫海峰
 */

#ifndef M_INC_M_USART_H_
#define M_INC_M_USART_H_

#include "stm32g4xx_hal.h"

#define BUFFER_SIZE		255  // 缓冲区大小
#define SYS_USART_INIT_CONFIG 0//是否使用系统串口配置

typedef struct
{
	uint8_t rx_len;  //接收一帧数据的长度
	volatile uint8_t usart_rx_flag; //一帧数据接收完成标志
	uint8_t rx_buffer[BUFFER_SIZE]; //接收缓存数组
} USART_RX_TYPEDEF;

typedef struct
{
	uint8_t tx_len;  //接收一帧数据的长度
	uint8_t tx_buffer[BUFFER_SIZE]; //接收缓存数组
} USART_TX_TYPEDEF;

extern USART_RX_TYPEDEF usart1_rx_struct; //串口1接收数据结构体
extern USART_TX_TYPEDEF usart1_tx_struct; //串口1发送数据结构体
extern USART_RX_TYPEDEF usart2_rx_struct; //串口2接收数据结构体
extern USART_TX_TYPEDEF usart2_tx_struct; //串口2发送数据结构体


void User_USART1_UART_Init(void);
void User_USART2_UART_Init(void);
void Usart_SendData(UART_HandleTypeDef *huart,const void *pro_struct);
void Usart_ProcessReceivedData(UART_HandleTypeDef *huart);
void Usart_Start_Receive(UART_HandleTypeDef *huart);

#endif /* M_INC_M_USART_H_ */
