/*
 * m_usart.c
 *
 *  Created on: Jul 22, 2025
 *      Author: 莫海峰
 */
#include "m_usart.h"
#include "usart.h"
#include "register.h"
#include "cmd.h"
#include <string.h>

volatile UsartState_t usart1_state = USART_RECEIVING;
volatile UsartState_t usart2_state = USART_RECEIVING;
USART_RX_TYPEDEF usart1_rx_struct; //串口1接收数据结构体
USART_TX_TYPEDEF usart1_tx_struct; //串口1发送数据结构体
USART_RX_TYPEDEF usart2_rx_struct; //串口2接收数据结构体
USART_TX_TYPEDEF usart2_tx_struct; //串口2发送数据结构体

static void Usart_Buffer_Init(UART_HandleTypeDef *huart);

void User_USART1_UART_Init(void)
{
	HAL_UART_DeInit(&huart1);
	huart1.Instance = USART1;
	huart1.Init.BaudRate = SysConfig.ser_baudrate;
	huart1.Init.WordLength = UART_WORDLENGTH_8B;
	huart1.Init.StopBits = UART_STOPBITS_1;
	huart1.Init.Parity = UART_PARITY_NONE;
	huart1.Init.Mode = UART_MODE_TX_RX;
	huart1.Init.HwFlowCtl = UART_HWCONTROL_NONE;
	huart1.Init.OverSampling = UART_OVERSAMPLING_16;
	huart1.Init.OneBitSampling = UART_ONE_BIT_SAMPLE_DISABLE;
	huart1.Init.ClockPrescaler = UART_PRESCALER_DIV1;
	huart1.AdvancedInit.AdvFeatureInit = UART_ADVFEATURE_NO_INIT;
	if (HAL_UART_Init(&huart1) != HAL_OK)
	{
		Error_Handler();
	}
	if (HAL_UARTEx_SetTxFifoThreshold(&huart1, UART_TXFIFO_THRESHOLD_1_8) != HAL_OK)
	{
		Error_Handler();
	}
	if (HAL_UARTEx_SetRxFifoThreshold(&huart1, UART_RXFIFO_THRESHOLD_1_8) != HAL_OK)
	{
		Error_Handler();
	}
	if (HAL_UARTEx_DisableFifoMode(&huart1) != HAL_OK)
	{
		Error_Handler();
	}
	Usart_Buffer_Init(&huart1); //初始化接收二维数组
	Usart_Start_Receive(&huart1); //开启DMA接收
	__HAL_UART_ENABLE_IT(&huart1, UART_IT_IDLE);	//使能空闲中断
	usart1_state = USART_RECEIVING;
}

void User_USART2_UART_Init(void)
{
	HAL_UART_DeInit(&huart2);
	huart2.Instance = USART2;
	huart2.Init.BaudRate = SysConfig.ser_baudrate;
	huart2.Init.WordLength = UART_WORDLENGTH_8B;
	huart2.Init.StopBits = UART_STOPBITS_1;
	huart2.Init.Parity = UART_PARITY_NONE;
	huart2.Init.Mode = UART_MODE_TX_RX;
	huart2.Init.HwFlowCtl = UART_HWCONTROL_NONE;
	huart2.Init.OverSampling = UART_OVERSAMPLING_16;
	huart2.Init.OneBitSampling = UART_ONE_BIT_SAMPLE_DISABLE;
	huart2.Init.ClockPrescaler = UART_PRESCALER_DIV1;
	huart2.AdvancedInit.AdvFeatureInit = UART_ADVFEATURE_NO_INIT;
	if (HAL_RS485Ex_Init(&huart2, UART_DE_POLARITY_HIGH, 0, 0) != HAL_OK)
	{
		Error_Handler();
	}
	if (HAL_UARTEx_SetTxFifoThreshold(&huart2, UART_TXFIFO_THRESHOLD_1_8) != HAL_OK)
	{
		Error_Handler();
	}
	if (HAL_UARTEx_SetRxFifoThreshold(&huart2, UART_RXFIFO_THRESHOLD_1_8) != HAL_OK)
	{
		Error_Handler();
	}
	if (HAL_UARTEx_DisableFifoMode(&huart2) != HAL_OK)
	{
		Error_Handler();
	}
	Usart_Buffer_Init(&huart1);
	Usart_Start_Receive(&huart2); //开启DMA接收
	__HAL_UART_ENABLE_IT(&huart2, UART_IT_IDLE);	//使能空闲中断
	usart2_state = USART_RECEIVING;
}

