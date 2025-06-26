/* USER CODE BEGIN Header */
/**
 ******************************************************************************
 * @file    usart.h
 * @brief   This file contains all the function prototypes for
 *          the usart.c file
 ******************************************************************************
 * @attention
 *
 * Copyright (c) 2024 STMicroelectronics.
 * All rights reserved.
 *
 * This software is licensed under terms that can be found in the LICENSE file
 * in the root directory of this software component.
 * If no LICENSE file comes with this software, it is provided AS-IS.
 *
 ******************************************************************************
 */
/* USER CODE END Header */
/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __USART_H__
#define __USART_H__

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "main.h"

/* USER CODE BEGIN Includes */
#include "protocol.h"
#include "dma.h"
/* USER CODE END Includes */

extern UART_HandleTypeDef huart1;

extern UART_HandleTypeDef huart2;

/* USER CODE BEGIN Private defines */
#define BUFFER_SIZE		255  // 缓冲区大小

/* USER CODE END Private defines */

void MX_USART1_UART_Init(void);
void MX_USART2_UART_Init(void);

/* USER CODE BEGIN Prototypes */
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
/* USER CODE END Private defines */

void MX_USART1_UART_Init(void);
void MX_USART2_UART_Init(void);

/* USER CODE BEGIN Prototypes */

void Usart_SendData(UART_HandleTypeDef *huart,const void *pro_struct);
void Usart_ProcessReceivedData(UART_HandleTypeDef *huart);
/* USER CODE END Prototypes */

#ifdef __cplusplus
}
#endif

#endif /* __USART_H__ */

