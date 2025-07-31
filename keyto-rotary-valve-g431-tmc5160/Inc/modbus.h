#ifndef __MODBUS_H__
#define __MODBUS_H__

#include "main.h"
#include "flash_program.h"
#include "console.h"
#include <stdio.h>
#include "valve_control.h"

typedef struct
{
	uint16_t index;
	uint8_t device_addr;
	uint8_t authority;
	uint16_t reg_addr;
	int16_t data;
	uint16_t checksum;
}ModbusStruct_t;

typedef int (*func)(ModbusStruct_t*);

typedef enum
{
	ONLY_WRITE = 0,
	ONLY_READ,
	WRITE_READ
}Access_t;

typedef enum
{
	U8 = 0,
	S16,
	U16
}TypeSize_t;


struct Modbus_Register_t
{
	uint16_t reg;
	Access_t authority;
	TypeSize_t size;
	uint16_t floor;
	uint16_t upper;
	void *data;
	func callback;
	struct Modbus_Register_t *nextnode;
};

typedef struct
{
	uint8_t num;
	struct Modbus_Register_t Modbus_Register[256];
}Modbus_List_t;




#define 	MODBUS_READ			0x03
#define		MODBUS_WRITE		0x06


void ModBusAnalysis(uint8_t *data,uint8_t len);
void ModbusCmdInit(void);
#endif

