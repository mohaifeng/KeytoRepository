/**
******************************************************************************
* @文件名           : simulate_slave_I2C_driver.c
******************************************************************************
* @版权信息
* Copyright (c) 2023 成都开图医疗系统科技有限公司.
* All rights reserved
******************************************************************************
* @关联库：STM32 HAL_I2C库 string库 stdbool库
* @程序说明
  1、该程序为模拟I2C从机程序，用于芯片硬件I2C接口被占用或不够使用的情况。
	2、在使用该模块前，应在CubeMX里面对I2C_SCL、I2C_SDA、ENI2C进行IO配置，开启中断服务函数，优先级2:0。
			I2C_SCL、I2C_SDA： 外部中断IO模式。选择上升沿和下降沿触发模式，只要发生电平转换就会进入中断。
			ENI2C：推挽输出模式。对I2C进行使能。
	3、在使用该模块前，需要在CubeMX生成的keil工程中的stm32xxxx_it.c文件中对 simulate_slave_I2C_driver.h 头文件用#include进行包含。
			并在中断服务函数里面调用 SI2C_IRQHandler(); 函数。将HAL库生成的 HAL_GPIO_EXTI_IRQHandler(); 注释掉。
	
******************************************************************************
*/

#include "simulate_slave_I2C_driver.h"
#include "string.h"
#include "stdbool.h"
#include "valve_control.h"
#include "protocol.h"
/*********************************************宏定义***************************/
#define SI2C_SetGPIO(pgpio, PinState)		HAL_GPIO_WritePin(pgpio->GPIOx, pgpio->GPIO_Pin, PinState)	//写I2C电平
#define SI2C_ReadGPIO(pgpio)						HAL_GPIO_ReadPin(pgpio->GPIOx, pgpio->GPIO_Pin)							//读I2C电平
/*********************************************枚举*****************************/
typedef enum
{
	Input 	= 0,
	Output	
}GPIO_Dir_u;			//IO方向
typedef enum
{
	SI2C_WRITE	= 0,
	SI2C_READ		= 1
}SI2C_WR_u;				//读写
typedef enum
{
	SI2CRUN_IDLE 	= 0,		//SI2C空闲
	SI2CRUN_START	= 1,
	SI2CRUN_DATA	= 2, 
	SI2CRUN_ACK		= 3,
	SI2CRUN_ACKING	= 6,
	SI2CRUN_NACK	= 4,
	SI2CRUN_END		= 5,
	
}SI2CRun_State_u;		//SI2C运行状态机
/*********************************************结构体定义***********************/
typedef struct
{
	SI2CRun_State_u		SI2CRun_State;			//状态
	uint8_t 					SI2C_Addr;			//I2C	从机地址
	uint8_t						*SI2C_Sendbuf;	//从机发送数据
	uint8_t						SI2C_SendIndex;	//发送索引
	uint8_t						*SI2C_Readbuf;	//从机接收数据
	uint8_t						SI2C_ReadIndex;	//接收索引
	uint8_t						SI2C_BitCount;	//bit位
	SI2C_WR_u					SI2C_WR_Flag;		//读写标志位
	uint8_t						SI2C_Byte;			//一字节
	bool							SI2C_AddrFlag;	//地址匹配标志位
	uint8_t						SI2C_Readlen;		//从机I2C从主机读取数据的长度
	bool							SI2C_ReadFinished;
	bool							SI2C_ACKEnableFlag;			//从机I2C是否应答标志位
	uint16_t					SI2C_TimeOutCont;				//从机I2C计数值
	bool							SI2C_StartTimeContFlag;	//从机I2C开始计时标志
}SI2C_data_t;							//SI2C数据
typedef struct 
{
  GPIO_TypeDef  *GPIOx;
	uint16_t 			GPIO_Pin;
}HAL_GPIO_t;				//GPIO结构体
typedef struct 
{
	HAL_GPIO_t 		*pgpio_SCL;				//SI2C SCL时序引脚
	HAL_GPIO_t 		*pgpio_SDA;				//SI2C SDA数据引脚
	HAL_GPIO_t 		*pgpio_EnI2C;			//SI2C 使能引脚
	const	IRQn_Type		SI2C_SCLIRQ;		//中断源
	const	IRQn_Type		SI2C_SDAIRQ;
}SI2C_GPIO_t;				//SI2C GPIO结构体
/*********************************************变量定义*************************/
static HAL_GPIO_t Gpio_SCL = 
{
	.GPIOx 		= I2C_SCL_GPIO_Port,
	.GPIO_Pin = I2C_SCL_Pin,	
};
static HAL_GPIO_t Gpio_SDA = 
{
	.GPIOx 		= I2C_SDA_GPIO_Port,
	.GPIO_Pin = I2C_SDA_Pin,	
};
static HAL_GPIO_t Gpio_EnI2C = 
{
	.GPIOx 		= I2C_EN_GPIO_Port,
	.GPIO_Pin = I2C_EN_Pin,	
};
static SI2C_GPIO_t SI2C_HALGpio = 
{
	.pgpio_SCL = &Gpio_SCL,
	.pgpio_SDA = &Gpio_SDA,
	.pgpio_EnI2C = &Gpio_EnI2C,
	.SI2C_SCLIRQ = I2C_SCL_EXTI_IRQn,
	.SI2C_SDAIRQ = I2C_SDA_EXTI_IRQn,
};
static uint8_t Si2c_Sendbuf[8];	//待发数组
static uint8_t Si2c_Readbuf[8];	//接收数组
static uint8_t Si2c_ReadSaveBuf[8];
//SI2C定义
static SI2C_data_t SI2C_data =
{
	.SI2C_Addr				= SI2C_INITADDR,
	.SI2CRun_State 		= SI2CRUN_IDLE,
	.SI2C_Sendbuf 		= Si2c_Sendbuf,
	.SI2C_Readbuf 		= Si2c_Readbuf,
	.SI2C_SendIndex 	= 0,
	.SI2C_ReadIndex 	= 0,
	.SI2C_BitCount 		= 0,
	.SI2C_WR_Flag 		= SI2C_WRITE,
	.SI2C_Byte				= 0,
	.SI2C_AddrFlag 		= false,
	.SI2C_Readlen 		= 0,
	.SI2C_ReadFinished	= false,
	.SI2C_ACKEnableFlag = false,
	.SI2C_TimeOutCont = 0,
	.SI2C_StartTimeContFlag = false,
};
static SI2C_data_t *pSI2C = &SI2C_data;
static SI2C_GPIO_t *pSI2C_gpio = &SI2C_HALGpio;
/*********************************************函数声明*************************/
static void SI2C_DataClearInit(void);
static void SI2C_SetGPIO_Dirput(HAL_GPIO_t *phal_gpio, GPIO_Dir_u gpio_dir);
static SI2CRun_State_u SI2C_SCL_UpToDown_Task(SI2CRun_State_u si2c_runstate);
static SI2CRun_State_u SI2C_SCL_DownToUp_Task(SI2CRun_State_u si2c_runstate);
static void SI2C_SetState(SI2CRun_State_u si2c_runstate);
static bool SI2C_ACK_Enable(void);
static void SI2C_StartTimeoutCounter(void);
static void SI2C_EndTimeoutCounter(void);
/*******************************************************************************
函数功能：	从机I2C GPIO初始化
函数说明：	配置IO中断模式，开启中断
全局变量：	pSI2C
输入参数：	无					
输出参数：	无
返回值：	无
修改记录：
	V1.0.0，陈佳宇,2023年8月16：首次创建
*******************************************************************************/
void SI2C_GPIO_Init(void)
{
  GPIO_InitTypeDef GPIO_InitStruct = {0};
	
	__HAL_RCC_GPIOA_CLK_ENABLE();
  __HAL_RCC_GPIOB_CLK_ENABLE();

  HAL_GPIO_WritePin(pSI2C_gpio->pgpio_EnI2C->GPIOx, pSI2C_gpio->pgpio_EnI2C->GPIO_Pin, GPIO_PIN_SET);

	GPIO_InitStruct.Pin = pSI2C_gpio->pgpio_SDA->GPIO_Pin;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
  GPIO_InitStruct.Pull = GPIO_PULLUP;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_OD;						//配置成开漏输出
	HAL_GPIO_Init(pSI2C_gpio->pgpio_SDA->GPIOx, &GPIO_InitStruct);
  GPIO_InitStruct.Mode = GPIO_MODE_IT_RISING_FALLING;
  HAL_GPIO_Init(pSI2C_gpio->pgpio_SDA->GPIOx, &GPIO_InitStruct);
	
	GPIO_InitStruct.Pin = pSI2C_gpio->pgpio_SCL->GPIO_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_IT_RISING_FALLING;
  GPIO_InitStruct.Pull = GPIO_PULLUP;
  HAL_GPIO_Init(pSI2C_gpio->pgpio_SCL->GPIOx, &GPIO_InitStruct);
	
	GPIO_InitStruct.Pin = pSI2C_gpio->pgpio_EnI2C->GPIO_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_PULLUP;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(pSI2C_gpio->pgpio_EnI2C->GPIOx, &GPIO_InitStruct);
	
	HAL_NVIC_SetPriority(pSI2C_gpio->SI2C_SCLIRQ, 0, 0);
	HAL_NVIC_SetPriority(pSI2C_gpio->SI2C_SDAIRQ, 0, 0);
  HAL_NVIC_EnableIRQ(pSI2C_gpio->SI2C_SCLIRQ);
  HAL_NVIC_EnableIRQ(pSI2C_gpio->SI2C_SDAIRQ);
}
/*******************************************************************************
函数功能：	修改从机I2C地址
函数说明：	更改内部I2C数据地址，清空内部数据
全局变量：	SI2C_data：内部全局变量
输入参数：	newaddr：新地址					
输出参数：	无
返回值：	无
修改记录：
	V1.0.0，陈佳宇,2023年4月14：首次创建
*******************************************************************************/
void SI2C_ChangeIICAddress(uint8_t newaddr)
{
	HAL_NVIC_DisableIRQ(pSI2C_gpio->SI2C_SCLIRQ);	//关闭中断
	HAL_NVIC_DisableIRQ(pSI2C_gpio->SI2C_SDAIRQ);	//关闭中断
	pSI2C->SI2C_Addr = newaddr & 0xFE;
	SI2C_DataClearInit();
	HAL_NVIC_EnableIRQ(pSI2C_gpio->SI2C_SDAIRQ);
	HAL_NVIC_EnableIRQ(pSI2C_gpio->SI2C_SCLIRQ);
}
/*******************************************************************************
函数功能：	读取从机I2C地址
函数说明：	
全局变量：	pSI2C：内部全局变量
输入参数：	无					
输出参数：	无
返回值：		SI2C地址
修改记录：
	V1.0.0，陈佳宇,2023年8月17：首次创建
*******************************************************************************/
uint8_t SI2C_ReadIICAddress(void)
{
	return pSI2C->SI2C_Addr;
}
/*******************************************************************************
函数功能：	从机I2C发数据给主机
函数说明：	更新发送缓存区
全局变量：	Si2c_Sendbuf
输入参数：	*sendbuf：需要发送的数据串
						len：长度
输出参数：	无
返回值：		SI2C_State_u：状态值
修改记录：
	V1.0.0，陈佳宇,2023年4月14：首次创建
*******************************************************************************/
SI2C_State_u SI2C_SendToMaster(char *sendbuf, uint8_t len)
{
  HAL_NVIC_DisableIRQ(pSI2C_gpio->SI2C_SCLIRQ);	//关闭中断
	HAL_NVIC_DisableIRQ(pSI2C_gpio->SI2C_SDAIRQ);	//关闭中断
	for(uint8_t i = 0; i < len; i++)
	{
		Si2c_Sendbuf[i] = sendbuf[i];
	}
	HAL_NVIC_EnableIRQ(pSI2C_gpio->SI2C_SDAIRQ);
	HAL_NVIC_EnableIRQ(pSI2C_gpio->SI2C_SCLIRQ);
	if(strlen((char*)Si2c_Sendbuf) == len)
	{
		return SI2C_SUCCEED;
	}
	else
	{
		return SI2C_FAULT;
	}		
}
/*******************************************************************************
函数功能：	从机I2C接收数据
函数说明：	将I2C读到的数据保存在readbuf
全局变量：	SI2C_data：I2C数据
输入参数：	len：接收长度					
输出参数：	*readbuf：读到的数据
返回值：		SI2C_SUCCEED：读取成功
						SI2C_FAULT：	读取失败
修改记录：
	V1.0.0，陈佳宇,2023年4月14：首次创建
*******************************************************************************/
SI2C_State_u SI2C_ReadFromMaster(char *readbuf, uint8_t len)
{
	pSI2C->SI2C_Readlen = len + 1;
//	HAL_NVIC_DisableIRQ(pSI2C_gpio->SI2C_SCLIRQ);	//关闭中断
//	HAL_NVIC_DisableIRQ(pSI2C_gpio->SI2C_SDAIRQ);	//关闭中断
//	if(pSI2C->SI2C_ReadFinished == false)
//	{
//		return SI2C_FAULT;
//	}
//	pSI2C->SI2C_ReadFinished = false;
	for(uint8_t i = 0; i < len; i++)
	{
		readbuf[i] = Si2c_ReadSaveBuf[i];			//+1 地址位
	}
//	HAL_NVIC_EnableIRQ(pSI2C_gpio->SI2C_SDAIRQ);
//	HAL_NVIC_EnableIRQ(pSI2C_gpio->SI2C_SCLIRQ);
	if(strlen((char*)Si2c_ReadSaveBuf) == len)
	{
		memset(Si2c_ReadSaveBuf,0,strlen((char*)Si2c_ReadSaveBuf));
		return SI2C_SUCCEED;
	}
	else
	{
		return SI2C_FAULT;
	}		
}
/*******************************************************************************
函数功能：	I2C 状态设置
函数说明：	对I2C状态进行设置
全局变量：	SI2C_data：I2C数据
输入参数：	si2c_runstate：I2C运行状态					
输出参数：	无
返回值：		无
修改记录：
	V1.0.0，陈佳宇,2023年8月14：首次创建
*******************************************************************************/
static void SI2C_SetState(SI2CRun_State_u si2c_runstate)
{
	pSI2C->SI2CRun_State = si2c_runstate;
}
/*******************************************************************************
函数功能：	I2C 数据初始化
函数说明：	对I2C数据进行初始化
全局变量：	SI2C_data：I2C数据
输入参数：	无					
输出参数：	无
返回值：		无
修改记录：
	V1.0.0，陈佳宇,2023年4月13：首次创建
*******************************************************************************/
static void SI2C_DataClearInit(void)
{
//	pSI2C->SI2CRun_State = SI2CRUN_IDLE;
//	memset(Si2c_Sendbuf, 0 , sizeof(Si2c_Sendbuf));
	memset(Si2c_Readbuf, 0 , sizeof(Si2c_Readbuf));
	pSI2C->SI2C_SendIndex = 0;
	pSI2C->SI2C_ReadIndex = 0;
	pSI2C->SI2C_BitCount = 0;
	pSI2C->SI2C_WR_Flag = SI2C_WRITE;
	pSI2C->SI2C_Byte = 0;
	pSI2C->SI2C_AddrFlag = false;
	pSI2C->SI2C_Readlen = 0;
//	pSI2C->SI2C_ReadFinished = false;
	SI2C_SetGPIO_Dirput(pSI2C_gpio->pgpio_SDA, Input);
//	SI2C_SetGPIO_Dirput(pSI2C_gpio->pgpio_SCL, Input);
}
/*******************************************************************************
函数功能：	设置GPIO输入输出
函数说明：	通过改变GPIO MODER寄存器的值来改变输入输出方向
全局变量：	无
输入参数：	hal_gpio：头文件中配置的硬件管脚
						gpio_dir：输入输出方向
输出参数：	无
返回值：		无
修改记录：
	V1.0.0，陈佳宇,2023年4月13：首次创建
*******************************************************************************/
static void SI2C_SetGPIO_Dirput(HAL_GPIO_t *phal_gpio, GPIO_Dir_u gpio_dir)
{
	uint32_t temp; 
	uint16_t gpio_pin = phal_gpio->GPIO_Pin;
	uint8_t pin = 0;
	
	while(gpio_pin >>= 1)
	{
		pin++;
	}
	temp = phal_gpio->GPIOx->MODER;	//获取IO输入输出模式
	temp &= (~(0x00000003U << (pin * 2)));
	temp |= ((gpio_dir & 0x00000003U) << (pin * 2));
	phal_gpio->GPIOx->MODER = temp;
}
/*******************************************************************************
函数功能：	是否允许应答
函数说明：	根据电机状态是否允许应答，只有电机在空闲状态下才允许应答
全局变量：	VALVE_IDLE 电机状态
输入参数：	无					
输出参数：	bool: true: 允许应答
								false: 不允许应答
返回值：		无
修改记录：
	V1.0.0，陈佳宇,2023年8月16：首次创建
*******************************************************************************/
static bool SI2C_ACK_Enable(void)
{
	uint8_t valvestate = Valve_GetFaultStatus();
	if(VALVE_IDLE == valvestate || VALVE_FAULT_IDLE == valvestate)
	{
		return true;
	}
	else
	{
		return false;
	}
}
/*******************************************************************************
函数功能：	开始超时计时
函数说明：	对标志位进行赋值
全局变量：	SI2C_data：I2C数据
输入参数：	无					
输出参数：	无
返回值：		无
修改记录：
	V1.0.0，陈佳宇,2023年8月16：首次创建
*******************************************************************************/
static void SI2C_StartTimeoutCounter(void)
{
	pSI2C->SI2C_TimeOutCont = 0;
	pSI2C->SI2C_StartTimeContFlag = true;	
}
/*******************************************************************************
函数功能：	结束超时计时
函数说明：	对标志位进行赋值
全局变量：	SI2C_data：I2C数据
输入参数：	无					
输出参数：	无
返回值：		无
修改记录：
	V1.0.0，陈佳宇,2023年8月16：首次创建
*******************************************************************************/
static void SI2C_EndTimeoutCounter(void)
{
	pSI2C->SI2C_TimeOutCont = 0;
	pSI2C->SI2C_StartTimeContFlag = false;	
}
//
/*******************************************************************************
函数功能：	从机I2C功能任务，包含掉电保存，超时计数
函数说明：	从机I2C功能任务，在主函数里面调用，根据SI2C_TASK_TIME间隔来执行
全局变量：	SI2C_data：I2C数据
输入参数：	无					
输出参数：	无
返回值：		无
修改记录：
	V1.0.0，陈佳宇,2023年8月16：首次创建
*******************************************************************************/
void SI2C_FunctionTask(void)
{
	SI2C_Control_Task();			//切阀
	SI2C_FlashSave_Task();		//掉电保存
	if(pSI2C->SI2C_StartTimeContFlag == true)
	{
		pSI2C->SI2C_TimeOutCont++;	//超时计数
	}
	if(pSI2C->SI2C_TimeOutCont >= (SI2C_TIMEOUT / SI2C_TASK_TIME))	//超时时间 / 计数间隔
	{
		SI2C_EndTimeoutCounter();
		//恢复从机I2C SDA引脚
		SI2C_SetState(SI2CRUN_IDLE);
		SI2C_SetGPIO_Dirput(pSI2C_gpio->pgpio_SDA, Input);
	}
}
/*******************************************************************************
函数功能：	外部中断调用中断服务函数
函数说明：	在外部中断中调用从机I2C中断函数
全局变量：	无
输入参数：	无					
输出参数：	无
返回值：		无
修改记录：
	V1.0.0，陈佳宇,2023年8月16：首次创建
*******************************************************************************/
void EXTI_IRQHandler(void)
{
  /* USER CODE BEGIN EXTI15_10_IRQn 0 */
	SI2C_IRQHandler();
	#if 0
  /* USER CODE END EXTI15_10_IRQn 0 */
  HAL_GPIO_EXTI_IRQHandler(I2C_SDA_Pin);
  HAL_GPIO_EXTI_IRQHandler(I2C_SCL_Pin);
  /* USER CODE BEGIN EXTI15_10_IRQn 1 */
	#endif
  /* USER CODE END EXTI15_10_IRQn 1 */
}
/*******************************************************************************
函数功能：	实现I2C通信
函数说明：	在外部中断中调用该函数，通过外部中断进入该函数，判断电平变化，执行对应的I2C操作
全局变量：	SI2C_data：I2C数据
输入参数：	无					
输出参数：	无
返回值：		无
修改记录：
	V1.0.0，陈佳宇,2023年4月13：首次创建
*******************************************************************************/
void SI2C_IRQHandler(void)
{
	//SCL触发边沿变化
	if(__HAL_GPIO_EXTI_GET_IT(pSI2C_gpio->pgpio_SCL->GPIO_Pin) != 0x00u)
	{
    __HAL_GPIO_EXTI_CLEAR_IT(pSI2C_gpio->pgpio_SCL->GPIO_Pin);		//清除中断标志位
		
		//SCL下降沿，更新SDA
		if(!SI2C_ReadGPIO(pSI2C_gpio->pgpio_SCL))											
		{
			SI2C_SetState(SI2C_SCL_UpToDown_Task(pSI2C->SI2CRun_State));
		}
		//SCL上升沿，记录SDA有效
		else
		{
			SI2C_SetState(SI2C_SCL_DownToUp_Task(pSI2C->SI2CRun_State));
		}
	}
	//SDA触发边沿变化 
	else if(__HAL_GPIO_EXTI_GET_IT(pSI2C_gpio->pgpio_SDA->GPIO_Pin) != 0x00u)
  {
    __HAL_GPIO_EXTI_CLEAR_IT(pSI2C_gpio->pgpio_SDA->GPIO_Pin);
		
		if(!SI2C_ReadGPIO(pSI2C_gpio->pgpio_SCL))		
		{//SCL低电平 SDA变化不做处理
			return ;			
		}
		//SCL为高，SDA上升沿，start信号；SDA下降沿，STOP信号
		//SDA下降沿变化 	Start
		if(!SI2C_ReadGPIO(pSI2C_gpio->pgpio_SDA) &&	(pSI2C->SI2CRun_State == SI2CRUN_IDLE))	
		{
			SI2C_DataClearInit();					//数据初始化
			SI2C_SetState(SI2CRUN_START);
			//开始I2C通信计时
			SI2C_StartTimeoutCounter();
		}
		//SDA上升沿变化		End
		else if(SI2C_ReadGPIO(pSI2C_gpio->pgpio_SDA))											
		{
			SI2C_SetState(SI2CRUN_IDLE);
			SI2C_EndTimeoutCounter();
		}
		
	}
}
/*******************************************************************************
函数功能：	SCL下降沿任务
函数说明：	在该函数中通过si2c_runstate值来判别当前I2C通信出于什么状态，改进行什么操作
全局变量：	SI2C_data：I2C数据
输入参数：	si2c_runstate：I2C运行状态				
输出参数：	无
返回值：		SI2CRun_State_u：运行状态
修改记录：
	V1.0.0，陈佳宇,2023年4月14：首次创建
*******************************************************************************/
static SI2CRun_State_u SI2C_SCL_UpToDown_Task(SI2CRun_State_u si2c_runstate)
{
	switch(si2c_runstate)
	{
		case SI2CRUN_IDLE:
			break;
		case SI2CRUN_START:	//开始之后第一次进入，此时主机准备SDA数据，从机SDA配置成输入，等待数据
			SI2C_SetGPIO_Dirput(pSI2C_gpio->pgpio_SDA, Input);
			
			return SI2CRUN_DATA;
		case SI2CRUN_DATA:
			if(pSI2C->SI2C_WR_Flag == SI2C_READ)
			{
				pSI2C->SI2C_BitCount++;
				if(pSI2C->SI2C_BitCount < 8)
				{
//					SI2C_SetGPIO_Dirput(pSI2C_gpio->pgpio_SDA, Output);
					if(pSI2C->SI2C_Sendbuf[pSI2C->SI2C_SendIndex] & (0x80 >> pSI2C->SI2C_BitCount))	
					{
						SI2C_SetGPIO(pSI2C_gpio->pgpio_SDA, GPIO_PIN_SET);			
					}
					else
					{
						SI2C_SetGPIO(pSI2C_gpio->pgpio_SDA, GPIO_PIN_RESET);			
					}
				}
				else if(pSI2C->SI2C_BitCount == 8)	//从机发完一个byte 配置成输入等待主机应答
				{
					SI2C_SetGPIO_Dirput(pSI2C_gpio->pgpio_SDA, Input);
					pSI2C->SI2C_SendIndex++;
					pSI2C->SI2C_BitCount = 0;
					return SI2CRUN_ACK;
				}
			}
			break;
		case SI2CRUN_ACK:		//在SCL低电平期间，配置输出SDA低 第8bit下降沿
			SI2C_SetGPIO(pSI2C_gpio->pgpio_SDA, GPIO_PIN_RESET);
			SI2C_SetGPIO_Dirput(pSI2C_gpio->pgpio_SDA, Output);
			return SI2CRUN_ACKING;
		
		case SI2CRUN_ACKING:	//1Byte的应答 第9bit结束 恢复SDA继续传输数据
			if(pSI2C->SI2C_WR_Flag == SI2C_WRITE)		//写
			{
				SI2C_SetGPIO_Dirput(pSI2C_gpio->pgpio_SDA, Input);
			}
			else 	//读 第一个byte
			{
				SI2C_SetGPIO_Dirput(pSI2C_gpio->pgpio_SDA, Output);	//在SDA低电平期间准备数据
				if(pSI2C->SI2C_Sendbuf[pSI2C->SI2C_SendIndex] & 0x80)	
				{
					SI2C_SetGPIO(pSI2C_gpio->pgpio_SDA, GPIO_PIN_SET);			
				}
				else
				{
					SI2C_SetGPIO(pSI2C_gpio->pgpio_SDA, GPIO_PIN_RESET);			
				}
			}
			return SI2CRUN_DATA;
		
		case SI2CRUN_NACK:	//配置输入SDA
			SI2C_SetGPIO_Dirput(pSI2C_gpio->pgpio_SDA, Input);
			return SI2CRUN_END;
		
		case SI2CRUN_END:
			return SI2CRUN_IDLE;
	}
	return si2c_runstate;
}
/*******************************************************************************
函数功能：	SCL上升沿任务
函数说明：	在该函数中通过si2c_runstate值来判别当前I2C通信出于什么状态，改进行什么操作
全局变量：	SI2C_data：I2C数据
输入参数：	si2c_runstate：I2C运行状态				
输出参数：	无
返回值：		SI2CRun_State_u：运行状态
修改记录：
	V1.0.0，陈佳宇,2023年4月14：首次创建
*******************************************************************************/
static SI2CRun_State_u SI2C_SCL_DownToUp_Task(SI2CRun_State_u si2c_runstate)
{
	switch(si2c_runstate)
	{
		case SI2CRUN_IDLE:
			break;
		case SI2CRUN_START:
			break;
		case SI2CRUN_DATA:
			if(pSI2C->SI2C_WR_Flag == SI2C_WRITE)	//写
			{
				//开始数据接收
				if(SI2C_ReadGPIO(pSI2C_gpio->pgpio_SDA))
				{
					pSI2C->SI2C_Byte |= (0x01 << (7U - pSI2C->SI2C_BitCount));
				}
				pSI2C->SI2C_BitCount++;
				if(pSI2C->SI2C_BitCount < 8)
				{
					return SI2CRUN_DATA;
				}
				else if(pSI2C->SI2C_BitCount == 8)		//一个byte接收完毕
				{
					pSI2C->SI2C_BitCount = 0;
					if(pSI2C->SI2C_ReadIndex == 0)	//第一个数据地址
					{
						pSI2C->SI2C_ACKEnableFlag = SI2C_ACK_Enable();	//根据电机状态获取是否应答标志
						//地址匹配，且允许应答
						if(((pSI2C->SI2C_Byte & 0xFE) == pSI2C->SI2C_Addr) && (pSI2C->SI2C_ACKEnableFlag == true))	
						{
							pSI2C->SI2C_AddrFlag = true;
							pSI2C->SI2C_WR_Flag = (SI2C_WR_u)(pSI2C->SI2C_Byte & 0x01);
						}
						else
						{
							pSI2C->SI2C_AddrFlag = false;
							return SI2CRUN_NACK;
						}
					}
					pSI2C->SI2C_Readbuf[pSI2C->SI2C_ReadIndex] = pSI2C->SI2C_Byte;	//保存第一个			
					pSI2C->SI2C_ReadIndex++;
					pSI2C->SI2C_Byte = 0;
					
					if(pSI2C->SI2C_ReadIndex == pSI2C->SI2C_Readlen)
					{
//						pSI2C->SI2C_ReadFinished = true;	//读完成
						for(uint8_t i = 0; i < pSI2C->SI2C_Readlen - 1; i++)
						{
							Si2c_ReadSaveBuf[i] = pSI2C->SI2C_Readbuf[i + 1];
						}
					}
					if(pSI2C->SI2C_ReadIndex == 4)		//Titan协议 Addr : cmd : value : Csum
					{
						if(false == Slave_IIC_Protocol(pSI2C->SI2C_Readbuf))
						{
							return SI2CRUN_NACK;
						}
					}
					return SI2CRUN_ACK;
				}
			}
			else	//读
			{
				return si2c_runstate;
			}
		case SI2CRUN_ACK:
			if(pSI2C->SI2C_WR_Flag == SI2C_WRITE)
			{
				return SI2CRUN_ACKING;
			}
			else	//读
			{
//				SI2C_SetGPIO_Dirput(pSI2C_gpio->pgpio_SDA, Input);
				if(SI2C_ReadGPIO(pSI2C_gpio->pgpio_SDA))
				{
					return SI2CRUN_NACK;
				}
				else
				{
					return SI2CRUN_ACKING;
				}
			}
		case SI2CRUN_ACKING:	//上升沿期间，主机检测SDA电平
			return SI2CRUN_ACKING;
		
		case SI2CRUN_NACK:
			break;
		case SI2CRUN_END:
			return SI2CRUN_IDLE;
	}
	return si2c_runstate;
}

