#ifndef __CONSOLE_H_
#define __CONSOLE_H_

#include "kconfig.h"
#include "flash_program.h"
#include "valve_control.h"
#include "tmc5160.h"
#include <stdio.h>
#include "protocol.h"

#define CONSOLE_TASK_TIME 2


//Consloe_State_t GetConsoleStatus(void);
//void SetConsoleStatus(Consloe_State_t status);
void uartOpenReceive(void);
HAL_StatusTypeDef sentUartData(uint8_t *data,uint8_t len);
HAL_StatusTypeDef SendCommunicationData(uint8_t ComMode,uint8_t *data,uint8_t len);
uint8_t GetUartNum(void);
void ConsoleControlTask(void);

#endif
