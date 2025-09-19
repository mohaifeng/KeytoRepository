/*
 * can.h
 *
 *  Created on: Sep 19, 2025
 *      Author: 莫海峰
 */

#ifndef INC_CAN_H_
#define INC_CAN_H_

#include "main.h"

typedef struct
{
	uint8_t tx_buff[8];
	FDCAN_TxHeaderTypeDef TxHeader;
} CAN_Data_Handle_t;

void User_CAN_Init(void);
void CAN_Transmit(void);

#endif /* INC_CAN_H_ */