static void ProcessReceivedData(UART_HandleTypeDef *huart)
{
	uint8_t *tx_buff;
	uint16_t tx_len = 0;
	if (huart->Instance == USART1)
	{
		// 示例：将接收到的数据回传
		Usart_GetProcessedData(&huart1, &tx_buff, &tx_len);
		memcpy(usart1_tx_struct.tx_buffer, tx_buff, tx_len);
		usart1_tx_struct.tx_len = tx_len;
		// 切换到发送状态
		usart1_state = USART_SENDING;
	}
	if (huart->Instance == USART2)
	{
		// 示例：将接收到的数据回传
		Usart_GetProcessedData(&huart2, &tx_buff, &tx_len);
		memcpy(usart2_tx_struct.tx_buffer, tx_buff, tx_len);
		usart2_tx_struct.tx_len = tx_len;
		// 切换到发送状态
		usart2_state = USART_SENDING;
	}

}

void Usart_Task(UART_HandleTypeDef *huart)
{
	if (huart->Instance == USART1)
	{
		switch (usart1_state)
		{
			case USART_RECEIVING:
				break;
			case USART_PROCESSING:
				ProcessReceivedData(huart);
				break;
			case USART_SENDING:
				Usart_SendData(huart);
			case USART_WAITTXFINISH:
				break;
			default:
				break;
		}
	}
	if (huart->Instance == USART2)
	{
		switch (usart2_state)
		{
			case USART_RECEIVING:
				break;
			case USART_PROCESSING:
				ProcessReceivedData(huart);
				//			Cmd_Task();
				break;
			case USART_SENDING:
				Usart_SendData(huart);
			case USART_WAITTXFINISH:
				break;
			default:
				break;
		}
	}
}

void Usart_Start_Receive(UART_HandleTypeDef *huart)
{
	if (huart->Instance == USART1)
	{
		memset(usart1_rx_struct.rx_buffer[usart1_rx_struct.active_buffer], 0, BUFFER_SIZE);  //清空缓存区
		while (HAL_UART_Receive_DMA(&huart1, usart1_rx_struct.rx_buffer[usart1_rx_struct.active_buffer], BUFFER_SIZE))
		{
			HAL_DMA_Abort(huart1.hdmarx);
		}
	}
	if (huart->Instance == USART2)
	{
		// 启动DMA接收
		memset(usart2_rx_struct.rx_buffer[usart2_rx_struct.active_buffer], 0, BUFFER_SIZE);  //清空缓存区
		while (HAL_UART_Receive_DMA(&huart2, usart2_rx_struct.rx_buffer[usart2_rx_struct.active_buffer], BUFFER_SIZE))
		{
			HAL_DMA_Abort(huart2.hdmarx);
		}
	}
}

static void Usart_Buffer_Init(UART_HandleTypeDef *huart)
{
	if (huart->Instance == USART1)
	{
		usart1_rx_struct.active_buffer = 0;
		usart1_rx_struct.dataready = 0;
	}
	if (huart->Instance == USART2)
	{
		usart2_rx_struct.active_buffer = 0;
		usart2_rx_struct.dataready = 0;
	}
}
// 获取待处理数据的缓冲区
HAL_StatusTypeDef Usart_GetProcessedData(UART_HandleTypeDef *huart, uint8_t **data, uint16_t *length)
{
	if (huart->Instance == USART1)
	{
		if (usart1_rx_struct.dataready)
		{
			*data = usart1_rx_struct.rx_buffer[!usart1_rx_struct.active_buffer];
			*length = usart1_rx_struct.rx_len;
			return HAL_OK;
		}
		return HAL_ERROR;
	}
	if (huart->Instance == USART2)
	{
		if (usart2_rx_struct.dataready)
		{
			*data = usart2_rx_struct.rx_buffer[!usart2_rx_struct.active_buffer];
			*length = usart2_rx_struct.rx_len;
			return HAL_OK;
		}
		return HAL_ERROR;
	}
	return HAL_ERROR;
}

void Usart_SendData(UART_HandleTypeDef *huart)
{
	if (huart->Instance == USART1)
	{
		HAL_UART_Transmit_DMA(&huart1, usart1_tx_struct.tx_buffer, usart1_tx_struct.tx_len);  //发送数据
		usart1_state = USART_WAITTXFINISH;
	}
	if (huart->Instance == USART2)
	{
		HAL_UART_Transmit_DMA(&huart2, usart2_tx_struct.tx_buffer, usart2_tx_struct.tx_len);  //发送数据
		usart2_state = USART_WAITTXFINISH;
	}

}

//接收到数据后处理函数

