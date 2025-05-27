/*
 * flash.h
 *
 *  Created on: May 19, 2025
 *      Author: 莫海峰
 */

#ifndef INC_FLASH_H_
#define INC_FLASH_H_

#include "main.h"
#include "verification.h"

//#define CONFIG_FLASH_ADDR 0x0801F800 //flash最后一页起始地址
//#define CONFIG_FLASH_SIZE 2048  // 2KB

#define FLASH_LAST_PAGE_ADDR  0x0801F800  // 假设最后一页起始地址

typedef enum
{
	Type_SysConfig = 1,
	Type_PlldConfig,
	Type_PressureConfig
} DataType;

#pragma pack(push, 1)
typedef struct
{
	DataType data_type;    // 数据类型标识
	uint16_t data_size;   // 数据部分大小
	uint32_t crc;         // CRC校验值
	uint8_t reserved;     // 1字节填充，使总大小为8字节
} FlashDataHeader;
#pragma pack(pop)

void Erase_Flash(void);
HAL_StatusTypeDef Write_Struct_To_Flash(DataType data_type, void *data, uint16_t data_size);
void* Read_Struct_From_Flash(DataType expected_type, uint16_t *out_size);
#endif /* INC_FLASH_H_ */
