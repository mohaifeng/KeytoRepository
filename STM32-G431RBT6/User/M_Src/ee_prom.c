/*
 * ee_prom.c
 *
 *  Created on: Jul 22, 2025
 *      Author: 莫海峰
 */
#include "ee_prom.h"
#include "i2c.h"

void EEPROM_Init()
{
	if (HAL_I2C_IsDeviceReady(&hi2c3, EEPOM_ADDR(0), 10, 1000) != HAL_OK)
	{
		SysConfig.status = EEPROM_ERROR;
	}
}
//memaddr范围：0-0x07ff
HAL_StatusTypeDef EEPROM_WriteBuf_DMA(uint16_t memaddr, uint8_t *pdata, uint16_t size)
{
	HAL_StatusTypeDef status;
	if (pdata == NULL || size == 0)
		return HAL_ERROR;
	while (size > 0)
	{
		// 1. 计算当前页剩余空间
		uint8_t remaining = EEPROM_PAGE_SIZE - (memaddr % EEPROM_PAGE_SIZE);
		uint8_t chunk = (size > remaining) ? remaining : size;
		status = HAL_I2C_Mem_Write_DMA(&hi2c3, EEPOM_ADDR(memaddr), EEPROM_MEM_ADDR(memaddr), EEPROM_MEMADD_SIZE, pdata, chunk);
		if (status != HAL_OK)
			return status;
		// 3. 等待传输完成
		while (HAL_I2C_GetState(&hi2c3) != HAL_I2C_STATE_READY);
		// 4. 等待EEPROM内部写入完成
		HAL_Delay(EEPROM_TIMEOUT);
		// 5. 更新指针
		memaddr += chunk;
		pdata += chunk;
		size -= chunk;
	}
	return HAL_OK;
}

HAL_StatusTypeDef EEPROM_ReadBuf_DMA(uint16_t memaddr, uint8_t *pdata, uint16_t size)
{
	HAL_StatusTypeDef status;
	status = HAL_I2C_Mem_Read_DMA(&hi2c3, EEPOM_ADDR(memaddr), EEPROM_MEM_ADDR(memaddr), EEPROM_MEMADD_SIZE, pdata, size);
	if (status != HAL_OK)
		return status;
	while (HAL_I2C_GetState(&hi2c3) != HAL_I2C_STATE_READY);
	return HAL_OK;
}
