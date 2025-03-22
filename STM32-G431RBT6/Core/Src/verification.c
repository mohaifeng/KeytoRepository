/*
 * verification.c
 *
 *  Created on: Mar 19, 2025
 *      Author: 莫海峰
 */
#include "verification.h"

uint8_t Checksum_8(uint8_t *data, uint32_t length)
{
	uint8_t checksum = 0;
	for (uint32_t i = 0; i < length; i++)
	{
		checksum += data[i];
	}
	// 返回和的低8位
	return checksum & 0xFF;
}
