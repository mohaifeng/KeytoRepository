/*
 * objectdirectory.c
 *
 *  Created on: Jun 13, 2025
 *      Author: 莫海峰
 */
#include "objectdirectory.h"
#include "register.h"
#include "main.h"
#include "cmd.h"
#include <string.h>

// 定义对象字典数据数组
int32_t oddata_arr[OD_SIZE] = { 0 }; //对象字典数据数组
OD_ENTRYTYPEDEF object_dictionary[OD_SIZE] = { 0 };
uint8_t od_count = 0; //对象字典数组序号
extern SysConfig_t SysConfig;

static void OD_Config_Callback(uint16_t index, uint8_t sub, OD_ACCESSTYPEDEF access, int32_t Deftval, int32_t Minval,
		int32_t Maxval)
{
	if (od_count < OD_SIZE)
	{
		object_dictionary[od_count].index = index;
		object_dictionary[od_count].sub_index = sub;
		object_dictionary[od_count].type = OD_I32;
		object_dictionary[od_count].access = access;
		object_dictionary[od_count].data_ptr = &oddata_arr[od_count];
		object_dictionary[od_count].Deftval = Deftval;
		object_dictionary[od_count].Minval = Minval;
		object_dictionary[od_count].Maxval = Maxval;
		object_dictionary[od_count].data_size = 7; //主索引（2字节）+子索引（1字节）+数据（4字节）
		*object_dictionary[od_count].data_ptr = object_dictionary[od_count].Deftval;
		oddata_arr[od_count] = object_dictionary[od_count].Deftval;
		od_count++;
	}
}

void OD_Init(void)
{
	od_count = 0;
	OD_Config_Callback(0x4000, 0, OD_WRITE, 0, 10, 1000); //置零过程中速度,单位：uL/s。
	OD_Config_Callback(0x4000, 1, OD_READ_WRITE, 100, 0, 100); //置零过程中功率百分比，单位：%
	OD_Config_Callback(0x4000, 2, OD_READ_WRITE, 0, 0, 2); //置零过程中退TIP模式

	OD_Config_Callback(0x4001, 0, OD_WRITE, 0, 1, 104000); //吸液量,单位：0.01uL。
	OD_Config_Callback(0x4001, 1, OD_READ_WRITE, 500, 1, 2000); //吸液速度,单位：uL/s。
	OD_Config_Callback(0x4001, 2, OD_READ_WRITE, 10, 0, 2000); //截流速度,单位：uL/s。
	OD_Config_Callback(0x4001, 3, OD_READ_WRITE, 0, 0, 2); //TIP补偿

	OD_Config_Callback(0x4002, 0, OD_WRITE, 0, 1, 104000); //排液量,单位：0.01uL。
	OD_Config_Callback(0x4002, 1, OD_READ_WRITE, 0, 0, 1000); //回吸体积,单位：0.01uL。
	OD_Config_Callback(0x4002, 2, OD_READ_WRITE, 500, 1, 2000); //排液速度,单位：uL/s。
	OD_Config_Callback(0x4002, 3, OD_READ_WRITE, 10, 0, 2000); //停止速度,单位：uL/s。

	OD_Config_Callback(0x4003, 0, OD_WRITE, 0, 0, 250880); //位置值，单位P
	OD_Config_Callback(0x4003, 1, OD_READ_WRITE, 128000, 0, 500000); //运行速度，单位P/s
	OD_Config_Callback(0x4003, 2, OD_READ_WRITE, 32000, 0, 256000); //停止速度，单位P/s

	OD_Config_Callback(0xFFFF, 0, OD_READ, 0, 0, 0); //对象字典结束标志
}

uint8_t OD_Idex_Count(uint16_t index)
{
	uint8_t count = 0;
	for (const OD_ENTRYTYPEDEF *entry = object_dictionary; entry->index != 0xFFFF; entry++)
	{
		if (entry->index == index)
		{
			count++;
		}
	}
	return count;
}

// 通过索引和子索引查找字典条目
const OD_ENTRYTYPEDEF* OD_Find_Entry(uint16_t index, uint8_t sub_index)
{
	for (const OD_ENTRYTYPEDEF *entry = object_dictionary; entry->index != 0xFFFF; entry++)
	{
		if (entry->index == index && entry->sub_index == sub_index)
		{
			return entry;
		}
	}
	return NULL;
}

// 读取对象字典值
HAL_StatusTypeDef OD_Read(uint16_t index, uint8_t sub_index, int32_t *pdata)
{
	if (index == 0x2000)
	{
		RegValue data = { 0 };
		if (Read_Register(sub_index, &data) == HAL_OK)
		{
			*pdata = data.i32v;
			return HAL_OK;
		}
	}
	const OD_ENTRYTYPEDEF *entry = OD_Find_Entry(index, sub_index);
	if (entry)
	{
		if (!(entry->access & OD_READ))
		{
			cmd_finish_flag = READ_WRITE_ONLY;
			return HAL_ERROR;
		}
		else
		{
			*pdata = *entry->data_ptr;
			cmd_finish_flag = EXECUTE_SUCCESS;
			return HAL_OK;
		}
	}
	cmd_finish_flag = PARAMETER_ERROR;
	return HAL_ERROR;
}

// 写入对象字典值0:执行成功；-1：无该字典；-2：状态错误
HAL_StatusTypeDef OD_Write(uint16_t index, uint8_t sub_index, int32_t value)
{
	if (index == 0x2000)
	{
		RegValue data = { 0 };
		data.i32v = value;
		return Write_Register(sub_index, &data);
	}
	const OD_ENTRYTYPEDEF *entry = OD_Find_Entry(index, sub_index);
	if (entry)
	{
		if (!(entry->access & OD_WRITE))
		{
			cmd_finish_flag = READ_WRITE_ONLY;
			return HAL_ERROR;
		}
		else
		{
			if (value < entry->Minval && value > entry->Maxval)
			{
				cmd_finish_flag = OVER_LIMIT;
				return HAL_ERROR;
			}
			else
			{
				*entry->data_ptr = value;
				cmd_finish_flag = EXECUTE_SUCCESS;
				return HAL_OK;
			}
		}
	}
	cmd_finish_flag = PARAMETER_ERROR;
	return HAL_ERROR;
}

