/*
 * flash.c
 *
 *  Created on: May 19, 2025
 *      Author: 莫海峰
 */

#include "flash.h"

uint8_t flash_w_flag = 0;
void Erase_Config_Sector(void)
{
	HAL_FLASH_Unlock();
	FLASH_EraseInitTypeDef erase;
	erase.TypeErase = FLASH_TYPEERASE_PAGES;
	erase.Page = 63;  // STM32G431C8T6最后一页(页63)
	erase.NbPages = 1;
	uint32_t pageError;
	if (HAL_FLASHEx_Erase(&erase, &pageError) != HAL_OK)
	{
		// 擦除失败处理
		Error_Handler();
	}
	HAL_FLASH_Lock();
}

HAL_StatusTypeDef Write_Config_Flash(SysConfig_t *config)
{
	// 计算CRC(不包括crc字段本身)
	config->crc = Calculate_Crc32((uint8_t*) config, sizeof(SysConfig_t) - 4);

	// 擦除Flash页
	Erase_Config_Sector();

	// 写入数据
	HAL_FLASH_Unlock();
	__HAL_FLASH_CLEAR_FLAG(FLASH_FLAG_ALL_ERRORS);

	uint64_t *src = (uint64_t*) config;
	uint64_t *dst = (uint64_t*) CONFIG_FLASH_ADDR;
	uint32_t doublewords = (sizeof(SysConfig_t) + 7) / 8;  // 计算双字数

	for (uint32_t i = 0; i < doublewords; i++)
	{
		if (HAL_FLASH_Program(FLASH_TYPEPROGRAM_DOUBLEWORD, (uint32_t) dst, *src) != HAL_OK)
		{
			HAL_FLASH_Lock();
			return HAL_ERROR;
		}
		dst++;
		src++;
	}

	HAL_FLASH_Lock();
	return HAL_OK;
}

HAL_StatusTypeDef Read_Config_Flash(SysConfig_t *config)
{
	const SysConfig_t *flash_config = (const SysConfig_t*) CONFIG_FLASH_ADDR;
	// 验证CRC
	uint32_t calc_crc = Calculate_Crc32((uint8_t*) flash_config, sizeof(SysConfig_t) - 4);
	if (calc_crc != flash_config->crc)
	{
		return HAL_ERROR;
	}
	// 检查型号是否匹配
	if (flash_config->model != DEV_TYPE)
	{
		return HAL_ERROR; // 型号不匹配，数据无效
	}
	// 复制有效数据
	memcpy(config, flash_config, sizeof(SysConfig_t));
	return HAL_OK;
}
