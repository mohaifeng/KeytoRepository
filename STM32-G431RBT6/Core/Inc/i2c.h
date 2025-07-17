/* USER CODE BEGIN Header */
/**
 ******************************************************************************
 * @file    i2c.h
 * @brief   This file contains all the function prototypes for
 *          the i2c.c file
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
#ifndef __I2C_H__
#define __I2C_H__

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "main.h"

/* USER CODE BEGIN Includes */
#define EEPROM_BASE_ADDR    0x0000 //0x0000-0x07FF
#define EEPROM_PAGE_SIZE    16
#define EEPROM_MEMADD_SIZE 	I2C_MEMADD_SIZE_8BIT  // 8 位地址宽度
#define EEPROM_TIMEOUT 	    5
#define EEPOM_ADDR(ADDR_16) ((0x50 | (ADDR_16 >> 8)) << 1)//计算ST24C16设备地址
#define EEPROM_MEM_ADDR(ADDR_16)   (ADDR_16 & 0xFF)//计算ST24C16存储地址
/* USER CODE END Includes */

extern I2C_HandleTypeDef hi2c3;

/* USER CODE BEGIN Private defines */

/* USER CODE END Private defines */

void MX_I2C3_Init(void);

/* USER CODE BEGIN Prototypes */
void EEPROM_Init();
HAL_StatusTypeDef EEPROM_WriteBuf_DMA(uint16_t memaddr, uint8_t *pdata, uint16_t size);
HAL_StatusTypeDef EEPROM_ReadBuf_DMA(uint16_t memaddr, uint8_t *pdata, uint16_t size);
/* USER CODE END Prototypes */

#ifdef __cplusplus
}
#endif

#endif /* __I2C_H__ */

