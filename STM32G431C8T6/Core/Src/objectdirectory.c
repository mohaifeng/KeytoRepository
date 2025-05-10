/*
 * can.c
 *
 *  Created on: May 10, 2025
 *      Author: 莫海峰
 */
#include "objectdirectory.h"
#include "string.h"

// 定义对象字典数据数组
uint32_t calib_data[CALIB_DATA_SIZE] = { 0 };
// 定义对象字典
const OD_ENTRYTYPEDEF object_dictionary[] = {
		// 使用宏定义数组元素
		OD_ARRAY_ENTRY(0x3000, 1, &calib_data[0], "Calib Data 1"),
		OD_ARRAY_ENTRY(0x3000, 2, &calib_data[1],"Calib Data 2"),
		// ... 其余元素 ...
		};

// 通过索引和子索引查找字典条目
const OD_ENTRYTYPEDEF* od_find_entry(uint16_t index, uint8_t sub_index)
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

// 在初始化函数中设置值
void init_calibration_data()
{
	for (int i = 0; i < CALIB_DATA_SIZE; i++)
	{
		calib_data[i] = 1.0f + i * 0.1f;  // 1.0, 1.1, 1.2...
	}
}

// 读取对象字典值
int od_read_value(uint16_t index, uint8_t sub_index, void *buffer, uint16_t buf_size)
{
	const OD_ENTRYTYPEDEF *entry = od_find_entry(index, sub_index);
	if (!entry || !(entry->access & OD_READ))
	{
		return -1;  // 条目不存在或不可读
	}

	if (buf_size < entry->data_size)
	{
		return -2;  // 缓冲区太小
	}

	memcpy(buffer, entry->data_ptr, entry->data_size);
	return 0;
}

// 写入对象字典值
int od_write_value(uint16_t index, uint8_t sub_index, const void *value, uint16_t value_size)
{
	const OD_ENTRYTYPEDEF *entry = od_find_entry(index, sub_index);
	if (!entry || !(entry->access & OD_WRITE))
	{
		return -1;  // 条目不存在或不可写
	}

	if (value_size != entry->data_size)
	{
		return -2;  // 数据大小不匹配
	}

	memcpy(entry->data_ptr, value, entry->data_size);
	return 0;
}
