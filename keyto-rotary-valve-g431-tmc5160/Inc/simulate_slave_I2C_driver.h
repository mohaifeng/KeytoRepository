/**
  ******************************************************************************
  * @文件名           : simulate_slave_I2C_driver.h
  * @编写者           : 陈佳宇
	* @修改日期         : 2023/4/15
  ******************************************************************************
  * @版权信息
  * Copyright (c) 2023 成都开图医疗系统科技有限公司.
  * All rights reserved
******************************************************************************
  */
	
#ifndef __SIMULATE_MASTER_I2C_DRIVER_H
#define __SIMULATE_MASTER_I2C_DRIVER_H

#include "main.h"

#define I2C_SDA_Pin 				GPIO_PIN_14
#define I2C_SDA_GPIO_Port 	GPIOB
#define I2C_SDA_EXTI_IRQn 	EXTI15_10_IRQn
#define I2C_EN_Pin 					GPIO_PIN_15
#define I2C_EN_GPIO_Port 		GPIOB
#define I2C_SCL_Pin 				GPIO_PIN_15
#define I2C_SCL_GPIO_Port 	GPIOA
#define I2C_SCL_EXTI_IRQn 	EXTI15_10_IRQn
#define EXTI_IRQHandler			EXTI15_10_IRQHandler

#define SI2C_INITADDR				0x0E			//初始SI2C地址
#define SI2C_TIMEOUT				50				//SI2C超时时间 ms
#define SI2C_TASK_TIME			10					//SI2C任务时间


typedef enum
{
	SI2C_SUCCEED 					= 0,	//成功 
	SI2C_FAULT 						= 1,	//错误 给上一级
	
	SI2C_ADDR_ERROR 			= 2,	//地址错误
	SI2C_PARAMETER_ERROR 	= 3,	//参数错误
	SI2C_GPIO_ENCLK_ERROR = 4,	//GPIO使能错误
	SI2C_GPIO_INIT_ERROR 	= 5,	//GPIO初始化错误
	SI2C_ACK_ERROR 				= 6,	//应答失败
	SI2C_ACK_SUCCEED 			= 7,	//应答成功
	SI2C_SEND_ERROR 			= 8,	//发送失败
	
}SI2C_State_u;						//SI2C状态

//中断服务函数
void SI2C_IRQHandler(void);

//外部调用
void SI2C_GPIO_Init(void);										//从机I2C GPIO初始化
void SI2C_ChangeIICAddress(uint8_t newaddr);	//更改从机地址
uint8_t SI2C_ReadIICAddress(void);
SI2C_State_u SI2C_SendToMaster(char *sendbuf, uint8_t len);
SI2C_State_u SI2C_ReadFromMaster(char *readbuf, uint8_t len);
void SI2C_FunctionTask(void);


#endif

















