/*
 * ee_prom.h
 *
 *  Created on: Jul 22, 2025
 *      Author: 莫海峰
 */

#ifndef M_INC_EE_PROM_H_
#define M_INC_EE_PROM_H_

#include "stm32g4xx_hal.h"

#define EEPROM_BASE_ADDR    0x0000 //0x0000-0x07FF
#define EEPROM_PAGE_SIZE    16
#define EEPROM_MEMADD_SIZE 	I2C_MEMADD_SIZE_8BIT  // 8 位地址宽度
#define EEPROM_TIMEOUT 	    5
#define EEPOM_ADDR(ADDR_16) ((0x50 | (ADDR_16 >> 8)) << 1)//计算ST24C16设备地址
#define EEPROM_MEM_ADDR(ADDR_16)   (ADDR_16 & 0xFF)//计算ST24C16存储地址

void EEPROM_Init();
HAL_StatusTypeDef EEPROM_WriteBuf_DMA(uint16_t memaddr, uint8_t *pdata, uint16_t size);
HAL_StatusTypeDef EEPROM_ReadBuf_DMA(uint16_t memaddr, uint8_t *pdata, uint16_t size);

#endif /* M_INC_EE_PROM_H_ */
