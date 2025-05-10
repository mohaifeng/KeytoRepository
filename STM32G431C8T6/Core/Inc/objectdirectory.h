/*
 * can.h
 *
 *  Created on: May 10, 2025
 *      Author: 莫海峰
 */

#ifndef INC_OBJECTDIRECTORY_H_
#define INC_OBJECTDIRECTORY_H_

#include <stdint.h>
// 在文件头部定义实际变量
#define CALIB_DATA_SIZE 255  // 假设校准数组有10个元素
#define OD_ARRAY_ENTRY(index, sub, ptr, name)	{index, sub, OD_I32, OD_READ_WRITE, ptr, 8, name}
// 对象字典数据类型定义
typedef enum
{
	OD_U8 = 0,      // 无符号8位
	OD_U16,         // 无符号16位
	OD_U32,         // 无符号32位
	OD_I8,          // 有符号8位
	OD_I16,         // 有符号16位
	OD_I32,         // 有符号32位
	OD_FLOAT,       // 单精度浮点
	OD_STRING,      // 字符串
	OD_ARRAY,       // 数组
	OD_RECORD       // 记录(包含子索引)
} OD_DATATYPEDEF;

// 访问权限定义
typedef enum
{
	OD_READ = 1,
	OD_WRITE,
	OD_READ_WRITE
} OD_ACCESSTYPEDEF;

// 对象字典条目定义
typedef struct
{
	uint16_t index;         // 索引
	uint8_t sub_index;      // 子索引(0表示主条目)
	OD_DATATYPEDEF type;    // 数据类型
	OD_ACCESSTYPEDEF access;     // 访问权限
	void *data_ptr;         // 数据指针
	uint8_t data_size;     // 数据大小(字节)
	const char *name;       // 条目名称(可选)
} OD_ENTRYTYPEDEF;

#endif /* INC_OBJECTDIRECTORY_H_ */
