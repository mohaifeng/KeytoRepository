/*
 * register.h
 *
 *  Created on: May 14, 2025
 *      Author: 莫海峰
 */

#ifndef INC_REGISTER_H_
#define INC_REGISTER_H_

#include "main.h"
#define REG_LIST_SIZE (sizeof(KtcmdFactoryRegSubNodeList) / sizeof(KtCmdRegSubNode))
//定义寄存器读写权限
typedef enum {
    REG_READ_ONLY = 0x01,
    REG_WRITE_ONLY = 0x02,
    REG_READ_WRITE = 0x03
} RegPermission;

//定义寄存器值数据类型
typedef enum {
    REG_U8,
    REG_U16,
    REG_U32,
    REG_I8,
    REG_I16,
    REG_I32,
    REG_FLOAT
} RegDataType;

typedef union {
    uint8_t u8v;
    uint16_t u16v;
    uint32_t u32v;
    int8_t i8v;
    int16_t i16v;
    int32_t i32v;
    float fv;
} RegValue;

typedef void (*RegCallback)(void* reg_ptr, RegValue new_value);

typedef struct {
    uint16_t addr;              // 寄存器地址
    RegPermission permission;   // 读写权限
    RegDataType data_type;      // 数据类型
    void* data_ptr;             // 数据存储指针
    RegValue min_value;         // 最小值
    RegValue max_value;         // 最大值
    RegValue default_value;     // 默认值
    RegCallback callback;       // 写操作回调函数
} KtCmdRegSubNode;


#endif /* INC_REGISTER_H_ */
