#include <stdlib.h>
#include "main.h"
#include "string.h"
#include "stm32g4xx_hal.h"
#include "console.h"


const char *Password="keyto\r";

uint8_t ConsoleMode=false;
//Consloe_State_t ConsoleStatus;
uint8_t RxUsartNum=0;
uint8_t InputMenuIndex=0;
uint8_t IntoInputKey=0;
uint8_t InputKeyNum=0;
int InputValue;
char InputKey[20];

extern uint32_t FlowZeroHoleOpt[2];
extern uint32_t FlowPositionHoleOpt[100];

extern UART_HandleTypeDef huart1;
extern UART_HandleTypeDef huart2;

extern uint8_t AssignTemporaryAddressUpUIDFlag;
extern  uint32_t Send_Scan_UID_TimeCont;
//重写fputc函数
#if USE_USART_CONSOLE_CONFIG
int fputc(int ch, FILE *f){
  uint8_t ch_out;
  ch_out=ch;
  HAL_UART_Transmit(&huart1, &ch_out, 1, 100);
  return ch_out;
}
#endif


//清除串口错误
void ClearUSART_Error(UART_HandleTypeDef *huart)
{
	uint32_t isrflags   = READ_REG(huart->Instance->ISR);
  uint32_t cr1its     = READ_REG(huart->Instance->CR1);
  uint32_t cr3its     = READ_REG(huart->Instance->CR3);
	
	uint32_t errorflags;
  uint32_t errorcode;
	
	errorflags = (isrflags & (uint32_t)(USART_ISR_PE | USART_ISR_FE | USART_ISR_ORE | USART_ISR_NE));
  /* If some errors occur */
  if ((errorflags != 0U)
      && ((((cr3its & (USART_CR3_RXFTIE | USART_CR3_EIE)) != 0U)
           || ((cr1its & (USART_CR1_RXNEIE_RXFNEIE | USART_CR1_PEIE)) != 0U))))
  {
    /* UART parity error interrupt occurred -------------------------------------*/
    if (((isrflags & USART_ISR_PE) != 0U) && ((cr1its & USART_CR1_PEIE) != 0U))
    {
      __HAL_UART_CLEAR_FLAG(huart, UART_CLEAR_PEF);

      huart->ErrorCode |= HAL_UART_ERROR_PE;
    }

    /* UART frame error interrupt occurred --------------------------------------*/
    if (((isrflags & USART_ISR_FE) != 0U) && ((cr3its & USART_CR3_EIE) != 0U))
    {
      __HAL_UART_CLEAR_FLAG(huart, UART_CLEAR_FEF);

      huart->ErrorCode |= HAL_UART_ERROR_FE;
    }

    /* UART noise error interrupt occurred --------------------------------------*/
    if (((isrflags & USART_ISR_NE) != 0U) && ((cr3its & USART_CR3_EIE) != 0U))
    {
      __HAL_UART_CLEAR_FLAG(huart, UART_CLEAR_NEF);

      huart->ErrorCode |= HAL_UART_ERROR_NE;
    }

    /* UART Over-Run interrupt occurred -----------------------------------------*/
    if (((isrflags & USART_ISR_ORE) != 0U)
        && (((cr1its & USART_CR1_RXNEIE_RXFNEIE) != 0U) ||
            ((cr3its & (USART_CR3_RXFTIE | USART_CR3_EIE)) != 0U)))
    {
      __HAL_UART_CLEAR_FLAG(huart, UART_CLEAR_OREF);

      huart->ErrorCode |= HAL_UART_ERROR_ORE;
    }

    /* Call UART Error Call back function if need be --------------------------*/
    if (huart->ErrorCode != HAL_UART_ERROR_NONE)
    {
      /* UART in mode Receiver ---------------------------------------------------*/
      if (((isrflags & USART_ISR_RXNE_RXFNE) != 0U)
          && (((cr1its & USART_CR1_RXNEIE_RXFNEIE) != 0U)
              || ((cr3its & USART_CR3_RXFTIE) != 0U)))
      {
        if (huart->RxISR != NULL)
        {
          huart->RxISR(huart);
        }
      }

      /* If Overrun error occurs, or if any error occurs in DMA mode reception,
         consider error as blocking */
      errorcode = huart->ErrorCode;
      if ((HAL_IS_BIT_SET(huart->Instance->CR3, USART_CR3_DMAR)) ||
          ((errorcode & HAL_UART_ERROR_ORE) != 0U))
      {
        /* Blocking error : transfer is aborted
           Set the UART state ready to be able to start again the process,
           Disable Rx Interrupts, and disable Rx DMA request, if ongoing */
        //UART_EndRxTransfer(huart);

        /* Disable the UART DMA Rx request if enabled */
        if (HAL_IS_BIT_SET(huart->Instance->CR3, USART_CR3_DMAR))
        {
          CLEAR_BIT(huart->Instance->CR3, USART_CR3_DMAR);

          /* Abort the UART DMA Rx channel */
          if (huart->hdmarx != NULL)
          {
            /* Set the UART DMA Abort callback :
               will lead to call HAL_UART_ErrorCallback() at end of DMA abort procedure */
            //huart->hdmarx->XferAbortCallback = UART_DMAAbortOnError;

            /* Abort DMA RX */
            if (HAL_DMA_Abort_IT(huart->hdmarx) != HAL_OK)
            {
              /* Call Directly huart->hdmarx->XferAbortCallback function in case of error */
              huart->hdmarx->XferAbortCallback(huart->hdmarx);
            }
          }
          else
          {
            /* Call user error callback */
#if (USE_HAL_UART_REGISTER_CALLBACKS == 1)
            /*Call registered error callback*/
            huart->ErrorCallback(huart);
#else
            /*Call legacy weak error callback*/
            HAL_UART_ErrorCallback(huart);
#endif /* USE_HAL_UART_REGISTER_CALLBACKS */

          }
        }
        else
        {
          /* Call user error callback */
#if (USE_HAL_UART_REGISTER_CALLBACKS == 1)
          /*Call registered error callback*/
          huart->ErrorCallback(huart);
#else
          /*Call legacy weak error callback*/
          HAL_UART_ErrorCallback(huart);
#endif /* USE_HAL_UART_REGISTER_CALLBACKS */
        }
      }
      else
      {
        /* Non Blocking error : transfer could go on.
           Error is notified to user through user error callback */
#if (USE_HAL_UART_REGISTER_CALLBACKS == 1)
        /*Call registered error callback*/
        huart->ErrorCallback(huart);
#else
        /*Call legacy weak error callback*/
        HAL_UART_ErrorCallback(huart);
#endif /* USE_HAL_UART_REGISTER_CALLBACKS */
        huart->ErrorCode = HAL_UART_ERROR_NONE;
      }
    }
    return;

  } /* End if some error occurs */
}
//串口1接收
#define MAX_RX_BUFF 100
uint8_t USART_Rx1Buff[MAX_RX_BUFF+1];
uint8_t USART_Rx1Cont=0;
uint8_t USART_Rx1EndFlag=0;
extern uint8_t USART1_RxFlag;
extern uint32_t USART1_RxTimeCont;
void USART1_Rx(UART_HandleTypeDef *huart)
{
  uint32_t isrflags   = READ_REG(huart->Instance->ISR);
  uint32_t cr1its     = READ_REG(huart->Instance->CR1);
  uint32_t errorflags;
  uint8_t  uhdata;
  errorflags = (isrflags & (uint32_t)(USART_ISR_PE | USART_ISR_FE | USART_ISR_ORE | USART_ISR_NE));
  if (errorflags == RESET)
  {
    if(((isrflags & USART_ISR_RXNE) != RESET) && ((cr1its & USART_CR1_RXNEIE) != RESET))
    {
      uhdata = (uint16_t) READ_REG(huart->Instance->RDR);
      if(USART_Rx1Cont>=MAX_RX_BUFF)
        USART_Rx1Cont=0;
      USART_Rx1Buff[USART_Rx1Cont++] = (uint8_t)(uhdata);
      USART1_RxFlag=1;
      USART1_RxTimeCont=0;
      if(ConsoleMode)
      {
        HAL_UART_Transmit(&huart1, &uhdata, 1, 100);
      }
      return ;
    }
  }
  else
  {
    ClearUSART_Error(huart);
    //HAL_UART_IRQHandler(huart);
    HAL_UART_Receive_IT(&huart1, (uint8_t *)USART_Rx1Buff, MAX_RX_BUFF);
  }
}
//串口2接收
uint8_t USART_Rx2Buff[MAX_RX_BUFF+1];
uint8_t USART_Rx2Cont=0;
uint8_t USART_Rx2EndFlag=0;
extern uint8_t USART2_RxFlag;
extern uint32_t USART2_RxTimeCont;
void USART2_Rx(UART_HandleTypeDef *huart)
{
  uint32_t isrflags   = READ_REG(huart->Instance->ISR);
  uint32_t cr1its     = READ_REG(huart->Instance->CR1);
  uint32_t errorflags;
  uint8_t  uhdata;
  errorflags = (isrflags & (uint32_t)(USART_ISR_PE | USART_ISR_FE | USART_ISR_ORE | USART_ISR_NE));
  if (errorflags == RESET)
  {
    if(((isrflags & USART_ISR_RXNE) != RESET) && ((cr1its & USART_CR1_RXNEIE) != RESET))
    {
      uhdata = (uint16_t) READ_REG(huart->Instance->RDR);
      if(USART_Rx2Cont>=MAX_RX_BUFF)
        USART_Rx2Cont=0;
      USART_Rx2Buff[USART_Rx2Cont++] = (uint8_t)(uhdata);
      USART2_RxFlag=1;
      USART2_RxTimeCont=0;
      return;
    }
  }
  else
  {
    ClearUSART_Error(huart);
    //HAL_UART_IRQHandler(huart);
    HAL_UART_Receive_IT(&huart2, (uint8_t *)USART_Rx2Buff, MAX_RX_BUFF);
  }
}
void uartOpenReceive(void)
{
  HAL_UART_Receive_IT(&huart1, (uint8_t *)USART_Rx1Buff, MAX_RX_BUFF);
  HAL_UART_Receive_IT(&huart2, (uint8_t *)USART_Rx2Buff, MAX_RX_BUFF);
}

