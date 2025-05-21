/*
 * flash.h
 *
 *  Created on: May 19, 2025
 *      Author: 莫海峰
 */

#ifndef INC_FLASH_H_
#define INC_FLASH_H_

#include "main.h"

#define CONFIG_FLASH_ADDR 0x0801F800 //flash最后一页起始地址
#define CONFIG_FLASH_SIZE 2048  // 2KB

void Erase_Config_Sector(void);
HAL_StatusTypeDef Write_Config_Flash(SysConfig_t *config);
HAL_StatusTypeDef Read_Config_Flash(SysConfig_t *config);
#endif /* INC_FLASH_H_ */
