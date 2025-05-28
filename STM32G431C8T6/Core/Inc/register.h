/*
 * register.h
 *
 *  Created on: May 14, 2025
 *      Author: 莫海峰
 */

#ifndef INC_REGISTER_H_
#define INC_REGISTER_H_

#include "stm32g4xx_hal.h"
#include "main.h"


#define REG_LIST_SIZE	 (sizeof(reg_user_list)/sizeof(RegConfigTypedef))
#define REG_CONFIG(addr,permission,data_type,data_ptr,min_value,max_value,default_value,save,callback) {addr,permission,data_type,data_ptr,min_value,max_value,default_value,save,callback}

//定义寄存器读写权限
typedef enum
{
	READ_ONLY = 1,
	WRITE_ONLY,
	READ_WRITE
} RegRWPer_t;
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
	REG_FLOAT
} RegDataTypedef;

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

typedef void (*RegWriteCallback)(void *reg_ptr, RegValue new_value); //定义写寄存器值函数指针

typedef struct
{
	uint16_t addr;              // 寄存器地址
	RegRWPer_t permission;   // 读写权限
	RegDataTypedef data_type;      // 数据类型
	void *data_ptr;             // 数据存储指针
	RegValue min_value;         // 最小值
	RegValue max_value;         // 最大值
	RegValue default_value;     // 默认值
	RegSavePer_t save;				//掉点保存权限
	RegWriteCallback callback;       // 写操作回调函数
} RegConfigTypedef;

void Init_Registers(void);
HAL_StatusTypeDef Read_Register(uint16_t addr, RegValue *out_value);
HAL_StatusTypeDef Write_Register(uint16_t addr, RegValue new_value);
HAL_StatusTypeDef Save_Register(void);
#endif /* INC_REGISTER_H_ */
