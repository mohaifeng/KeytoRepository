#ifndef __KCONFIG_H__
#define __KCONFIG_H__

#include "main.h"
#include "stm32g4xx_hal.h"
#include <stdbool.h>

////编译年份
//#define YEAR (((__DATE__ [9] - '0')) * 10 + (__DATE__ [10] - '0'))  
////编译月份  
//#define MONTH (__DATE__ [2] == 'n' ? 1 \
//    : __DATE__ [2] == 'b' ? 2 \
//    : __DATE__ [2] == 'r' ? (__DATE__ [0] == 'M' ? 3 : 4) \
//    : __DATE__ [2] == 'y' ? 5 \
//    : __DATE__ [2] == 'n' ? 6 \
//    : __DATE__ [2] == 'l' ? 7 \
//    : __DATE__ [2] == 'g' ? 8 \
//    : __DATE__ [2] == 'p' ? 9 \
//    : __DATE__ [2] == 't' ? 10 \
//    : __DATE__ [2] == 'v' ? 11 : 12)  
////编译日  
//#define DAY ((__DATE__ [4] == ' ' ? 0 : __DATE__ [4] - '0') * 10 \
//    + (__DATE__ [5] - '0'))
//  
////版本号  
//#define SYSTERM_VERSION_NUMBER (115)
//                
////编译日期加版本号
//#define SYSTERM_VERSION     (((YEAR*100 + MONTH)*100 + DAY)*1000 + SYSTERM_VERSION_NUMBER)


#define SYS_FACILITY_ID			0X00010011
#define SYS_SOFT_VERSIONS		230927118 //SYSTERM_VERSION

#define ENC_SPEED_ARRAY_SIZE  16

//#define FOR_NCS_VERSION			//钢研纳克版本

#ifdef FOR_NCS_VERSION
#define FLASH_INIT_FLAG    	123456789
#else
#define FLASH_INIT_FLAG    	220920114
#endif


#define USED_ENCODER				0

#if (USED_ENCODER == 1)
#define ENCODER_TASK_TIME 		10
#endif

typedef struct
{
  uint8_t   Port;                //通道编号
  int  MotorPosition;       		 //电机位置
} Valve_Port_t;


typedef struct 
{
  uint16_t StopMode;								//0:HardStop,1:SoftStop
	uint16_t MicroStep;								//细分
  uint16_t StartPhaseCurrent;       //起始相电流
  uint16_t PhaseCurrent;						//额定电流
	uint16_t PhaseVoltage;						//额定电压
  uint32_t Acceleration;						//加速度
  uint32_t Deceleration;						//减速度
  uint32_t RunSpeed;								//运行速度(用来保存速度模式的运行速度)
  uint32_t MaxSpeed;								//最大速度(用来保存位置模式下的最大速度)
  uint32_t MinSpeed;								//最小速度
	uint8_t PositionDir;							//方向
	uint8_t ParaUseMicroStep;					//参数计算采用细分
	uint8_t	StepClockModeEn;					//位置模式是否采用stepclock模式
}StepMotorParaStruct_TypeDef;


typedef struct
{
	uint8_t ResetPort;
  uint16_t ResetSpeed;                              //复位速度（单位 转/分）  5-350转/分
  uint32_t FWD_ValveOptocouplerGapStep;             //光耦挡片缺口宽度  
  uint32_t OptPosition[30];              	//位置光耦触发位置
	uint32_t OptInterval;										//位置光耦间隔
	uint8_t OptGapNum;											//光耦触发缺口个数
  Valve_Port_t Port[30];                 //端口   
  uint8_t PortNum;                       //端口个数
	uint8_t ErrorAutoResetAllowCont;	
	int FlowClockwisePhaseCompensation;    	//顺时针补偿值           
  int FlowAnticlockwisePhaseCompensation; //逆时针补偿值  
	uint8_t Using_ReductionBox;
	float Reduction_Ratio;
	uint16_t MaxStep;
	uint8_t MaxGapStep;
} Flow_Config_t;

typedef struct
{
  uint16_t PulseNumber; 				/*!< Number of pulses per revolution, provided by each
																		of the two encoder signals, multiplied by 4 */
	uint8_t  Direction;						//编码器方向
	uint8_t EncoderEn;						//是否启用编码器
	
}EncoderParaStruct_TypeDef;

typedef struct
{ 
	uint8_t ContDir;
	int32_t EncAvrSpeed;									//编码器平均速度,单位为cont/s,其中cont为编码器计数值

  /* HW Settings */
  uint16_t TimerOverflowNb;    /*!< Number of overflows occurred since
                                        last speed measurement event*/


  int32_t PreviousCapture;            /*!< Timer counter value captured during
                                        previous speed measurement event*/

  int32_t DeltaCapturesBuffer[ENC_SPEED_ARRAY_SIZE]; /*!< Buffer used to store
                                        captured variations of timer counter*/

  volatile uint8_t DeltaCapturesIndex; /*! <Buffer index*/
} ENCODER_Handle_t;

typedef struct
{
  uint8_t Add;                  //地址
  uint8_t RS232_BaudRate;      //RS232波特率
  uint8_t RS485_BaudRate;      //RS485波特率
  uint16_t CAN_BaudRate;        //CAN波特率
  uint8_t CAN_TargetAdd;        //CAN目标地址
} Communication_Config_t;


typedef struct
{
  uint8_t IntoBootMode;
  uint32_t InitFlag;
  uint8_t  WorkMode;      //电机工作模式，0:步进电机微步模式，1：流体控制模式
	uint8_t AutoResetZeroEn;
	uint8_t AutoAgingEnable;
  Flow_Config_t ValveConfig;
  StepMotorParaStruct_TypeDef MotorValveConfig;
  Communication_Config_t CommunicationConfig;   //通讯配置
	EncoderParaStruct_TypeDef EncoderConfig;			//编码器配置
  uint32_t Password;
	uint8_t Not_Change;														//不适用装换10通模式
	uint8_t IIC_Addr;
	uint8_t i2c_valve_profile;
	uint8_t i2c_valvecmd;
	uint8_t UserData[50];
} SysConfig_type;


typedef struct
{
	uint32_t ValveCont;
	uint32_t PlungerCont;
} SysRunCont_type;


//extern SysConfig_type  SysConfig;
extern SysRunCont_type  SysRunCont;


void Param_Init(void);

void SysTick_Init(uint8_t SYSCLK);
void delay_us(uint32_t nus);
void delay_ms(uint16_t nms);
uint32_t my_abs(int32_t data);
void Set_None_ReductionBox_PortAndOptPosition(uint8_t num);
void Set_Multi_ReductionBox_PortAndOptPosition(uint8_t num);
void setValueChannel(uint8_t number);
uint8_t ChangePositionBack(uint8_t p);
uint8_t ChangePosition(uint8_t p);
#endif

