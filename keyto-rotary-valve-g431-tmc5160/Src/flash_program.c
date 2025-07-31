#include <string.h>
#include "stm32g4xx.h"
#include "stm32g4xx_hal.h"
#include "flash_program.h"

SysConfig_type  SysConfig;
int WriteSysConfigToFlash(SysConfig_type  *config)
{
  FLASH_EraseInitTypeDef EraseInitStruct;
  uint32_t Address = 0, PageError = 0,i;
  uint64_t *Data;
	uint64_t data32;
  uint16_t Len;
  HAL_FLASH_Unlock();
  Data=(uint64_t *)config;
  Len=sizeof(SysConfig_type)/8;
	if(sizeof(SysConfig_type)%8)
		Len+=1;
  /* Erase the user Flash area
    (area defined by FLASH_USER_START_ADDR and FLASH_USER_END_ADDR) ***********/

  /* Fill EraseInit structure*/
  EraseInitStruct.TypeErase = FLASH_TYPEERASE_PAGES;
  EraseInitStruct.Banks = FLASH_BANK_1;
  EraseInitStruct.Page = 31;
  EraseInitStruct.NbPages = (FLASH_USER_END_ADDR - FLASH_USER_START_ADDR) / FLASH_PAGE_SIZE;

  if(HAL_FLASHEx_Erase(&EraseInitStruct, &PageError) != HAL_OK)
  {
    return -1;
  }

  /* Program the user Flash area word by word
    (area defined by FLASH_USER_START_ADDR and FLASH_USER_END_ADDR) ***********/

  Address = FLASH_USER_START_ADDR;
  for(i=0;i<Len;i++)
  {
    if (HAL_FLASH_Program(FLASH_TYPEPROGRAM_DOUBLEWORD, Address, Data[i]) == HAL_OK)
    {
      Address = Address + 8;
      if(Address >= FLASH_USER_END_ADDR)
        break;
			
    }
    else
    {
      /* Error occurred while writing data in Flash memory.
         User can add here some code to deal with this error */
      return -2;
    }
  }

  /* Lock the Flash to disable the flash control register access (recommended
     to protect the FLASH memory against possible unwanted operation) *********/
  HAL_FLASH_Lock();
  
  Address = FLASH_USER_START_ADDR;

  for(i=0;i<Len;i++)
  {
    data32 = *(__IO uint64_t *)Address;//uint32_t

    if (data32 != Data[i])
    {
      return -3;
    }
    Address = Address + 8;
  }
  return 0;
}

void ReadSysConfigFromFlash(void)
{
  memcpy((void *)&SysConfig,(void *)(FLASH_USER_START_ADDR),sizeof(SysConfig_type));
}
SysConfig_type *GetSysConfigFromFlash(void)
{
  return &SysConfig;
}
/*RTC_HandleTypeDef hrtc;
static void MX_RTC_Init(void)
{
  hrtc.Instance = RTC;
  hrtc.Init.HourFormat = RTC_HOURFORMAT_24;
  hrtc.Init.AsynchPrediv = 127;
  hrtc.Init.SynchPrediv = 255;
  hrtc.Init.OutPut = RTC_OUTPUT_DISABLE;
  hrtc.Init.OutPutPolarity = RTC_OUTPUT_POLARITY_HIGH;
  hrtc.Init.OutPutType = RTC_OUTPUT_TYPE_OPENDRAIN;
  HAL_RTC_Init(&hrtc);
}
uint32_t ReadRTC_BKP_Flag(void)
{
  hrtc.Instance = RTC;
  return HAL_RTCEx_BKUPRead(&hrtc, 0);
}
void WriteRTC_BKP_Flag(uint32_t flag)
{
  MX_RTC_Init();
  __HAL_RCC_RTC_ENABLE();
  HAL_RTCEx_BKUPWrite(&hrtc, 0,flag);
}*/

void Flash_WriteIntoBootLoadModeFlag(uint8_t flag)
{
  SysConfig.IntoBootMode=flag;
  WriteSysConfigToFlash(&SysConfig);
}
