/*
 * register.c
 *
 *  Created on: Sep 4, 2025
 *      Author: 莫海峰
 */

/*
 * register.c
 *
 *  Created on: Jun 13, 2025
 *      Author: 莫海峰
 */
#include "register.h"
#include "flash.h"
#include "cmd.h"

// 寄存器列表
// 地址 | 权限            | 类型   | 数据指针               | 最小值     | 最大值       | 默认值     |是否可以掉点保存| 回调函数
//@formatter:off
static RegConfigTypedef reg_user_list[] =
{
	/*系统配置参数*/
	REG_CONFIG( 1, READ_WRITE, REG_U8, &sysconfig.RunStatus,{.u8v = 0},{.u8v = 0xFF},{.u8v = 0},SAVE_DISABLE ,NULL ), //状态
	REG_CONFIG( 2, READ_WRITE, REG_U8, &sysconfig.AutoAgingEnable,{.u8v = 0},{.u8v = 1},{.u8v = 0},SAVE_ENABLE, NULL ), //自动老化使能
	REG_CONFIG( 3, READ_WRITE, REG_U8,&sysconfig.AutoResetZeroEn,{.u8v = 0},{.u8v = 1},{.u8v = 0},SAVE_ENABLE, NULL), //自动置零使能


	REG_CONFIG( 4,READ_WRITE, REG_U8, &sysconfig.CommunicationConfig.Add,{.u8v = 0},{.u8v = 0xFF},{.u8v = 0},SAVE_ENABLE,NULL ), //地址
	REG_CONFIG( 5, READ_WRITE, REG_U16, &sysconfig.CommunicationConfig.CAN_BaudRate,{.u16v = 0},{.u16v = 1000},{.u16v = 500}, SAVE_ENABLE,WriteCANBaudrateCallback ),//CAN波特率
	REG_CONFIG( 6, READ_WRITE, REG_U32, &sysconfig.CommunicationConfig.RS232_BaudRate,{.u32v = 9600},{.u32v = 115200},{.u32v = 38400},SAVE_ENABLE, WriteUsartBaudrateCallback ),//串口波特率
	REG_CONFIG( 7, READ_WRITE, REG_U32, &sysconfig.CommunicationConfig.RS485_BaudRate,{.u32v = 9600},{.u32v = 115200},{.u32v = 38400},SAVE_ENABLE, WriteUsartBaudrateCallback ),//串口波特率
	REG_CONFIG( 8, READ_WRITE, REG_U8, &sysconfig.CommunicationConfig.protocol_type,{.u8v = 0},{.u8v = 2},{.u8v = 0},SAVE_DISABLE, NULL ),//协议类型
	REG_CONFIG( 9, READ_ONLY, REG_U32, &sysconfig.model,{.u32v = 0},{.u32v = 0xFFFFFFFF},{.u32v = SYS_FACILITY_ID}, SAVE_DISABLE,NULL ),//设备类型
	REG_CONFIG( 10, READ_ONLY, REG_U32, &sysconfig.version,{.u32v = 0},{.u32v = 0xFFFFFFFF},{.u32v = SYS_SOFT_VERSIONS}, SAVE_DISABLE,NULL ),//软件版本
	REG_CONFIG( 11, READ_WRITE, REG_U32, &sysconfig.CommunicationConfig.CAN_Heart,{.u32v = 0},{.u32v = 0xFFFFFFFF},{.u32v = 1000}, SAVE_ENABLE,NULL ),//软件版本
	/*电机配置参数*/
	REG_CONFIG( 20, READ_WRITE, REG_U8, &sysconfig.MotorValveConfig.StopMode,{.u8v = 0},{.u8v = 0x1},{.u8v = 0}, SAVE_ENABLE,NULL ),//软件版本
	REG_CONFIG( 21, READ_WRITE, REG_U16, &sysconfig.MotorValveConfig.MicroStep,{.u16v = 0},{.u16v = 256},{.u16v = 32}, SAVE_ENABLE,NULL ),//软件版本
	REG_CONFIG( 22, READ_WRITE, REG_U16, &sysconfig.MotorValveConfig.PhaseCurrent,{.u16v = 0},{.u16v = 2300},{.u16v = 1000}, SAVE_ENABLE,NULL ),//软件版本
	REG_CONFIG( 23, READ_WRITE, REG_U16, &sysconfig.MotorValveConfig.PhaseVoltage,{.u16v = 0},{.u16v = 2300},{.u16v = 1000}, SAVE_ENABLE,NULL ),//软件版本
	REG_CONFIG( 24, READ_WRITE, REG_U32, &sysconfig.MotorValveConfig.StartSpeed,{.u32v = 0},{.u32v = 0xFFFFFFFF},{.u32v = 100}, SAVE_ENABLE,NULL ),//软件版本
	REG_CONFIG( 25, READ_WRITE, REG_U32, &sysconfig.MotorValveConfig.Acceleration,{.u32v = 0},{.u32v = 0xFFFFFFFF},{.u32v = 10000}, SAVE_ENABLE,NULL ),//软件版本
	REG_CONFIG( 26, READ_WRITE, REG_U32, &sysconfig.MotorValveConfig.Deceleration,{.u32v = 0},{.u32v = 0xFFFFFFFF},{.u32v = 10000}, SAVE_ENABLE,NULL ),//软件版本
	REG_CONFIG( 27, READ_WRITE, REG_U32, &sysconfig.MotorValveConfig.RunSpeed,{.u32v = 0},{.u32v = 0xFFFFFFFF},{.u32v = 0}, SAVE_DISABLE,NULL ),//软件版本
	REG_CONFIG( 28, READ_WRITE, REG_U32, &sysconfig.MotorValveConfig.MaxSpeed,{.u32v = 0},{.u32v = 0xFFFFFFFF},{.u32v = 3000}, SAVE_ENABLE,NULL ),//软件版本
	REG_CONFIG( 29, READ_WRITE, REG_U32, &sysconfig.MotorValveConfig.StopSpeed,{.u32v = 0},{.u32v = 0xFFFFFFFF},{.u32v = 200}, SAVE_ENABLE,NULL ),//软件版本
	REG_CONFIG( 30, READ_WRITE, REG_U8, &sysconfig.MotorValveConfig.PositionDir,{.u8v = 0},{.u8v = 1},{.u32v = 0}, SAVE_ENABLE,NULL ),//软件版本
	REG_CONFIG( 31, READ_WRITE, REG_U8, &sysconfig.MotorValveConfig.PositionDir,{.u8v = 0},{.u8v = 1},{.u32v = 0}, SAVE_ENABLE,NULL ),//软件版本

	/*通讯配置参数*/

	/*********/

	/*********/
};
//@formatter:on
//初始化寄存器表，不能掉点保存的寄存器恢复默认值
void Init_Registers(void)
{
	for (uint8_t i = 0; i < REG_LIST_SIZE; i++)
	{
		RegConfigTypedef *node = &reg_user_list[i];
		if (node->save == SAVE_DISABLE)
		{
			// 设置默认值
			switch (node->data_type)
			{
				case REG_U8:
					*(uint8_t*) node->data_ptr = node->default_value.u8v;
					break;
				case REG_U16:
					*(uint16_t*) node->data_ptr = node->default_value.u16v;
					break;
				case REG_U32:
					*(uint32_t*) node->data_ptr = node->default_value.u32v;
					break;
				case REG_I8:
					*(int8_t*) node->data_ptr = node->default_value.i8v;
					break;
				case REG_I16:
					*(int16_t*) node->data_ptr = node->default_value.i16v;
					break;
				case REG_I32:
					*(int32_t*) node->data_ptr = node->default_value.i32v;
					break;
				default:
					break;
			};
		}
	}
}

