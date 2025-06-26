/*
 * register.c
 *
 *  Created on: Jun 13, 2025
 *      Author: 莫海峰
 */
#include "register.h"
#include "flash.h"
#include "cmd.h"
#include "motor_control.h"

// 寄存器列表
// 地址 | 权限            | 类型   | 数据指针               | 最小值     | 最大值       | 默认值     |是否可以掉点保存| 回调函数
// @formatter:off
static RegConfigTypedef reg_user_list[] =
{
	REG_CONFIG( 1, READ_WRITE, REG_U8, &SysConfig.status,{.u8v = 0},{.u8v = 0xFF},{.u8v = 0},SAVE_DISABLE ,NULL ), //状态
	REG_CONFIG( 2, READ_ONLY, REG_U8, &PlldConfig.plld_flag,{.u8v = 0},{.u8v = 1},{.i32v = 0},SAVE_DISABLE, NULL ), //探测到液面标志
	REG_CONFIG( 3, READ_ONLY, REG_U8,&SysConfig.is_tip,{.u8v = 0},{.u8v = 1},{.u8v = 0},SAVE_DISABLE, setMotorDirCallback), //有无tip
	REG_CONFIG( 4,READ_ONLY, REG_I32, &SysConfig.pressure,{.i32v = 0x80000000},{.i32v = 0x7FFFFFFF},{.i32v = 0},SAVE_DISABLE,NULL ), //当前气压值
	REG_CONFIG( 10, READ_WRITE, REG_U8, &SysConfig.gpo1_out_mode,{.u8v = 0},{.u8v = 2},{.u8v = 0}, SAVE_ENABLE,NULL ),//GPO1输出配置
	REG_CONFIG( 80, READ_WRITE, REG_U32, &SysConfig.ser_baudrate,{.u32v = 9600},{.u32v = 115200},{.u32v = 38400},SAVE_ENABLE, NULL ),//串口波特率
	REG_CONFIG( 90, READ_ONLY, REG_U32, &SysConfig.version,{.u32v = 0},{.u32v = 0xFFFFFFFF},{.u32v = SOFTWARE_VERSION},SAVE_DISABLE, NULL ),//软件版本
	REG_CONFIG( 91, READ_ONLY, REG_U32, &SysConfig.model,{.u32v = 0},{.u32v = 0xFFFFFFFF},{.u32v = DEV_TYPE}, SAVE_DISABLE,NULL ),//设备类型
};
// @formatter:on
void Init_Registers(void)
{
	FlashData_t flash_data;
	if(Read_Config_Flash(&flash_data) != HAL_OK)
	{
		for(uint8_t i = 0; i < REG_LIST_SIZE; i++)
		{
			RegConfigTypedef *node = &reg_user_list[i];
			// 设置默认值
			switch(node->data_type)
				{
				case REG_U8:
					*(uint8_t*)node->data_ptr = node->default_value.u8v;
					break;
				case REG_U16:
					*(uint16_t*)node->data_ptr = node->default_value.u16v;
					break;
				case REG_U32:
					*(uint32_t*)node->data_ptr = node->default_value.u32v;
					break;
				case REG_I8:
					*(int8_t*)node->data_ptr = node->default_value.i8v;
					break;
				case REG_I16:
					*(int16_t*)node->data_ptr = node->default_value.i16v;
					break;
				case REG_I32:
					*(int32_t*)node->data_ptr = node->default_value.i32v;
					break;
				case REG_FLOAT:
					*(float*)node->data_ptr = node->default_value.fv;
					break;
				default:
					break;
				};
		}

	}
	else
	{
		memcpy(&SysConfig,&flash_data.flash_sysconfig,sizeof(SysConfig_t));
		memcpy(&PlldConfig,&flash_data.flash_plld_Config,sizeof(PlldConfig_t));
		memcpy(&PressureDectConfig,&flash_data.flash_pressuredect_Config,sizeof(PressureDectConfig_t));
		for(uint8_t i = 0; i < REG_LIST_SIZE; i++)
		{
			RegConfigTypedef *node = &reg_user_list[i];
			if(node->save == SAVE_DISABLE)
			{
				// 设置默认值
				switch(node->data_type)
					{
					case REG_U8:
						*(uint8_t*)node->data_ptr = node->default_value.u8v;
						break;
					case REG_U16:
						*(uint16_t*)node->data_ptr = node->default_value.u16v;
						break;
					case REG_U32:
						*(uint32_t*)node->data_ptr = node->default_value.u32v;
						break;
					case REG_I8:
						*(int8_t*)node->data_ptr = node->default_value.i8v;
						break;
					case REG_I16:
						*(int16_t*)node->data_ptr = node->default_value.i16v;
						break;
					case REG_I32:
						*(int32_t*)node->data_ptr = node->default_value.i32v;
						break;
					case REG_FLOAT:
						*(float*)node->data_ptr = node->default_value.fv;
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
	for(size_t i = 0; i < REG_LIST_SIZE; i++)
	{
		if(reg_user_list[i].addr == addr)
		{
			return &reg_user_list[i];
		}
	}
	return NULL;
}

// 读取寄存器值
HAL_StatusTypeDef Read_Register(uint16_t addr,RegValue *out_value)
{
	RegConfigTypedef *node = Find_Reg_Node(addr);
	if(!node)
	{
		cmd_finish_flag = REG_ERROR;
		return HAL_ERROR; // 寄存器地址错误
	}

	if(!(node->permission & READ_ONLY))
	{
		cmd_finish_flag = READ_WRITE_ONLY;
		return HAL_ERROR; // 寄存器地址错误
	}
	switch(node->data_type)
		{
		case REG_U8:
			out_value->u8v = *(uint8_t*)node->data_ptr;
			break;
		case REG_U16:
			out_value->u16v = *(uint16_t*)node->data_ptr;
			break;
		case REG_U32:
			out_value->u32v = *(uint32_t*)node->data_ptr;
			break;
		case REG_I8:
			out_value->i8v = *(int8_t*)node->data_ptr;
			break;
		case REG_I16:
			out_value->i16v = *(int16_t*)node->data_ptr;
			break;
		case REG_I32:
			out_value->i32v = *(int32_t*)node->data_ptr;
			break;
		case REG_FLOAT:
			out_value->fv = *(float*)node->data_ptr;
			break;
		default:
			cmd_finish_flag = PARAMETER_ERROR;
			return HAL_ERROR; // 不支持的数据类型
		}
	cmd_finish_flag = EXECUTE_SUCCESS;
	return HAL_OK;
}

// 写入寄存器值
HAL_StatusTypeDef Write_Register(uint16_t addr,RegValue *new_value)
{
	RegConfigTypedef *node = Find_Reg_Node(addr);
	if(!node)
	{
		cmd_finish_flag = REG_ERROR;
		return HAL_ERROR;
	}
	if(!(node->permission & WRITE_ONLY))
	{
		cmd_finish_flag = READ_WRITE_ONLY;
		return HAL_ERROR;
	}
	// 检查值范围
	switch(node->data_type)
		{
		case REG_U8:
			if(new_value->u8v < node->min_value.u8v || new_value->u8v > node->max_value.u8v)
			{
				cmd_finish_flag = OVER_LIMIT;
				return HAL_ERROR; // 寄存器地址错误
			}
			*(uint8_t*)node->data_ptr = new_value->u8v;
			break;
		case REG_U16:
			if(new_value->u16v < node->min_value.u16v || new_value->u16v > node->max_value.u16v)
			{
				cmd_finish_flag = OVER_LIMIT;
				return HAL_ERROR; // 寄存器地址错误
			}
			*(uint16_t*)node->data_ptr = new_value->u16v;
			break;
		case REG_U32:
			if(new_value->u32v < node->min_value.u32v || new_value->u32v > node->max_value.u32v)
			{
				cmd_finish_flag = OVER_LIMIT;
				return HAL_ERROR; // 寄存器地址错误
			}
			*(uint32_t*)node->data_ptr = new_value->u32v;
			break;
		case REG_I8:
			if(new_value->i8v < node->min_value.i8v || new_value->i8v > node->max_value.i8v)
			{
				cmd_finish_flag = OVER_LIMIT;
				return HAL_ERROR; // 寄存器地址错误
			}
			*(int8_t*)node->data_ptr = new_value->i8v;
			break;
		case REG_I16:
			if(new_value->i16v < node->min_value.i16v || new_value->i16v > node->max_value.i16v)
			{
				cmd_finish_flag = OVER_LIMIT;
				return HAL_ERROR; // 寄存器地址错误
			}
			*(int16_t*)node->data_ptr = new_value->i16v;
			break;
		case REG_I32:
			if(new_value->i32v < node->min_value.i32v || new_value->i32v > node->max_value.i32v)
			{
				cmd_finish_flag = OVER_LIMIT;
				return HAL_ERROR; // 寄存器地址错误
			}
			*(int32_t*)node->data_ptr = new_value->i32v;
			break;
		case REG_FLOAT:
			if(new_value->fv < node->min_value.fv || new_value->fv > node->max_value.fv)
			{
				cmd_finish_flag = OVER_LIMIT;
				return HAL_ERROR; // 寄存器地址错误
			}
			*(float*)node->data_ptr = new_value->fv;
			break;
		default:
			cmd_finish_flag = PARAMETER_ERROR;
			return HAL_ERROR; // 寄存器地址错误
		}
	// 调用回调函数
	if(node->callback)
	{
		node->callback(node->data_ptr,new_value);
	}
	cmd_finish_flag = EXECUTE_SUCCESS;
	return HAL_OK; // 寄存器地址错误
}

HAL_StatusTypeDef Save_Register(void)
{
	FlashData_t flash_data;
	Erase_Config_Sector();
	flash_data.flash_sysconfig = SysConfig;
	flash_data.flash_plld_Config = PlldConfig;
	flash_data.flash_pressuredect_Config = PressureDectConfig;
	if(Write_Config_Flash(&flash_data) == HAL_OK)
	{
		cmd_finish_flag = EXECUTE_SUCCESS;
		return HAL_OK;
	}
	return HAL_ERROR;

}

