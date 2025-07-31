#ifndef __PROTOCOL_H_
#define __PROTOCOL_H_

#include <stdlib.h>
#include "main.h"
#include "string.h"
#include "stm32g4xx_hal.h"
#include "stm32g4xx_hal_fdcan.h"
#include "delay.h"
#include "console.h"
#include "flash_program.h"
#include "valve_control.h"
#include "kconfig.h"

typedef struct
{
  uint8_t Head;         //头
  uint8_t Add;          //地址
  uint8_t Cmd;          //命令
  uint8_t code[4];
  uint8_t data[4];     //数据
  uint8_t end;          //尾
  uint16_t verify;      //校验
} Protocol_Handle_t;
typedef struct
{
  uint8_t Head;             //头
  uint8_t Add;              //地址
  uint8_t cmd;       				//指令   
  uint8_t data[4];        	//数据
  uint16_t verify;          //校验
} GeneralProtocol_Handle_t;

#define CMD_LIST_MAX 32
typedef struct
{
	/*通用部分*/
	uint8_t cmd;
	int32_t value;
	#if MASTER_BOARD
	
	#endif
}serial_t;

typedef struct
{
	serial_t cmd[CMD_LIST_MAX];
	uint8_t num;
	uint8_t head;
	uint8_t tail;
}cmd_list_t;

extern uint8_t SendTitanFlag;


void Scan_Resolution(uint8_t *data , uint8_t len);
void UART_ReturnGeneralData(uint8_t add,int data);
int ResolutionProtocol(uint8_t Mode,uint8_t *data , uint8_t len);
int ResolutionSmartSV_Protocol(uint8_t Mode,uint8_t *data , uint8_t len);
int UART_ResolutionGeneral_Protocol(uint8_t *data , uint8_t len);
void SetProtocolAdd(uint8_t add);
uint8_t CheckSysStatus(void);
void ResponseTiTan_Send(char *Data,uint8_t len);
void SetRS232BaudRate(uint8_t baud);
void SetRS485BaudRate(uint8_t baud);
HAL_StatusTypeDef CanSendStdData(uint32_t id, uint8_t *data,uint8_t len);
int ResolutionTiTan_Protocol(uint8_t Mode,uint8_t *data , uint8_t len);
void TiTanResponse(void);
int CAN_ResolutionGeneral_Protocol(uint8_t *data , uint8_t len);
void CAN_ReturnGeneralData(uint16_t add,int data);
void ResponseTiTan_I2C_Send(char *Data,uint8_t len);
int ResolutionTiTan_I2C_Protocol(uint8_t *data , uint8_t len);
void ResponseSmartSV_Send(uint8_t status , uint16_t data);
void ResponseTiTan_Send(char *Data,uint8_t len);
void ResponseSmartSV_UserData_Send(uint8_t status);
bool SI2C_CheckFlashSaveFlag(void);
void SI2C_FlashSave_Task(void);
void SI2C_Control_Task(void);
bool Slave_IIC_Protocol(uint8_t *data);


void cmd_list_clear(void);
int cmd_list_append(serial_t cmd);
void cmd_list_execute(void);
#endif