static RegConfigTypedef* Find_Reg_Node(uint16_t addr)
{
	for (size_t i = 0; i < REG_LIST_SIZE; i++)
	{
		if (reg_user_list[i].addr == addr)
		{
			return &reg_user_list[i];
		}
	}
	return NULL;
}

// 读取寄存器值
Sys_Status_t Read_Register(uint16_t addr, RegValue_Handle_t *regval)
{
	RegConfigTypedef *node = Find_Reg_Node(addr);
	if (!node)
	{
		return REG_ERROR; // 寄存器地址错误
	}

	if (!(node->permission & READ_ONLY))
	{
		return READ_WRITE_ONLY; // 只写
	}
	switch (node->data_type)
	{
		case REG_U8:
			regval->value.u8v = *(uint8_t*) node->data_ptr;
			regval->val_type = REG_U8;
			break;
		case REG_U16:
			regval->value.u16v = *(uint16_t*) node->data_ptr;
			regval->val_type = REG_U16;
			break;
		case REG_U32:
			regval->value.u32v = *(uint32_t*) node->data_ptr;
			regval->val_type = REG_U32;
			break;
		case REG_I8:
			regval->value.i8v = *(int8_t*) node->data_ptr;
			regval->val_type = REG_I8;
			break;
		case REG_I16:
			regval->value.i16v = *(int16_t*) node->data_ptr;
			regval->val_type = REG_I16;
			break;
		case REG_I32:
			regval->value.i32v = *(int32_t*) node->data_ptr;
			regval->val_type = REG_I32;
			break;
		default:
			return PARAMETER_ERROR; // 不支持的数据类型
	}
	return EXECUTE_SUCCESS;
}