void ConsoleControlTask(void)
{
    if(USART1_RxFlag==1&&USART1_RxTimeCont>=5)
    {
      RxUsartNum=0;
			ResolutionProtocol(0,USART_Rx1Buff,USART_Rx1Cont);
			memset(USART_Rx1Buff,0,sizeof(USART_Rx1Buff));
			USART_Rx1EndFlag=0;
			USART_Rx1Cont=0;
			USART1_RxFlag=0;
			USART1_RxTimeCont=0;   
    }
    if(USART2_RxFlag==1&&USART2_RxTimeCont>=5)
    {
      RxUsartNum=1;
      ResolutionProtocol(1,USART_Rx2Buff,USART_Rx2Cont);
      memset(USART_Rx2Buff,0,sizeof(USART_Rx2Buff));
      USART_Rx2EndFlag=0;
      USART_Rx2Cont=0;
      USART2_RxFlag=0;
      USART2_RxTimeCont=0;        
    }
		
		if((READ_REG(huart2.Instance->CR1)&0x120) == 0)
			huart2.Instance->CR1 |= (0x12<<4);
}

HAL_StatusTypeDef sentUartData(uint8_t *data,uint8_t len)
{
  HAL_StatusTypeDef status;
  if(RxUsartNum==0)
    status=HAL_UART_Transmit(&huart1, data, len, 100);
  else
    status=HAL_UART_Transmit(&huart2, data, len, 100);
  return status;
}

HAL_StatusTypeDef SendCommunicationData(uint8_t ComMode,uint8_t *data,uint8_t len)
{
  HAL_StatusTypeDef status;
  if(RxUsartNum==0)
    status=HAL_UART_Transmit(&huart1, data, len, 100);
  else if(RxUsartNum==1)
    status=HAL_UART_Transmit(&huart2, data, len, 100);
  return status;
}

uint8_t GetUartNum(void)
{
  return RxUsartNum;
}
