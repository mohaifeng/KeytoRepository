/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.h
  * @brief          : Header for main.c file.
  *                   This file contains the common defines of the application.
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; Copyright (c) 2021 STMicroelectronics.
  * All rights reserved.</center></h2>
  *
  * This software component is licensed by ST under BSD 3-Clause license,
  * the "License"; You may not use this file except in compliance with the
  * License. You may obtain a copy of the License at:
  *                        opensource.org/licenses/BSD-3-Clause
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

/* USER CODE END Includes */

/* Exported types ------------------------------------------------------------*/
/* USER CODE BEGIN ET */
#define USE_USART_CONSOLE_CONFIG 			0
#define USE_SMARTSV_PROTOCOL					1
#define DEFAULT_FLOW_MIN_POSITION 		1
/* USER CODE END ET */

/* Exported constants --------------------------------------------------------*/
/* USER CODE BEGIN EC */

/* USER CODE END EC */

/* Exported macro ------------------------------------------------------------*/
/* USER CODE BEGIN EM */
extern TIM_HandleTypeDef htim3;
extern TIM_HandleTypeDef htim4;

extern uint32_t ValvePositionOptTriggerFlag;
extern uint32_t ValveZeroOptTriggerFlag;
extern uint8_t NonVolatileMemory;
/* USER CODE END EM */

/* Exported functions prototypes ---------------------------------------------*/
void Error_Handler(void);

/* USER CODE BEGIN EFP */

/* USER CODE END EFP */

/* Private defines -----------------------------------------------------------*/
#define OW1_Pin GPIO_PIN_0
#define OW1_GPIO_Port GPIOA
#define OW1_EXTI_IRQn EXTI0_IRQn
#define SPI1_CS_Pin GPIO_PIN_4
#define SPI1_CS_GPIO_Port GPIOA
#define DIAG0_Pin GPIO_PIN_1
#define DIAG0_GPIO_Port GPIOB
#define DIAG1_Pin GPIO_PIN_2
#define DIAG1_GPIO_Port GPIOB
#define DRV_ENN_Pin GPIO_PIN_10
#define DRV_ENN_GPIO_Port GPIOB
#define MCU_LED_Pin GPIO_PIN_13
#define MCU_LED_GPIO_Port GPIOB
#define OW2_Pin GPIO_PIN_3
#define OW2_GPIO_Port GPIOB
#define OW2_EXTI_IRQn EXTI3_IRQn
#define TMC_STEP_Pin GPIO_PIN_4
#define TMC_STEP_GPIO_Port GPIOB
#define TMC_DIR_Pin GPIO_PIN_5
#define TMC_DIR_GPIO_Port GPIOB
/* USER CODE BEGIN Private defines */
#define LED_ON() HAL_GPIO_WritePin(MCU_LED_GPIO_Port, MCU_LED_Pin, GPIO_PIN_RESET)
#define LED_OFF() HAL_GPIO_WritePin(MCU_LED_GPIO_Port, MCU_LED_Pin, GPIO_PIN_SET)
#define LED_Toggle() HAL_GPIO_TogglePin(MCU_LED_GPIO_Port, MCU_LED_Pin)

#define PUMP_ON() //HAL_GPIO_WritePin(Pump_Ctrl_GPIO_Port, Pump_Ctrl_Pin, GPIO_PIN_RESET)
#define PUMP_OFF() //HAL_GPIO_WritePin(Pump_Ctrl_GPIO_Port, Pump_Ctrl_Pin, GPIO_PIN_SET)


#define USE_TITAN_PROTOCOL												1
#define USING_OPT_INVERTER												1	//配置光耦是否反向			
/* USER CODE END Private defines */

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */
