/*
 * dev.c
 *
 *  Created on: Aug 12, 2025
 *      Author: 莫海峰
 */

#include "user_usart.h"
#include "usart.h"
#include "dev.h"
#include "tim.h"
#include <string.h>

extern DMA_HandleTypeDef hdma_usart1_rx;
extern DMA_HandleTypeDef hdma_usart2_rx;

Usart_RX_t usart1_rx_stu; //串口1接收数据结构体
Usart_RX_t usart2_rx_stu; //串口2接收数据结构体
Usart_TX_list_t usart_tx_stu; //串口1发送数据结构体

Oem_Idex_t usart1_idex_stu; //串口1上一条应答数据
Oem_Idex_t usart2_idex_stu; //串口2上一条应答数据

static void Clear_Usart_RxBuffer(UART_HandleTypeDef *huart);

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
	Clear_Usart_RxBuffer(&huart1); //初始化接收二维数组
	Usart_Start_Receive(&huart1); //开启DMA接收
	__HAL_UART_ENABLE_IT(&huart1, UART_IT_IDLE);	//使能空闲中断
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
	Clear_Usart_RxBuffer(&huart1);
	Usart_Start_Receive(&huart2); //开启DMA接收
	__HAL_UART_ENABLE_IT(&huart2, UART_IT_IDLE);	//使能空闲中断
}

void Usart_Transmit_Task(void)
{
	if (usart_tx_stu.num > 0)
	{
		switch (usart_tx_stu.tx_buff_list[usart_tx_stu.head].port)
		{
			case 1:
				if (__HAL_UART_GET_FLAG(&huart1, UART_FLAG_TC) == SET)
				{
					HAL_UART_Transmit_DMA(&huart1, usart_tx_stu.tx_buff_list[usart_tx_stu.head].tx_buffer,
							usart_tx_stu.tx_buff_list[usart_tx_stu.head].tx_len);  //发送数据
					usart_tx_stu.head = (usart_tx_stu.head + 1) % TX_LIST_MAX;
					usart_tx_stu.num--;
				}
				break;
			case 2:
				if (__HAL_UART_GET_FLAG(&huart2, UART_FLAG_TC) == SET)
				{
					HAL_UART_Transmit_DMA(&huart2, usart_tx_stu.tx_buff_list[usart_tx_stu.head].tx_buffer,
							usart_tx_stu.tx_buff_list[usart_tx_stu.head].tx_len);  //发送数据
					usart_tx_stu.head = (usart_tx_stu.head + 1) % TX_LIST_MAX;
					usart_tx_stu.num--;
				}
				break;
			default:
				break;
		}
	}
}

static void Clear_Usart_RxBuffer(UART_HandleTypeDef *huart)
{
	if (huart->Instance == USART1)
	{
		usart1_rx_stu.active_buffer = 0;
		usart1_rx_stu.dataready = 0;
		usart1_rx_stu.rx_len = 0;
	}
	if (huart->Instance == USART2)
	{
		usart2_rx_stu.active_buffer = 0;
		usart2_rx_stu.dataready = 0;
		usart2_rx_stu.rx_len = 0;
	}
}

void Clear_Usart_TxList(void)
{
	usart_tx_stu.head = 0;
	usart_tx_stu.tail = 0;
	usart_tx_stu.num = 0;
	for (uint8_t i = 0; i < TX_LIST_MAX; i++)
	{
		memset(usart_tx_stu.tx_buff_list[i].tx_buffer, 0, BUFFER_SIZE);
		usart_tx_stu.tx_buff_list[i].tx_len = 0;
		usart_tx_stu.tx_buff_list[i].port = 0;
	}
	usart1_idex_stu.idex = 0;
	memset(&usart1_idex_stu.tx_buff, 0, sizeof(Usart_TX_t));
	usart2_idex_stu.idex = 0;
	memset(&usart2_idex_stu.tx_buff, 0, sizeof(Usart_TX_t));
}

HAL_StatusTypeDef Usart_TxBuffer_Append(Usart_TX_t *tx_stu)
{
	if (usart_tx_stu.num >= TX_LIST_MAX)
		return HAL_ERROR;
	memcpy(&usart_tx_stu.tx_buff_list[usart_tx_stu.tail % TX_LIST_MAX], tx_stu, sizeof(Usart_TX_t));
	usart_tx_stu.tail = (usart_tx_stu.tail + 1) % TX_LIST_MAX;
	usart_tx_stu.num = (usart_tx_stu.num + 1) % TX_LIST_MAX;
	return HAL_OK;
}
// 获取缓冲区数据
HAL_StatusTypeDef Usart_GetData(UART_HandleTypeDef *huart, uint8_t **data, uint8_t *length)
{
	Usart_RX_t *tmp_rx_struct;
	if (huart->Instance == USART1)
	{
		tmp_rx_struct = &usart1_rx_stu;
	}
	else if (huart->Instance == USART2)
	{
		tmp_rx_struct = &usart2_rx_stu;
	}
	else
		return HAL_ERROR;
	if (tmp_rx_struct->dataready)
	{
		*data = tmp_rx_struct->rx_buffer[!tmp_rx_struct->active_buffer];
		*length = tmp_rx_struct->rx_len;
		return HAL_OK;
	}
	return HAL_ERROR;
}

void Usart_Start_Receive(UART_HandleTypeDef *huart)
{
	if (huart->Instance == USART1)
	{
		while (HAL_UART_Receive_DMA(huart, usart1_rx_stu.rx_buffer[usart1_rx_stu.active_buffer], BUFFER_SIZE))
		{
			HAL_DMA_Abort(huart->hdmarx);
		}
	}
	if (huart->Instance == USART2)
	{
		while (HAL_UART_Receive_DMA(huart, usart2_rx_stu.rx_buffer[usart2_rx_stu.active_buffer], BUFFER_SIZE))
		{
			HAL_DMA_Abort(huart->hdmarx);
		}
	}

}

void USER_UART_IRQHandler(UART_HandleTypeDef *huart)
{
	if (RESET != __HAL_UART_GET_FLAG(huart, UART_FLAG_IDLE))  //判断是否是空闲中断
	{
		__HAL_UART_CLEAR_IDLEFLAG(huart);  //清楚空闲中断标志（否则会一直不断进入中断）
		USER_UART_IDLECallback(huart);  //调用中断处理函数
	}

}

void USER_UART_IDLECallback(UART_HandleTypeDef *huart)
{
	HAL_UART_DMAStop(huart);  //停止本次DMA传输
	if (huart->Instance == USART1)
	{
		usart1_rx_stu.rx_len = BUFFER_SIZE - __HAL_DMA_GET_COUNTER(&hdma_usart1_rx);  //计算接收到的数据长度
		if (usart1_rx_stu.rx_len > 0)
		{
			usart1_rx_stu.active_buffer = (!usart1_rx_stu.active_buffer);
			usart1_rx_stu.dataready = 1;
			console1_status = CONSOLE_READY;
			USART1_RxTimeCnt = 0;
		}
	}
	if (huart->Instance == USART2)
	{
		usart2_rx_stu.rx_len = BUFFER_SIZE - __HAL_DMA_GET_COUNTER(&hdma_usart2_rx);  //计算接收到的数据长度
		if (usart2_rx_stu.rx_len > 0)
		{
			usart2_rx_stu.active_buffer = (!usart2_rx_stu.active_buffer);
			usart2_rx_stu.dataready = 1;
			console2_status = CONSOLE_READY;
			USART2_RxTimeCnt = 0;
		}
	}
	Usart_Start_Receive(huart);
}