// 写入寄存器值
Sys_Status_t Write_Register(uint16_t addr, int32_t new_value)
{
	RegConfigTypedef *node = Find_Reg_Node(addr);
	Sys_Status_t state = EXECUTE_SUCCESS;
	if (!node)
	{
		return REG_ERROR;
	}
	if (!(node->permission & WRITE_ONLY))
	{
		return READ_WRITE_ONLY;
	}
	if (node->callback)
	{
		if (!node->callback(new_value))
			return OVER_LIMIT;
	}
	// 检查值范围
	switch (node->data_type)
	{
		case REG_U8:
			if (new_value < node->min_value.u8v || new_value > node->max_value.u8v)
				state = OVER_LIMIT; // 超限
			else
				*(uint8_t*) node->data_ptr = new_value;
			break;
		case REG_U16:
			if (new_value < node->min_value.u16v || new_value > node->max_value.u16v)
				state = OVER_LIMIT; // 超限
			else
				*(uint16_t*) node->data_ptr = new_value;
			break;
		case REG_U32:
			if (new_value < node->min_value.u32v || new_value > node->max_value.u32v)
				state = OVER_LIMIT; // 超限
			else
				*(uint32_t*) node->data_ptr = new_value;
			break;
		case REG_I8:
			if (new_value < node->min_value.i8v || new_value > node->max_value.i8v)
				state = OVER_LIMIT; // 超限
			else
				*(int8_t*) node->data_ptr = new_value;
			break;
		case REG_I16:
			if (new_value < node->min_value.i16v || new_value > node->max_value.i16v)
				state = OVER_LIMIT; // 超限
			else
				*(int16_t*) node->data_ptr = new_value;
			break;
		case REG_I32:
			if (new_value < node->min_value.i32v || new_value > node->max_value.i32v)
				state = OVER_LIMIT; // 超限
			else
				*(int32_t*) node->data_ptr = new_value;
			break;
		default:
			state = PARAMETER_ERROR; // 参数错误
			break;
	}
	return state;
}

Sys_Status_t Save_Register(void)
{
	FlashData_t flash_data;
	Erase_Config_Sector();
	flash_data.flash_sysconfig = sysconfig;
	if (Write_Config_Flash(&flash_data) != HAL_OK)
	{
		return FLASH_ERROR;
	}
	return EXECUTE_SUCCESS;
}

Sys_Status_t Reset_Register(void)
{
	for (uint8_t i = 0; i < REG_LIST_SIZE; i++)
	{
		RegConfigTypedef *node = &reg_user_list[i];
		// 设置默认值
		switch (node->data_type)
		{
			case REG_U8:
				*(uint8_t*) node->data_ptr = node->default_value.u8v;
				break;
			case REG_U16:
				*(uint16_t*) node->data_ptr = node->default_value.u16v;
				break;
			case REG_U32:
				*(uint32_t*) node->data_ptr = node->default_value.u32v;
				break;
			case REG_I8:
				*(int8_t*) node->data_ptr = node->default_value.i8v;
				break;
			case REG_I16:
				*(int16_t*) node->data_ptr = node->default_value.i16v;
				break;
			case REG_I32:
				*(int32_t*) node->data_ptr = node->default_value.i32v;
				break;
			default:
				break;
		};
	}
	return Save_Register();
}

uint8_t WriteUsartBaudrateCallback(int32_t new_value)
{
	return IS_USART_BAUD_RATE(new_value);
}

uint8_t WriteCANBaudrateCallback(int32_t new_value)
{
	return IS_CAN_BAUD_RATE(new_value);
}

