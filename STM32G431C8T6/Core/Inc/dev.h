/*
 * dev.h
 *
 *  Created on: Mar 31, 2025
 *      Author: 莫海峰
 */

#ifndef INC_DEV_H_
#define INC_DEV_H_

#include "main.h"

typedef struct
{
	uint8_t addr;
	uint8_t status;
} DEV_TYPEDEF;

void Dev_Init(void);
void Dev_Change_Addr(uint8_t addr);
void Dev_Change_Status(uint8_t status);

#endif /* INC_DEV_H_ */
