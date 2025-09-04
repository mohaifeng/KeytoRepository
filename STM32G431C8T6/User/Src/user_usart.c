/*
 * dev.c
 *
 *  Created on: Aug 12, 2025
 *      Author: 莫海峰
 */

#include "user_usart.h"
#include "usart.h"
#include "dev.h"
#include <string.h>

extern DMA_HandleTypeDef hdma_usart1_rx;
extern DMA_HandleTypeDef hdma_usart2_rx;

volatile UsartState_t usart1_state = USART_RECEIVING;
volatile UsartState_t usart2_state = USART_RECEIVING;
Usart_RX_t usart1_rx_struct; //串口1接收数据结构体
Usart_TX_t usart1_tx_struct; //串口1发送数据结构体
Usart_RX_t usart2_rx_struct; //串口2接收数据结构体
Usart_TX_t usart2_tx_struct; //串口2发送数据结构体

static void Usart_Buffer_Init(UART_HandleTypeDef *huart);

void User_USART1_Init(void)
{
	huart1.Instance = USART1;
	huart1.Init.BaudRate = sysconfig.CommunicationConfig.RS232_BaudRate;
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

void User_USART2_Init(void)
{
	huart2.Instance = USART2;
	huart2.Init.BaudRate = sysconfig.CommunicationConfig.RS485_BaudRate;
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


void Usart_Transmit(UART_HandleTypeDef *huart)
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

// 获取缓冲区数据
HAL_StatusTypeDef Usart_GetData(UART_HandleTypeDef *huart, uint8_t **data, uint8_t length)
{
	if (huart->Instance == USART1)
	{
		if (usart1_rx_struct.dataready)
		{
			*data = usart1_rx_struct.rx_buffer[!usart1_rx_struct.active_buffer];
			length = usart1_rx_struct.rx_len;
			usart1_rx_struct.dataready = 0;  //清除数据就绪标志
			return HAL_OK;
		}
		return HAL_ERROR;
	}
	if (huart->Instance == USART2)
	{
		if (usart2_rx_struct.dataready)
		{
			*data = usart2_rx_struct.rx_buffer[!usart2_rx_struct.active_buffer];
			length = usart2_rx_struct.rx_len;
			usart2_rx_struct.dataready = 0;  //清除数据就绪标志
			return HAL_OK;
		}
		return HAL_ERROR;
	}
	return HAL_ERROR;
}

void Usart_Start_Receive(UART_HandleTypeDef *huart)
{
	if (huart->Instance == USART1)
	{
		// 启动DMA接收
		usart1_rx_struct.dataready = 0;
		memset(usart1_rx_struct.rx_buffer[usart1_rx_struct.active_buffer], 0, BUFFER_SIZE);  //清空缓存区
		while (HAL_UART_Receive_DMA(&huart1, usart1_rx_struct.rx_buffer[usart1_rx_struct.active_buffer], BUFFER_SIZE))
		{
			HAL_DMA_Abort(huart1.hdmarx);
		}
	}
	if (huart->Instance == USART2)
	{
		// 启动DMA接收
		usart2_rx_struct.dataready = 0;
		memset(usart2_rx_struct.rx_buffer[usart2_rx_struct.active_buffer], 0, BUFFER_SIZE);  //清空缓存区
		while (HAL_UART_Receive_DMA(&huart2, usart2_rx_struct.rx_buffer[usart2_rx_struct.active_buffer], BUFFER_SIZE))
		{
			HAL_DMA_Abort(huart2.hdmarx);
		}
	}

}

void USER_UART_IRQHandler(UART_HandleTypeDef *huart)
{
	if (huart->Instance == USART1) //判断是否是串口1
	{
		if (RESET != __HAL_UART_GET_FLAG(&huart1, UART_FLAG_IDLE))  //判断是否是空闲中断
		{
			__HAL_UART_CLEAR_IDLEFLAG(&huart1);  //清楚空闲中断标志（否则会一直不断进入中断）
			USER_UART_IDLECallback(&huart1);  //调用中断处理函数
		}
	}
	if (huart->Instance == USART2) //判断是否是串口1
	{
		if (RESET != __HAL_UART_GET_FLAG(&huart2, UART_FLAG_IDLE))  //判断是否是空闲中断
		{
			__HAL_UART_CLEAR_IDLEFLAG(&huart2);  //清楚空闲中断标志（否则会一直不断进入中断）
			USER_UART_IDLECallback(&huart2);  //调用中断处理函数
		}
	}
}

void USER_UART_IDLECallback(UART_HandleTypeDef *huart)
{
	if (huart->Instance == USART1)
	{
		HAL_UART_DMAStop(&huart1);  //停止本次DMA传输
		usart1_rx_struct.rx_len = BUFFER_SIZE - __HAL_DMA_GET_COUNTER(&hdma_usart1_rx);  //计算接收到的数据长度
		if (usart1_rx_struct.rx_len > 0)
		{
			// 切换到非活动缓冲区
			usart1_rx_struct.active_buffer = (!usart1_rx_struct.active_buffer);
			usart1_rx_struct.dataready = 1;
			USART1_RxTimeCnt = 0;
			usart1_state = USART_PROCESSING;    // 接受完成标志位置1
		}
		else
		{
			Usart_Start_Receive(&huart1); //开启DMA接收
			__HAL_UART_ENABLE_IT(&huart1, UART_IT_IDLE);	//使能空闲中断
		}
	}
	if (huart->Instance == USART2)
	{
		HAL_UART_DMAStop(&huart2);  //停止本次DMA传输
		usart2_rx_struct.rx_len = BUFFER_SIZE - __HAL_DMA_GET_COUNTER(&hdma_usart2_rx);  //计算接收到的数据长度
		if (usart2_rx_struct.rx_len > 0)
		{
			usart2_rx_struct.active_buffer = (!usart1_rx_struct.active_buffer);
			usart2_rx_struct.dataready = 1;
			USART2_RxTimeCnt = 0;
			usart2_state = USART_PROCESSING;    // 接受完成标志位置1
		}
		else
		{
			Usart_Start_Receive(&huart2); //开启DMA接收
			__HAL_UART_ENABLE_IT(&huart2, UART_IT_IDLE);	//使能空闲中断
		}
	}
}

void HAL_UART_TxCpltCallback(UART_HandleTypeDef *huart)
{
	if (huart->Instance == USART1)
	{
		usart1_state = USART_RECEIVING;
	}
	if (huart->Instance == USART2)
	{
		usart2_state = USART_RECEIVING;
	}
}
