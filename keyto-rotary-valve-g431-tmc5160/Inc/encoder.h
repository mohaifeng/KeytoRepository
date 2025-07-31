/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __ENCODER_H
#define __ENCODER_H

/* Includes ------------------------------------------------------------------*/
#include "stm32g4xx_hal.h"
#include "stm32g4xx.h"
#include "main.h"
#include "kconfig.h"

#if (USED_ENCODER == 1)
void Encoder_Init( void );
void Encoder_CalcAvrgMecSpeed(void);
int Encoder_GetAvrSpeed(void);
void Encoder_SetTimContDir(uint8_t ContDir);
int32_t Encoder_GetPositionCont(void);
void Encoder_SetPositionCont(uint32_t p);
void Encoder_SetComparedCaptureCont(uint32_t Cont);
void Encoder_StopComparedCapture(void);
#endif
#endif /* __ENCODER_H */

/******************* (C) COPYRIGHT 2013 STMicroelectronics *****END OF FILE****/
