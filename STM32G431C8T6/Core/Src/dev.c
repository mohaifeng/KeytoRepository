/*
 * dev.c
 *
 *  Created on: Mar 31, 2025
 *      Author: 莫海峰
 */

#include "dev.h"

DEV_TYPEDEF dev_instance;

void Dev_Init(void)
{
	dev_instance.addr = 0;
	dev_instance.status = 0;
}

void Dev_Change_Addr(uint8_t addr)
{
	dev_instance.addr = addr;
}

void Dev_Change_Status(uint8_t status)
{
	dev_instance.status = status;
}
