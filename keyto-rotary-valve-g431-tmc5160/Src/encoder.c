#include "encoder.h"
 

#if (USED_ENCODER == 1)
#include "valve_control.h"
#include "stm32g4xx_hal.h"
#include "stm32g4xx.h"
#include "stdlib.h"
#include "string.h"
#include "kconfig.h"
#include "flash_program.h"



ENCODER_Handle_t ENCODER_M1 =
{ 
  /* SW Settings */
  .TimerOverflowNb       =	0,
	.PreviousCapture       =	0,
	.DeltaCapturesIndex    =	0,
	.EncAvrSpeed       		 =	0,
	.ContDir							 =  0,
};


void HAL_TIM_OC_DelayElapsedCallback(TIM_HandleTypeDef *htim)
{
	if(htim->Instance==TIM4)
	{
		Valve_ImmediatelyStopMotor();
	}
}


void Encoder_Init( void )
{
	if(SysConfig.EncoderConfig.EncoderEn)
	{
		Encoder_SetPositionCont(0);
		__HAL_TIM_CLEAR_FLAG(&htim4, TIM_FLAG_UPDATE);	
		__HAL_TIM_ENABLE_IT(&htim4, TIM_IT_UPDATE);
		HAL_TIM_Encoder_Start(&htim4,TIM_CHANNEL_ALL);
		Encoder_StopComparedCapture();
	}
	else
	{
		__HAL_TIM_CLEAR_FLAG(&htim4, TIM_FLAG_UPDATE);	
		__HAL_TIM_DISABLE_IT(&htim4, TIM_IT_UPDATE);
		HAL_TIM_Encoder_Stop(&htim4,TIM_CHANNEL_ALL);
	}
}
/*
设置电机位置计数方向
*/
void Encoder_SetTimContDir(uint8_t ContDir)
{
	ENCODER_M1.ContDir=ContDir;
}
/*
获取编码器位置
*/
int32_t Encoder_GetPositionCont(void)
{
	int position;
  position = __HAL_TIM_GET_COUNTER(&htim4);
	if(ENCODER_M1.ContDir)
		position=0-position;
	return position;
}
/*
设置编码器位置
*/
void Encoder_SetPositionCont(uint32_t p)
{
	if(ENCODER_M1.ContDir)
		__HAL_TIM_SET_COUNTER(&htim4,0-p);
	else
		__HAL_TIM_SET_COUNTER(&htim4,p);
}
/*
获取定时器计数方向
*/
uint32_t TIM_GetDirection(TIM_TypeDef *TIMx)
{
  return (uint32_t)(READ_BIT(TIMx->CR1, TIM_CR1_DIR));
}
/*
函数功能：设置位置计数器捕获位置计数值
*/
void Encoder_SetComparedCaptureCont(uint32_t Cont)
{
	if(ENCODER_M1.ContDir)
		Cont=0-Cont;
  TIM_OC_InitTypeDef sConfigOC;
  sConfigOC.OCMode = TIM_OCMODE_TIMING;
  sConfigOC.Pulse = Cont;
  sConfigOC.OCPolarity = TIM_OCPOLARITY_HIGH;
  sConfigOC.OCNPolarity = TIM_OCNPOLARITY_HIGH;
  sConfigOC.OCFastMode = TIM_OCFAST_DISABLE;
  sConfigOC.OCIdleState = TIM_OCIDLESTATE_RESET;
  sConfigOC.OCNIdleState = TIM_OCNIDLESTATE_RESET;
  if (HAL_TIM_OC_ConfigChannel(&htim4, &sConfigOC, TIM_CHANNEL_3) != HAL_OK)
  {
    Error_Handler();
  }
  __HAL_TIM_CLEAR_IT(&htim4, TIM_IT_CC3);
  if (HAL_TIM_OC_Start_IT(&htim4, TIM_CHANNEL_3) != HAL_OK)
  {
    Error_Handler();
  }
}
/*
函数功能：关闭位置计数器捕获
*/
void Encoder_StopComparedCapture(void)
{
  HAL_TIM_OC_Stop_IT(&htim4, TIM_IT_CC3);
}

/*
根据编码器计算电机速度
*/
void Encoder_CalcAvrgMecSpeed(void)
{
	if(SysConfig.EncoderConfig.EncoderEn==0)
		return;
	int64_t wOverallAngleVariation = 0;
	int32_t CntCapture;
	uint8_t bBufferIndex;
	CntCapture=Encoder_GetPositionCont();;
	//计数编码器瞬时增量
	ENCODER_M1.DeltaCapturesBuffer[ENCODER_M1.DeltaCapturesIndex++] =
       CntCapture +(ENCODER_M1.TimerOverflowNb*SysConfig.EncoderConfig.PulseNumber ) - ( int32_t )( ENCODER_M1.PreviousCapture ) ;		
	for ( bBufferIndex = 0u; bBufferIndex < ENC_SPEED_ARRAY_SIZE; bBufferIndex++ )
  {
    wOverallAngleVariation += ENCODER_M1.DeltaCapturesBuffer[bBufferIndex];
  }
	//计数平均速度
	//(wOverallAngleVariation/ENC_SPEED_ARRAY_SIZE)*(1000/ENCODER_TASK_TIME)
	ENCODER_M1.EncAvrSpeed=(wOverallAngleVariation*1000)/(ENC_SPEED_ARRAY_SIZE*ENCODER_TASK_TIME);
	//将编码器速度换算为步进电机速度step/s
	
	if ( ENCODER_M1.DeltaCapturesIndex >= ENC_SPEED_ARRAY_SIZE )
  {
    ENCODER_M1.DeltaCapturesIndex = 0u;
  }
	ENCODER_M1.TimerOverflowNb=0;
	ENCODER_M1.PreviousCapture = CntCapture;
}
int Encoder_GetAvrSpeed(void)
{
	return ENCODER_M1.EncAvrSpeed;
}
void Encoder_UpDateCont( void )
{
  ENCODER_M1.TimerOverflowNb += 1u;
}

#endif
