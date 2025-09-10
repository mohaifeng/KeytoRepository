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
	REG_CONFIG( 1, READ_WRITE, REG_U8, &sysconfig.RunStatus,{.u8v = 0},{.u8v = 0xFF},{.u8v = 0},SAVE_DISABLE ,NULL ), //状态
	REG_CONFIG( 2, READ_WRITE, REG_U8, &sysconfig.AutoAgingEnable,{.u8v = 0},{.u8v = 1},{.i32v = 0},SAVE_ENABLE, NULL ), //探测到液面标志
	REG_CONFIG( 3, READ_WRITE, REG_U8,&sysconfig.AutoResetZeroEn,{.u8v = 0},{.u8v = 1},{.u8v = 0},SAVE_ENABLE, NULL), //有无tip
	REG_CONFIG( 4,READ_WRITE, REG_U8, &sysconfig.CommunicationConfig.Add,{.u8v = 0},{.u8v = 0xFF},{.i32v = 0},SAVE_ENABLE,NULL ), //当前气压值
	REG_CONFIG( 5, READ_WRITE, REG_U16, &sysconfig.CommunicationConfig.CAN_BaudRate,{.u16v = 0},{.u16v = 1000},{.u16v = 500}, SAVE_ENABLE,NULL ),//GPO1输出配置
	REG_CONFIG( 6, READ_WRITE, REG_U32, &sysconfig.CommunicationConfig.RS232_BaudRate,{.u32v = 9600},{.u32v = 115200},{.u32v = 38400},SAVE_ENABLE, NULL ),//串口波特率
	REG_CONFIG( 7, READ_WRITE, REG_U32, &sysconfig.CommunicationConfig.RS485_BaudRate,{.u32v = 9600},{.u32v = 115200},{.u32v = 38400},SAVE_ENABLE, NULL ),//串口波特率
	REG_CONFIG( 8, READ_WRITE, REG_U8, &sysconfig.CommunicationConfig.protocol_type,{.u8v = 0},{.u8v = 2},{.u8v = 0},SAVE_DISABLE, NULL ),//协议类型
	REG_CONFIG( 9, READ_ONLY, REG_U32, &sysconfig.model,{.u32v = 0},{.u32v = 0xFFFFFFFF},{.u32v = SYS_FACILITY_ID}, SAVE_DISABLE,NULL ),//设备类型
	REG_CONFIG( 10, READ_ONLY, REG_U32, &sysconfig.version,{.u32v = 0},{.u32v = 0xFFFFFFFF},{.u32v = SYS_SOFT_VERSIONS}, SAVE_DISABLE,NULL ),//软件版本

};
//@formatter:on
void Init_Registers(void)
{
	FlashData_t flash_data;
	if (Read_Config_Flash(&flash_data) != HAL_OK)
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
				case REG_FLOAT:
					*(float*) node->data_ptr = node->default_value.fv;
					break;
				default:
					break;
			};
		}

	}
	else
	{
		memcpy(&sysconfig, &flash_data.flash_sysconfig, sizeof(SysConfig_t));
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
					case REG_FLOAT:
						*(float*) node->data_ptr = node->default_value.fv;
						break;
					default:
						break;
				};
			}
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
		case REG_FLOAT:
			regval->value.fv = *(float*) node->data_ptr;
			regval->val_type = REG_FLOAT;
			break;
		default:
			return PARAMETER_ERROR; // 不支持的数据类型
	}
	return EXECUTE_SUCCESS;
}

// 写入寄存器值
Sys_Status_t Write_Register(uint16_t addr, RegValue *new_value)
{
	RegConfigTypedef *node = Find_Reg_Node(addr);
	if (!node)
	{
		return REG_ERROR;
	}
	if (!(node->permission & WRITE_ONLY))
	{
		return READ_WRITE_ONLY;
	}
	// 检查值范围
	switch (node->data_type)
	{
		case REG_U8:
			if (new_value->u8v < node->min_value.u8v || new_value->u8v > node->max_value.u8v)
			{
				return OVER_LIMIT; // 寄存器地址错误
			}
			*(uint8_t*) node->data_ptr = new_value->u8v;
			break;
		case REG_U16:
			if (new_value->u16v < node->min_value.u16v || new_value->u16v > node->max_value.u16v)
			{
				return OVER_LIMIT; // 寄存器地址错误
			}
			*(uint16_t*) node->data_ptr = new_value->u16v;
			break;
		case REG_U32:
			if (new_value->u32v < node->min_value.u32v || new_value->u32v > node->max_value.u32v)
			{
				return OVER_LIMIT; // 寄存器地址错误
			}
			*(uint32_t*) node->data_ptr = new_value->u32v;
			break;
		case REG_I8:
			if (new_value->i8v < node->min_value.i8v || new_value->i8v > node->max_value.i8v)
			{
				return OVER_LIMIT; // 寄存器地址错误
			}
			*(int8_t*) node->data_ptr = new_value->i8v;
			break;
		case REG_I16:
			if (new_value->i16v < node->min_value.i16v || new_value->i16v > node->max_value.i16v)
			{
				return OVER_LIMIT; // 寄存器地址错误
			}
			*(int16_t*) node->data_ptr = new_value->i16v;
			break;
		case REG_I32:
			if (new_value->i32v < node->min_value.i32v || new_value->i32v > node->max_value.i32v)
			{
				return OVER_LIMIT; // 寄存器地址错误
			}
			*(int32_t*) node->data_ptr = new_value->i32v;
			break;
		case REG_FLOAT:
			if (new_value->fv < node->min_value.fv || new_value->fv > node->max_value.fv)
			{
				return OVER_LIMIT; // 寄存器地址错误
			}
			*(float*) node->data_ptr = new_value->fv;
			break;
		default:
			return PARAMETER_ERROR; // 寄存器地址错误
	}
	// 调用回调函数
	if (node->callback)
	{
		node->callback(node->data_ptr, new_value);
	}
	return EXECUTE_SUCCESS; // 寄存器地址错误
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

