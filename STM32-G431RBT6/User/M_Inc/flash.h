/*
 * flash.h
 *
 *  Created on: Jun 13, 2025
 *      Author: 莫海峰
 */

#ifndef INC_FLASH_H_
#define INC_FLASH_H_

#include "main.h"
#include "verification.h"

#define FLASH_BASE_ADDR   	0x08000000 //flash起始地址
#define CONFIG_FLASH_SIZE 	2048  // 2KB
#define CONFIG_FLASH_PAGE 	31	//STM32G431C8T6最后一页(页31)
#define CONFIG_FLASH_ADDR 	(FLASH_BASE_ADDR+(CONFIG_FLASH_PAGE*CONFIG_FLASH_SIZE)) //flash最后一页起始地址

//typedef enum
//{
//	Type_SysConfig = 1,
//	Type_PlldConfig,
//	Type_PressureConfig
//} DataType;

typedef struct
{
	SysConfig_t flash_sysconfig;
	PlldConfig_t flash_plld_Config;
	PressureDectConfig_t flash_pressuredect_Config;
	uint32_t crc; // 添加CRC校验字段
} FlashData_t;

//#pragma pack(push, 1)
//typedef struct
//{
//	DataType data_type;    // 数据类型标识
//	uint16_t data_size;   // 数据部分大小
//	uint32_t crc;         // CRC校验值
//	uint8_t reserved;     // 1字节填充，使总大小为8字节
//} FlashDataHeader;
//#pragma pack(pop)

void Erase_Config_Sector(void);
HAL_StatusTypeDef Write_Config_Flash(FlashData_t *config);
HAL_StatusTypeDef Read_Config_Flash(FlashData_t *config);

//HAL_StatusTypeDef Write_Struct_To_Flash(DataType data_type, void *data, uint16_t data_size);
//void* Read_Struct_From_Flash(DataType expected_type, uint16_t *out_size);

#endif /* INC_FLASH_H_ */
