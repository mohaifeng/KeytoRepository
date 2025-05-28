/*
 * flash.c
 *
 *  Created on: May 19, 2025
 *      Author: 莫海峰
 */
#include "flash.h"
#include "stdlib.h"
#include "usart.h"

uint8_t flash_w_flag = 0;

//清除数据
void Erase_Config_Sector(void)
{
	HAL_FLASH_Unlock();
	FLASH_EraseInitTypeDef erase;
	erase.TypeErase = FLASH_TYPEERASE_PAGES;
	erase.Page = CONFIG_FLASH_PAGE;  // STM32G431C8T6最后一页(页63)
	erase.NbPages = 1;
	uint32_t pageError;
	if (HAL_FLASHEx_Erase(&erase, &pageError) != HAL_OK)
	{
		// 擦除失败处理
		Error_Handler();
	}
	HAL_FLASH_Lock();
}

//写数据到flash
HAL_StatusTypeDef Write_Config_Flash(FlashData_t *config)
{
	// 计算CRC(不包括crc字段本身)
	config->crc = Calculate_Crc32((uint8_t*) config, sizeof(FlashData_t) - 4);
	// 擦除Flash页
	Erase_Config_Sector();
	// 写入数据
	HAL_FLASH_Unlock();
	__HAL_FLASH_CLEAR_FLAG(FLASH_FLAG_ALL_ERRORS);

	uint64_t *src = (uint64_t*) config;
	uint64_t *dst = (uint64_t*) CONFIG_FLASH_ADDR;
	uint32_t doublewords = (sizeof(FlashData_t) + 7) / 8;  // 计算双字数

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
//从flash读取数据
HAL_StatusTypeDef Read_Config_Flash(FlashData_t *config)
{
	const FlashData_t *flash_config = (const FlashData_t*) CONFIG_FLASH_ADDR;
	// 验证CRC
	uint32_t calc_crc = Calculate_Crc32((uint8_t*) flash_config, sizeof(FlashData_t) - sizeof(uint32_t));
	if (calc_crc != flash_config->crc || flash_config->crc == 0xFF)  //crc不对或没有存储数据
	{
		return HAL_ERROR;
	}
	// 复制有效数据
	memcpy(config, flash_config, sizeof(FlashData_t));
	return HAL_OK;
}

//// 写入任意结构体到Flash
//HAL_StatusTypeDef Write_Struct_To_Flash(DataType data_type, void *data, uint16_t data_size)
//{
//	FlashDataHeader header;
//	header.data_type = data_type;
//	header.data_size = data_size;
//	header.crc = Calculate_Crc32((uint8_t*) data, data_size); // 实现CRC计算函数
//	uint32_t addr = FLASH_LAST_PAGE_ADDR;
//	uint32_t end_addr = FLASH_LAST_PAGE_ADDR + FLASH_PAGE_SIZE;
//	// 查找第一个可用位置
//	while (addr < end_addr)
//	{
//		FlashDataHeader stored_header;
//		memcpy(&stored_header, (void*) addr, sizeof(FlashDataHeader));
//		if (stored_header.data_type == 0xFF)
//		{ // 空白区域
//			break;
//		}
//		addr += sizeof(FlashDataHeader) + stored_header.data_size;
//		if (addr + sizeof(FlashDataHeader) + data_size > end_addr)
//		{
//			return HAL_ERROR; // 空间不足
//		}
//	}
//	// 开始写入
//	HAL_FLASH_Unlock();
//	// 写入头
//	uint64_t *header_ptr = (uint64_t*) &header;
//	for (uint16_t i = 0; i < sizeof(FlashDataHeader); i += 8)
//	{
//		HAL_FLASH_Program(FLASH_TYPEPROGRAM_DOUBLEWORD, addr + i, *(header_ptr + i / 8));
//	}
//	// 写入数据
//	uint64_t *data_ptr = (uint64_t*) data;
//	uint16_t data_len = (data_size + 7) / 8; // 向上取整到8字节倍数
//	for (uint16_t i = 0; i < data_len; i++)
//	{
//		HAL_FLASH_Program(FLASH_TYPEPROGRAM_DOUBLEWORD, addr + sizeof(FlashDataHeader) + i * 8, *(data_ptr + i));
//	}
//	HAL_FLASH_Lock();
//	return HAL_OK;
//}
//
//// 从Flash读取特定类型的数据
//void* Read_Struct_From_Flash(DataType expected_type, uint16_t *out_size)
//{
//	uint32_t addr = FLASH_LAST_PAGE_ADDR;
//	while (addr < FLASH_LAST_PAGE_ADDR + FLASH_PAGE_SIZE)
//	{
//		FlashDataHeader header;
//		memcpy(&header, (void*) addr, sizeof(FlashDataHeader));
//		if (header.data_type == 0xFF)
//			break; // 空白区域
//		if (header.data_type == expected_type)
//		{
//			void *data = malloc(header.data_size);
//			if (data)
//			{
//				memcpy(data, (void*) (addr + sizeof(FlashDataHeader)), header.data_size);
//				// 校验CRC
//				if (Calculate_Crc32(data, header.data_size) == header.crc)
//				{
//					*out_size = header.data_size;
//					return data;
//				}
//				free(data);
//			}
//			return NULL;
//		}
//		addr += sizeof(FlashDataHeader) + header.data_size;
//	}
//	return NULL; // 未找到
//}
