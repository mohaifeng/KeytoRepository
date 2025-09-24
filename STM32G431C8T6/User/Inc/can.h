/*
 * can.h
 *
 *  Created on: Sep 19, 2025
 *      Author: 莫海峰
 */

#ifndef INC_CAN_H_
#define INC_CAN_H_

#include "main.h"
#include "register.h"

// 在文件头部定义实际变量
#define OD_SIZE 255 //定义对象字典

typedef enum
{
	CMD_RESPONSE,
	CMD_WRITE,
	CMD_READ,
	CMD_PROCESS,
	CMD_HEART,
	CMD_WARNING = 0x80,
} CAN_CmdType;

typedef struct
{
	uint16_t cmd;
	uint8_t target_addr;
	uint8_t ser_num;
	uint16_t idex;
	uint8_t sub_idex;
	int32_t data;
} CANFrame_Handle_t;

// 对象字典条目定义
typedef struct
{
	uint16_t index;         // 索引
	uint8_t sub_index;      // 子索引(0表示主条目)
	RW_Handle_t access;     // 访问权限
	int32_t data;         // 数据
	int32_t Maxval;
	int32_t Minval;
	int32_t Deftval;
} OD_Typedef;

typedef struct
{
	OD_Typedef od_arr[OD_SIZE];
	uint8_t tail;
} OD_ListTypedef;

void User_CAN_Init(void);
void OD_Init(void);
void CAN_Transmit(uint32_t id);
void CAN_ResolutionGeneral_Protocol(uint32_t idtype, uint8_t *data, uint8_t len);
void CAN_Heart_Transmit(void);
#endif /* INC_CAN_H_ */
