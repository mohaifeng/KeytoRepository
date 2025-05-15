/*
 * register.c
 *
 *  Created on: May 14, 2025
 *      Author: 莫海峰
 */
#include "register.h"

SysConfig_t SysConfig = { 0 };
BMConfig_t BMConfig = { 0 };

// 回调函数示例
void setMotorDirCallback(void *reg_ptr, RegValue new_value)
{
	// 电机方向设置逻辑
	*(uint8_t*) reg_ptr = new_value.u8v;
	// 可以在这里添加硬件控制代码
}

// 寄存器列表
// 地址 | 权限            | 类型   | 数据指针               | 最小值     | 最大值       | 默认值     | 回调函数
static KtCmdRegSubNode KtcmdFactoryRegSubNodeList[] = {
		{ 1, REG_READ_WRITE, REG_U8, &SysConfig.protocolswitch, { .u8v =0 }, { .u8v = 1 }, { .u8v = 0 }, NULL },
		{ 10, REG_READ_WRITE, REG_I32, &BMConfig.PWMFre, { .i32v = 0 }, { .i32v =0x7FFFFFFF }, { .i32v = 0 }, NULL },
		{ 11, REG_READ_WRITE, REG_U8, &BMConfig.Dir, { .u8v = 0 }, { .u8v = 1 }, {.u8v = 0 }, setMotorDirCallback },
		{ 12, REG_READ_WRITE, REG_U16, &BMConfig.ReverseDelay, { .u16v = 0 }, { .u16v =0xFFFF }, { .u16v = 0 }, NULL },
		{ 13, REG_WRITE_ONLY, REG_U8, &BMConfig.PWMMaxPulse, { .u8v = 0 }, { .u8v = 0x64 },{ .u8v = 0 }, NULL }
};

void Init_Registers(void)
{
	for (size_t i = 0; i < REG_LIST_SIZE; i++)
	{
		KtCmdRegSubNode *node = &KtcmdFactoryRegSubNodeList[i];
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
		}
	}
}

KtCmdRegSubNode* find_reg_node(uint16_t addr)
{
	for (size_t i = 0; i < REG_LIST_SIZE; i++)
	{
		if (KtcmdFactoryRegSubNodeList[i].addr == addr)
		{
			return &KtcmdFactoryRegSubNodeList[i];
		}
	}
	return NULL;
}

// 读取寄存器值
int read_register(uint16_t addr, RegValue *out_value)
{
	KtCmdRegSubNode *node = find_reg_node(addr);
	if (!node || !(node->permission & REG_READ_ONLY))
	{
		return -1; // 寄存器不存在或不可读
	}

	switch (node->data_type)
	{
		case REG_U8:
			out_value->u8v = *(uint8_t*) node->data_ptr;
			break;
		case REG_U16:
			out_value->u16v = *(uint16_t*) node->data_ptr;
			break;
		case REG_U32:
			out_value->u32v = *(uint32_t*) node->data_ptr;
			break;
		case REG_I8:
			out_value->i8v = *(int8_t*) node->data_ptr;
			break;
		case REG_I16:
			out_value->i16v = *(int16_t*) node->data_ptr;
			break;
		case REG_I32:
			out_value->i32v = *(int32_t*) node->data_ptr;
			break;
		case REG_FLOAT:
			out_value->fv = *(float*) node->data_ptr;
			break;
		default:
			return -2; // 不支持的数据类型
	}
	return 0;
}

// 写入寄存器值
int write_register(uint16_t addr, RegValue new_value)
{
	KtCmdRegSubNode *node = find_reg_node(addr);
	if (!node || !(node->permission & REG_WRITE_ONLY))
	{
		return -1; // 寄存器不存在或不可写
	}

	// 检查值范围
	switch (node->data_type)
	{
		case REG_U8:
			if (new_value.u8v < node->min_value.u8v || new_value.u8v > node->max_value.u8v)
			{
				return -3; // 超出范围
			}
			*(uint8_t*) node->data_ptr = new_value.u8v;
			break;

		case REG_U16:
			if (new_value.u16v < node->min_value.u16v || new_value.u16v > node->max_value.u16v)
			{
				return -3;
			}
			*(uint16_t*) node->data_ptr = new_value.u16v;
			break;

			// 其他类型类似处理...

		default:
			return -2; // 不支持的数据类型
	}

	// 调用回调函数
	if (node->callback)
	{
		node->callback(node->data_ptr, new_value);
	}
	return 0;
}

