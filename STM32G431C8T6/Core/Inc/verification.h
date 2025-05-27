/*
 * verification.h
 *
 *  Created on: Mar 26, 2025
 *      Author: 莫海峰
 */

#ifndef INC_VERIFICATION_H_
#define INC_VERIFICATION_H_

#include "stm32g4xx_hal.h"
#include "string.h"

uint8_t Checksum_8(const uint8_t *data,uint32_t length);
uint32_t Calculate_Crc32(const uint8_t *data, size_t length);

#endif /* INC_VERIFICATION_H_ */
