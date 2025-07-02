/* USER CODE BEGIN Header */
/**
 ******************************************************************************
 * @file           : main.h
 * @brief          : Header for main.c file.
 *                   This file contains the common defines of the application.
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
#define DEV_TYPE	0x00200001	//设备类型
#define SOFTWARE_VERSION 250519100 //软件日期
/* USER CODE END ET */

/* Exported constants --------------------------------------------------------*/
/* USER CODE BEGIN EC */
typedef enum
{
	DEV_IDLE = 0, //空闲
	DEV_BUSY, //忙
	EXECUTE_SUCCESS, //执行成功
	COMPLETE, //上报完成
	OVER_LIMIT = 10, //超限
	PARAMETER_ERROR, //参数错误
	SYNTAX_ERROR, //语法错误
	INVALID_CMD, //非法指令
	REG_ERROR, //寄存器地址错误
	READ_WRITE_ONLY, //只读/写
	CMD_OVERFLOW, //指令溢出
	NO_INIT //未初始化
} DEV_STATUSTYPEDEF;

// 系统配置结构体
typedef struct
{
	uint8_t protocolswitch; //协议类型
	uint8_t addr; //地址
	DEV_STATUSTYPEDEF status; //状态
	uint32_t permission; //权限
	uint8_t ow1_status;
	uint8_t ow2_status; //tip在位标志
	int32_t pressure; //压力值
	uint8_t gpo1_out_mode; //gpo1输出模式
	uint16_t max_volume; //最大体积
	uint8_t check_tip; //是否检测tip
	uint32_t ser_baudrate; //串口波特率
	uint16_t can_baudrate; //can波特率
	uint8_t report_flag; //主动上报标志
	uint32_t can_heart; //can心跳
	uint32_t version; //版本
	uint32_t model; //型号

} SysConfig_t;

typedef struct
{
	uint8_t plld_flag;
} PlldConfig_t;

typedef struct
{
	uint8_t detect_enable;
	uint8_t clot_par; //吸堵参数
	uint8_t empty_par; //吸空参数
	uint8_t foam_par; //吸气泡参数

} PressureDectConfig_t;
/* USER CODE END EC */

/* Exported macro ------------------------------------------------------------*/
/* USER CODE BEGIN EM */
extern SysConfig_t SysConfig;
extern PlldConfig_t PlldConfig;
extern PressureDectConfig_t PressureDectConfig;
extern volatile uint8_t ow1_status;
/* USER CODE END EM */

/* Exported functions prototypes ---------------------------------------------*/
void Error_Handler(void);

/* USER CODE BEGIN EFP */

/* USER CODE END EFP */

/* Private defines -----------------------------------------------------------*/
#define MCU_ID1_Pin GPIO_PIN_13
#define MCU_ID1_GPIO_Port GPIOC
#define MCU_GPO1_Pin GPIO_PIN_14
#define MCU_GPO1_GPIO_Port GPIOC
#define MCU_GPO1_EXTI_IRQn EXTI15_10_IRQn
#define MCU_GPO2_Pin GPIO_PIN_15
#define MCU_GPO2_GPIO_Port GPIOC
#define MCU_GPO2_EXTI_IRQn EXTI15_10_IRQn
#define MCU_ID2_Pin GPIO_PIN_3
#define MCU_ID2_GPIO_Port GPIOC
#define ENCODE_ETR_Pin GPIO_PIN_0
#define ENCODE_ETR_GPIO_Port GPIOA
#define MCU_ID5_Pin GPIO_PIN_4
#define MCU_ID5_GPIO_Port GPIOA
#define OW1_Pin GPIO_PIN_5
#define OW1_GPIO_Port GPIOA
#define OW1_EXTI_IRQn EXTI9_5_IRQn
#define OW2_Pin GPIO_PIN_7
#define OW2_GPIO_Port GPIOA
#define OW2_EXTI_IRQn EXTI9_5_IRQn
#define MCU_ID3_Pin GPIO_PIN_1
#define MCU_ID3_GPIO_Port GPIOB
#define MCU_ID4_Pin GPIO_PIN_2
#define MCU_ID4_GPIO_Port GPIOB
#define DRDY_Pin GPIO_PIN_10
#define DRDY_GPIO_Port GPIOB
#define CS_ADC_Pin GPIO_PIN_11
#define CS_ADC_GPIO_Port GPIOB
#define LED_RED_Pin GPIO_PIN_6
#define LED_RED_GPIO_Port GPIOC
#define LED_GREEN_Pin GPIO_PIN_7
#define LED_GREEN_GPIO_Port GPIOC
#define ENCODE_CH1_Pin GPIO_PIN_15
#define ENCODE_CH1_GPIO_Port GPIOA
#define ENCODE_CH2_Pin GPIO_PIN_3
#define ENCODE_CH2_GPIO_Port GPIOB
#define LED_BLUE_Pin GPIO_PIN_9
#define LED_BLUE_GPIO_Port GPIOB

/* USER CODE BEGIN Private defines */

/* USER CODE END Private defines */

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */
