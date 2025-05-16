/* USER CODE BEGIN Header */
/**
 ******************************************************************************
 * @file           : main.h
 * @brief          : Header for main.c file.
 *                   This file contains the common defines of the application.
 ******************************************************************************
 * @attention
 *
 * Copyright (c) 2025 STMicroelectronics.
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
#ifndef __MAIN_H
#define __MAIN_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "stm32g4xx_hal.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "gpio.h"
#include "usart.h"
#include "led.h"
#include "tmc5160.h"
#include "objectdirectory.h"
/* USER CODE END Includes */

/* Exported types ------------------------------------------------------------*/
/* USER CODE BEGIN ET */

/* USER CODE END ET */

/* Exported constants --------------------------------------------------------*/
/* USER CODE BEGIN EC */
	typedef enum
	{
		DEV_IDLE = 0,
		DEV_BUSY,
		EXECUTE_SUCCESS,
		COMPLETE,
		OVER_LIMIT = 10,
		PARAMETER_ERROR,
		SYNTAX_ERROR,
		INVALID_CMD,
		REG_ERROR,
		READ_WRITE_ONLY,
		CMD_OVERFLOW,
		NO_INIT
	} DEV_STATUSTYPEDEF;

	// 系统配置结构体
	typedef struct
	{
		uint8_t protocolswitch;
		uint8_t addr;
		uint8_t status;
		uint32_t permission;
		uint8_t is_tip;
		int32_t pressure;
		uint8_t gpo1_out_mode;
		uint16_t max_volume;
		uint8_t check_tip;
		uint32_t ser_baudrate;
		uint16_t can_baudrate;
		uint8_t report_flag;
		uint32_t can_heart;
		uint32_t version;
		uint32_t model;
	} SysConfig_t;

	typedef struct
	{
		uint8_t plld_par;


	} PlldConfig_t;

	typedef struct
	{
		uint8_t detect_enable;
		uint8_t clot_par;
		uint8_t empty_par;
		uint8_t foam_par;

	} PressureAbnormalConfig_t;

	typedef struct
	{
		int32_t PWMFre;
		uint8_t Dir;
		uint16_t ReverseDelay;
		uint8_t PWMMaxPulse;
	// 其他电机配置...
	} BMConfig_t;
/* USER CODE END EC */

/* Exported macro ------------------------------------------------------------*/
/* USER CODE BEGIN EM */

/* USER CODE END EM */

/* Exported functions prototypes ---------------------------------------------*/
void Error_Handler(void);

/* USER CODE BEGIN EFP */

/* USER CODE END EFP */

/* Private defines -----------------------------------------------------------*/
#define OW1_Pin GPIO_PIN_0
#define OW1_GPIO_Port GPIOA
#define OW1_EXTI_IRQn EXTI0_IRQn
#define LED_BLUE_Pin GPIO_PIN_13
#define LED_BLUE_GPIO_Port GPIOB

/* USER CODE BEGIN Private defines */

/* USER CODE END Private defines */

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */
