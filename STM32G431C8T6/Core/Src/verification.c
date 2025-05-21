/*
 * verification.c
 *
 *  Created on: Mar 26, 2025
 *      Author: 莫海峰
 */
#include "verification.h"

uint8_t Checksum_8(const uint8_t *data, uint32_t length)
{
	uint8_t checksum = 0;
	for (uint32_t i = 0; i < length; i++)
	{
		checksum += data[i];
	}
	// 返回和的低8位
	return checksum & 0xFF;
}

uint32_t Calculate_Crc32(const uint8_t *data, size_t length)
{
	__HAL_RCC_CRC_CLK_ENABLE();
	CRC->POL = 0x04C11DB7;  // 标准CRC32多项式
	CRC->CR = CRC_CR_RESET;  // 复位CRC计算器

	// 以32位为单位处理数据
	while (length >= 4)
	{
		CRC->DR = *((uint32_t*) data);
		data += 4;
		length -= 4;
	}

	// 处理剩余字节
	if (length > 0)
	{
		uint32_t temp = 0;
		memcpy(&temp, data, length);
		CRC->DR = temp;
	}

	return CRC->DR;
}


