#ifndef  __TMC5160_H__
#define  __TMC5160_H__

#include "main.h"
#include <stdlib.h>
#include "kconfig.h"

#define TMC5160_SD_MODE					0		//0：内部斜坡,DIR,STEP用作外部停止
																		//1：外部步进信号，DIR,STEP用作外部方向和步进信号
#if TMC5160_SD_MODE==0
	#define TMC5160_STOP_MODE			1		//0:内部停止指令
																		//1:REFR_DIR作为外部停止信号
#endif


//typedef struct 
//{
//  uint16_t StopMode;								//0:HardStop,1:SoftStop
//	uint16_t MicroStep;								//细分
//  uint16_t StartPhaseCurrent;       //起始相电流
//  uint16_t PhaseCurrent;						//额定电流
//	uint16_t PhaseVoltage;						//额定电压
//  uint32_t Acceleration;						//加速度
//  uint32_t Deceleration;						//减速度
//  uint32_t RunSpeed;								//运行速度(用来保存速度模式的运行速度)
//  uint32_t MaxSpeed;								//最大速度(用来保存位置模式下的最大速度)
//  uint32_t MinSpeed;								//最小速度
//	uint8_t PositionDir;							//方向
//	uint8_t ParaUseMicroStep;					//参数计算采用细分
//	uint8_t	StepClockModeEn;					//位置模式是否采用stepclock模式
//}StepMotorParaStruct_TypeDef;

#define TMC_SPI1_CS_L()		HAL_GPIO_WritePin(SPI1_CS_GPIO_Port, SPI1_CS_Pin, GPIO_PIN_RESET)		
#define TMC_SPI1_CS_H()		HAL_GPIO_WritePin(SPI1_CS_GPIO_Port, SPI1_CS_Pin, GPIO_PIN_SET)	
#define TMC_DRV_ON()			HAL_GPIO_WritePin(DRV_ENN_GPIO_Port, DRV_ENN_Pin, GPIO_PIN_RESET)		
#define TMC_DRV_OFF()			HAL_GPIO_WritePin(DRV_ENN_GPIO_Port, DRV_ENN_Pin, GPIO_PIN_SET)	

#if TMC5160_SD_MODE==0
#define TMC_EXT_STOP()			HAL_GPIO_WritePin(TMC_DIR_GPIO_Port, TMC_DIR_Pin, GPIO_PIN_SET)		
#define TMC_EXT_RELEASE()		HAL_GPIO_WritePin(TMC_DIR_GPIO_Port, TMC_DIR_Pin, GPIO_PIN_RESET)	
#endif


//将ustep/s速度转换为ustep/t速度，其中t= 2^24 / fclk,fclk=12M
#define Convert_usteps_To_ustept(usteps) ((uint64_t)usteps*0x1000000/12000000) 
//将ustep/t速度转换为ustep/s速度,v[Hz] = v[5160] * ( fCLK[Hz]/2 / 2^23 )
#define Convert_ustept_To_usteps(ustept) (((uint64_t)ustept*12000000)/0x1000000) 

////将ustep/s2加减速转换为ustep/ta2加减速，其中ta2(ustep/f2)= 2^41 / (fclk*fclk)=0.01527099483
//#define Convert_ustepss_To_usteptt(ustepss) (((uint64_t)ustepss*0x20000000000)/(12000000*12000000)) 

//将ustep/s^2加减速转换为ustep/ta^2加减速，ustep acceleration a[Hz/s]= a[5160] * fCLK[Hz]^2 / (512*256) / 2^24
#define ConvertACC_usteptt_To_ustepss(usteptt) ((((uint64_t)usteptt*12000000*12000000)/((uint64_t)0x20000000000)))

//将ustep/ta^2加减速转换为ustep/s^2加减速，ustep acceleration a[Hz/s]= a[5160] * fCLK[Hz]^2 / (512*256) / 2^24
//0.01527099483
//#define ConvertACC_ustepss_To_usteptt(ustepss) ((((uint64_t)ustepss*(uint64_t)0x20000000000))/(12000000*12000000))
#define ConvertACC_ustepss_To_usteptt(ustepss) (ustepss*0.01527099483)
/*↓基础操作函数*/
void TMC5160_SPIWriteInt(uint8_t reg, int32_t byte);
uint8_t TMC5160_SPI_ReadState(uint8_t reg);
int32_t TMC5160_ReadData(uint8_t reg);
void TMC5160_Init(void);
void Clear_Error(void);

/*控制操作函数*/
void Motor_HardStop(void);
void Motor_SoftStop(void);
void Motor_SpeedMove(uint8_t Dir,uint32_t speed);
void Motor_MovePosition(int32_t pos);
void Motor_SetDirection(uint8_t dir);
/*参数设置函数*/
void Motor_SetMicrostep(uint16_t step);
void Motor_SetPhaseCurrent(uint16_t current);
void Motor_SetVSTART(uint32_t uspeed);
void Motor_SetA1(uint32_t uspeed);
void Motor_SetV1(uint32_t uspeed);
void Motor_SetAMAX(uint32_t uspeed);
void Motor_SetVMAX(uint32_t uspeed);
void Motor_SetDMAX(uint32_t uspeed);
void Motor_SetD1(uint32_t uspeed);
void Motor_SetVSTOP(uint32_t uspeed);
/*参数获取*/
int32_t Get_MotorPosition(void);
int32_t Get_MotorSpeed(void);
void Set_MotorPosition(int32_t position);
uint32_t Get_RAMP_Status(void);
uint32_t Plunger_Get_RAMP_Status(void);
uint32_t Valve_Get_RAMP_Status(void);
uint32_t Get_Motor_Status(void);
void Motor_ClearReachPosition_Status(void);
#endif

