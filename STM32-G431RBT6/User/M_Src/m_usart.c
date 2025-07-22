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

USART_RX_TYPEDEF usart1_rx_struct; //串口1接收数据结构体
USART_TX_TYPEDEF usart1_tx_struct; //串口1发送数据结构体
USART_RX_TYPEDEF usart2_rx_struct; //串口2接收数据结构体
USART_TX_TYPEDEF usart2_tx_struct; //串口2发送数据结构体

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
	usart1_rx_struct.usart_rx_flag = 0;
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
	usart2_rx_struct.usart_rx_flag = 0;
}

void Usart_Start_Receive(UART_HandleTypeDef *huart)
{
	if (huart->Instance == USART1)
	{
		// 启动DMA接收
		usart1_rx_struct.rx_len = 0;  //清除计数
		usart1_rx_struct.usart_rx_flag = 0;  //清除接收结束标志位
		memset(usart1_rx_struct.rx_buffer, 0, BUFFER_SIZE);  //清空缓存区
		while (HAL_UART_Receive_DMA(&huart1, usart1_rx_struct.rx_buffer, BUFFER_SIZE))
		{
			HAL_DMA_Abort(huart1.hdmarx);
		}
	}
	if (huart->Instance == USART2)
	{
		// 启动DMA接收
		usart2_rx_struct.rx_len = 0;  //清除计数
		usart2_rx_struct.usart_rx_flag = 0;  //清除接收结束标志位
		memset(usart2_rx_struct.rx_buffer, 0, BUFFER_SIZE);  //清空缓存区
		while (HAL_UART_Receive_DMA(&huart2, usart2_rx_struct.rx_buffer, BUFFER_SIZE))
		{
			HAL_DMA_Abort(huart2.hdmarx);
		}
	}

}

void Usart_SendData(UART_HandleTypeDef *huart, const void *pro_struct)
{
	Send_Data_Conf(huart, pro_struct);
	if (huart->Instance == USART1)
	{
		HAL_UART_Transmit_DMA(&huart1, usart1_tx_struct.tx_buffer, usart1_tx_struct.tx_len);  //发送数据
		while (HAL_UART_GetState(&huart1) != HAL_UART_STATE_READY)	 //等待发送完成
		{
		}
		Usart_Start_Receive(&huart1);
	}
	if (huart->Instance == USART2)
	{
		HAL_UART_Transmit_DMA(&huart2, usart2_tx_struct.tx_buffer, usart2_tx_struct.tx_len);  //发送数据
		while (HAL_UART_GetState(&huart2) != HAL_UART_STATE_READY)	 //等待发送完成
		{
		}
		Usart_Start_Receive(&huart2);
	}

}

//接收到数据后处理函数
void Usart_ProcessReceivedData(UART_HandleTypeDef *huart)
{
	if (huart->Instance == USART1)
	{
		Protocol_Analyze(usart1_rx_struct.rx_buffer, usart1_rx_struct.rx_len);
		switch (protocol_type)
		{
			case PROTOCOL_DT:
				Cmd_Task(&huart1, dt_struct.data_buff, dt_struct.cmd_len);
				break;
			case PROTOCOL_OEM:
				Cmd_Task(&huart1, oem_struct.data_buff, oem_struct.cmd_len);
				break;
			case PROTOCOL_IdexSame:
				Usart_SendData(&huart1, &oem_struct);
				break;
			default:
				break;
		}
		if (usart1_rx_struct.usart_rx_flag == 1)
		{
			Usart_Start_Receive(&huart1);
		}
	}
	if (huart->Instance == USART2)
	{
		Protocol_Analyze(usart2_rx_struct.rx_buffer, usart2_rx_struct.rx_len);
		switch (protocol_type)
		{
			case PROTOCOL_DT:
				Cmd_Task(&huart2, dt_struct.data_buff, dt_struct.cmd_len);
				break;
			case PROTOCOL_OEM:
				Cmd_Task(&huart2, oem_struct.data_buff, oem_struct.cmd_len);
				break;
			case PROTOCOL_IdexSame:
				Usart_SendData(&huart2, &oem_struct);
				break;
			default:
				break;
		}
		if (usart2_rx_struct.usart_rx_flag == 1)
		{
			Usart_Start_Receive(&huart2);
		}
	}
}

