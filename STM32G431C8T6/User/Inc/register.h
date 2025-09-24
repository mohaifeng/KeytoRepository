/*
 * register.h
 *
 *  Created on: Sep 4, 2025
 *      Author: 莫海峰
 */

#ifndef INC_REGISTER_H_
#define INC_REGISTER_H_

#include "main.h"
#include "dev.h"

#define REG_LIST_SIZE	 (sizeof(reg_user_list)/sizeof(RegConfigTypedef))
#define REG_CONFIG(addr,permission,data_type,data_ptr,min_value,max_value,default_value,save,callback) {addr,permission,data_type,data_ptr,min_value,max_value,default_value,save,callback}
#define IS_USART_BAUD_RATE(__N__)	(((__N__)==9600)||((__N__)==19200)||((__N__)==38400)||((__N__)==115200))
#define IS_CAN_BAUD_RATE(__N__)   (((__N__)==100)||((__N__)==250)||((__N__)==500)||((__N__)==1000))
//定义寄存器读写权限
typedef enum
{
	READ_ONLY = 1,
	WRITE_ONLY,
	READ_WRITE
} RW_Handle_t;
//定义寄存器是否可以掉电保存权限
typedef enum
{
	SAVE_DISABLE = 0,
	SAVE_ENABLE,
} RegSavePer_t;
//定义寄存器值数据类型
typedef enum
{
	REG_U8,
	REG_U16,
	REG_U32,
	REG_I8,
	REG_I16,
	REG_I32,
} DataTypedef;

//定义寄存器值的数据类型
typedef union
{
	uint8_t u8v;
	uint16_t u16v;
	uint32_t u32v;
	int8_t i8v;
	int16_t i16v;
	int32_t i32v;
	float fv;
} RegValue;

typedef struct
{
	RegValue value;
	DataTypedef val_type;
} RegValue_Handle_t;

typedef uint8_t (*RegWriteCallback)(int32_t new_value); //定义写寄存器值函数指针

typedef struct
{
	uint16_t addr;              // 寄存器地址
	RW_Handle_t permission;   // 读写权限
	DataTypedef data_type;      // 数据类型
	void *data_ptr;             // 数据存储指针
	RegValue min_value;         // 最小值
	RegValue max_value;         // 最大值
	RegValue default_value;     // 默认值
	RegSavePer_t save;				//掉点保存权限
	RegWriteCallback callback;       // 写操作回调函数
} RegConfigTypedef;

void Init_Registers(void);
Sys_Status_t Read_Register(uint16_t addr, RegValue_Handle_t *regval);
Sys_Status_t Write_Register(uint16_t addr, int32_t new_value);
Sys_Status_t Save_Register(void);
Sys_Status_t Reset_Register(void);
uint8_t WriteUsartBaudrateCallback(int32_t new_value);
uint8_t WriteCANBaudrateCallback(int32_t new_value);

#endif /* INC_REGISTER_H_ */
