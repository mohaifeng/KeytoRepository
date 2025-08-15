/*
 * protocol.c
 *
 *  Created on: Mar 26, 2025
 *      Author: 莫海峰
 */
#include "protocol.h"
#include <string.h>
#include "user_usart.h"
#include "cmd.h"
#include "dev.h"
#include "verification.h"

void ResolutionProtocol(uint8_t *data, uint8_t len)
{
	uint8_t i = 0;
	while (i + 8 <= len)
	{
		if (data[i] == 0xAA)
		{
			UART_ResolutionGeneral_Protocol(&data[i], 8);
		}
		i++;
	}
}

HAL_StatusTypeDef UART_ResolutionGeneral_Protocol(uint8_t *data, uint8_t len)
{
	uint8_t sum = 0;
	int32_t value = 0;
	GeneralProtocol_Handle_t protocol;
	Cmd_Par_t temp;
	if (len < 8)
		return HAL_ERROR;
	protocol.head = data[0];
	if (protocol.head != 0xAA)
		return HAL_ERROR;
	protocol.add = data[1];
	if (protocol.add != sysconfig.CommunicationConfig.Add)
		return HAL_ERROR;
	protocol.cmd = data[2];
	memcpy(protocol.data, &(data[3]), 4);
	value = ((uint32_t) (protocol.data[0]) << 24) | ((uint32_t) (protocol.data[1]) << 16)
			| ((uint32_t) (protocol.data[2]) << 8) | ((uint32_t) (protocol.data[3]));
	protocol.verify = data[7];
	sum = Checksum_8(data, 7);
	if (sum != protocol.verify)
		return HAL_ERROR;
	temp.cmd = protocol.cmd;
	temp.value = value;
	Cmd_List_Append(temp);
	return HAL_OK;
}
