#include "tmc5160.h" 
#include "delay.h"
#include "string.h"
#include "flash_program.h"
/**************************************************/
//外部变量声明
/**************************************************/
extern SPI_HandleTypeDef hspi1;

/**************************************************/
//变量定义
/**************************************************/
uint32_t Motor_Power = 0x00060001;
uint8_t OPT_Check = 0;
uint8_t ExeCmd = 0;
/*******************************************************************************************************/
/*
↓TMC5160基础操作函数↓
*/
/*******************************************************************************************************/
/***********************
函数功能：通过软件延时，单位us
函数说明：
************************/
void TMC_Delay(uint32_t delay_time)
{
	delay_time=delay_time*5;
	while(delay_time--)
		__NOP();
}
/*
功能：tmc5160spi数据读取
输入：需要读取的状态寄存器地址
返回：tmc5160相应寄存器的数据
*/
int32_t TMC5160_ReadData(uint8_t reg)
{	  
	HAL_StatusTypeDef status;
  int32_t Ret = 0;     
	uint8_t SPI_tx_data[5],SPI_rx_data[5];

	SPI_tx_data[0] = reg;
	SPI_tx_data[1]= 0;
	SPI_tx_data[2]= 0;
	SPI_tx_data[3]= 0;
	SPI_tx_data[4]= 0;
	memset(SPI_rx_data,0,sizeof(SPI_rx_data));
	
	TMC_SPI1_CS_L();
	status = HAL_SPI_TransmitReceive(&hspi1,SPI_tx_data,SPI_rx_data,5,200);
	TMC_SPI1_CS_H();
	TMC_Delay(1);
	//因TMC5160读取的是上一次的数据，所以这里需要读两次
	TMC_SPI1_CS_L();
	HAL_SPI_TransmitReceive(&hspi1,SPI_tx_data,SPI_rx_data,5,200);
	TMC_SPI1_CS_H();

  Ret = (SPI_rx_data[1]<<24) | (SPI_rx_data[2]<<16) | (SPI_rx_data[3]<<8) | (SPI_rx_data[4]); 

  return Ret; 
}

int32_t Valve_TMC5160_ReadData(uint8_t reg)
{	  
	HAL_StatusTypeDef status;
  int32_t Ret = 0;     
	uint8_t SPI_tx_data[5],SPI_rx_data[5];

	SPI_tx_data[0] = reg;
	SPI_tx_data[1]= 0;
	SPI_tx_data[2]= 0;
	SPI_tx_data[3]= 0;
	SPI_tx_data[4]= 0;
	memset(SPI_rx_data,0,sizeof(SPI_rx_data));
	TMC_SPI1_CS_L();
	status = HAL_SPI_TransmitReceive(&hspi1,SPI_tx_data,SPI_rx_data,5,200);
	TMC_SPI1_CS_H();
	TMC_Delay(5);
	//因TMC5160读取的是上一次的数据，所以这里需要读两次
	TMC_SPI1_CS_L();
	HAL_SPI_TransmitReceive(&hspi1,SPI_tx_data,SPI_rx_data,5,200);
	TMC_SPI1_CS_H();
  Ret = (SPI_rx_data[1]<<24) | (SPI_rx_data[2]<<16) | (SPI_rx_data[3]<<8) | (SPI_rx_data[4]); 

  return Ret; 
}

/*
功能：tmc5160spi寄存器写入数据
输入：reg :需要写入的寄存器地址
			byte:需要写入的数据
返回：tmc5160相应寄存器的数据
*/
void TMC5160_SPIWriteInt(uint8_t reg, int32_t byte)
{		  
	HAL_StatusTypeDef status;
	int8_t SPI_tx_data[5];
	SPI_tx_data[0]= reg | 0x80;
	SPI_tx_data[1]= byte>>24;
	SPI_tx_data[2]= byte>>16;
	SPI_tx_data[3]= byte>>8;
	SPI_tx_data[4]= byte;
	
	TMC_SPI1_CS_L();
	status = HAL_SPI_Transmit(&hspi1,(uint8_t*)SPI_tx_data,5,200);
	TMC_SPI1_CS_H();
}

void Valve_TMC5160_SPIWriteInt(uint8_t reg, int32_t byte)
{		  
	HAL_StatusTypeDef status;
	int8_t SPI_tx_data[5];
	SPI_tx_data[0]= reg | 0x80;
	SPI_tx_data[1]= byte>>24;
	SPI_tx_data[2]= byte>>16;
	SPI_tx_data[3]= byte>>8;
	SPI_tx_data[4]= byte;
	TMC_SPI1_CS_L();
	status = HAL_SPI_Transmit(&hspi1,(uint8_t*)SPI_tx_data,5,200);
	TMC_SPI1_CS_H();
}
/*
功能：tmc5160spi初始化
输入：无
返回：无
*/
void TMC5160_Init(void)
{	
	/**********************,MOTOR_VALVE*********************/
	TMC5160_SPIWriteInt(0x10, 	0x00020F00);//Power_Set.Power_Config); 	// IHOLD_IRUN: IHOLD=9, IRUN=15 , IHOLDDELAY=2
	TMC5160_SPIWriteInt(0x11, 	0x00000007);	// TPOWERDOWN=10:
	TMC5160_SPIWriteInt(0x12, 	0x0000000A);	// TPOWERDOWN=10: 	
	TMC5160_SPIWriteInt(0x13, 	Convert_usteps_To_ustept(1500*SysConfig.MotorValveConfig.MicroStep));
	//TMC5160_SPIWriteInt(0x13, 	0);
	//TMC5160_SPIWriteInt(0x14, 	0x00004189); 		// writing value 0x00004189 = 16777 = 0.0 to address 11 = 0x14(TCOOLTHRS)
	TMC5160_SPIWriteInt(0x15, 	0x00000000); 		// writing value 0x00000000 = 0 = 0.0 to address 12 = 0x15(THIGH)

	TMC5160_SPIWriteInt(0x00, 	0x00000002);	//faststandstill，0x00000002
	
	TMC5160_SPIWriteInt(0x20, 	0);						
	TMC5160_SPIWriteInt(0x21, 	0); 		// writing value 0xFFCC12F0 = 0 = 0.0 to address 14 = 0x21(XACTUAL)
	TMC5160_SPIWriteInt(0x23, 	500);					//VSTART
	TMC5160_SPIWriteInt(0x24, 	ConvertACC_ustepss_To_usteptt(SysConfig.MotorValveConfig.Acceleration));				//A1 VSTART-V1 速度模式不用配置
	TMC5160_SPIWriteInt(0x25, 	(SysConfig.MotorValveConfig.MicroStep*100));				//第一阶段加减速阀值
	TMC5160_SPIWriteInt(0x26, 	ConvertACC_ustepss_To_usteptt(SysConfig.MotorValveConfig.Acceleration));					//AMAX V1-VMAX  500
	TMC5160_SPIWriteInt(0x27, 	Convert_usteps_To_ustept(SysConfig.MotorValveConfig.MaxSpeed));						//VMAX   200*ustep*RPM/60
	TMC5160_SPIWriteInt(0x28, 	ConvertACC_ustepss_To_usteptt(SysConfig.MotorValveConfig.Deceleration));					//DMAX VMAX-V1 700
	TMC5160_SPIWriteInt(0x2A, 	ConvertACC_ustepss_To_usteptt(SysConfig.MotorValveConfig.Deceleration));					//D1 V1-VSTOP速度模式不用配置
	TMC5160_SPIWriteInt(0x2B, 	Convert_usteps_To_ustept(SysConfig.MotorValveConfig.MinSpeed));					//VSTOP
	//TMC5160_SPIWriteInt(0x2B, 	Convert_usteps_To_ustept(SysConfig.MotorValveConfig.MaxSpeed));
	
	
	TMC5160_SPIWriteInt(0x33, 	0x00000000); 		// writing value 0x00000000 = 0 = 0.0 to address 25 = 0x33(VDCMIN)
	TMC5160_SPIWriteInt(0x34, 	0x00000400); 		// writing value 0x00000400 = 1024 = 0.0 to address 26 = 0x34(SW_MODE)
	
	TMC5160_SPIWriteInt(0x60, 	0xAAAAB554); 		// writing value 0xAAAAB554 = 0 = 0.0 to address 31 = 0x60(MSLUT[0])
	TMC5160_SPIWriteInt(0x61, 	0x4A9554AA); 		// writing value 0x4A9554AA = 1251300522 = 0.0 to address 32 = 0x61(MSLUT[1])
	TMC5160_SPIWriteInt(0x62, 	0x24492929); 		// writing value 0x24492929 = 608774441 = 0.0 to address 33 = 0x62(MSLUT[2])
	TMC5160_SPIWriteInt(0x63, 	0x10104222); 		// writing value 0x10104222 = 269500962 = 0.0 to address 34 = 0x63(MSLUT[3])
	TMC5160_SPIWriteInt(0x64, 	0xFBFFFFFF); 		// writing value 0xFBFFFFFF = 0 = 0.0 to address 35 = 0x64(MSLUT[4])
	TMC5160_SPIWriteInt(0x65, 	0xB5BB777D); 		// writing value 0xB5BB777D = 0 = 0.0 to address 36 = 0x65(MSLUT[5])
	TMC5160_SPIWriteInt(0x66, 	0x49295556); 		// writing value 0x49295556 = 1227445590 = 0.0 to address 37 = 0x66(MSLUT[6])
	TMC5160_SPIWriteInt(0x67, 	0x00404222); 		// writing value 0x00404222 = 4211234 = 0.0 to address 38 = 0x67(MSLUT[7])
	TMC5160_SPIWriteInt(0x68, 	0xFFFF8056); 		// writing value 0xFFFF8056 = 0 = 0.0 to address 39 = 0x68(MSLUTSEL)
	TMC5160_SPIWriteInt(0x69, 	0x00F70000); 		// writing value 0x00F70000 = 16187392 = 0.0 to address 40 = 0x69(MSLUTSTART)
   
  TMC5160_SPIWriteInt(0x6C, 	0x10410153); 		// writing value 0x00410153 = 4260179 = 0.0 to address 41 = 0x6C(CHOPCONF)
	TMC5160_SPIWriteInt(0x6D, 	0x00030000); 		// 电流下降速率0,最小电流: 1/2 电流设置(IRUN)
	TMC5160_SPIWriteInt(0x6E, 	0x00000000); 		// writing value 0x00000000 = 0 = 0.0 to address 43 = 0x6E(DCCTRL)
	//TMC5160_SPIWriteInt(0x70, 	0xC40C001E); 		// writing value 0xC40C001E = 0 = 0.0 to address 44 = 0x70(PWMCONF) 
	TMC5160_SPIWriteInt(0x70, 	0xC40C2f1f);
	
	Motor_SetMicrostep(SysConfig.MotorValveConfig.MicroStep);
	Motor_SetPhaseCurrent(SysConfig.MotorValveConfig.PhaseCurrent);
	Motor_SetDirection(SysConfig.MotorValveConfig.PositionDir);
#if TMC5160_STOP_MODE
	TMC5160_SPIWriteInt(0x34,0x00000082);			//REFR高电平停止电机, 朝着有效电平跳变时位置锁存
	TMC_EXT_STOP();
#endif 
}
/*************************************************************************************************/
/*
↑以上区域是对TMC5160寄存器进行读写的基本操作函数↑
	↓以下区域是直接对步进电机进行控制的接口函数↓
*/
/*************************************************************************************************/

/*
函数功能：硬停电机
输入：无
输出：硬停电机
返回值：无
*/
void Motor_HardStop(void)
{	
	TMC_DRV_OFF();
	
#if TMC5160_STOP_MODE
	TMC_EXT_STOP();
#else
	TMC5160_SPIWriteInt(0x34,0x00000400);
#endif
	TMC5160_SPIWriteInt(0x27,0);
}
/*
函数功能：软停电机
输入：无
输出：软停电机
返回值：无
*/
void Motor_SoftStop(void)
{
	TMC5160_SPIWriteInt(0x27,0);
#if TMC5160_STOP_MODE
	TMC_EXT_STOP();
#else	
	TMC5160_SPIWriteInt(0x34,0x00000400);	
#endif
}
/*
速度模式运行电机
输入：speed(microstep/s)
输出：无
*/

void Motor_SpeedMove(uint8_t Dir,uint32_t uSpeed)
{
	TMC_DRV_ON();
#if TMC5160_STOP_MODE
	TMC_EXT_RELEASE();
#endif
	if(Dir==0)
		TMC5160_SPIWriteInt(0x20,1);
	else
		TMC5160_SPIWriteInt(0x20,2);
	Motor_SetVMAX(uSpeed);
}
/*
位置模式运行电机
输入：pos(microstep/s)
输出：无
*/

void Motor_MovePosition(int32_t pos)
{	
	TMC_DRV_ON();
#if TMC5160_STOP_MODE
	TMC_EXT_RELEASE();
#endif
	TMC5160_SPIWriteInt(0x20,0);		//位置模式	
	TMC5160_SPIWriteInt(0x2D,pos);	//往指定方向运动到指定位置
}
/*
函数功能：设置电机方向
输入：	0：正向
			1：反向
输出:无
返回值：无
*/
void Motor_SetDirection(uint8_t dir)
{
	uint32_t val;

	val = TMC5160_ReadData(0);
	val &= ~(0x01<<4);
	if(dir)
		val |= (1<<4);
	TMC5160_SPIWriteInt(0,val);
}
/*
函数功能：清除错误
输入：无
输出:无
返回值：无
*/
void Clear_Error(void)
{
	//Motor_Stall_Detection();
}
/********************************************************************************************/
/*
↑以上区域是对电机的运动控制函数↑
		↓以下区域是参数设置函数↓
*/
/********************************************************************************************/
/*
函数功能：设置细分
输入：细分参数
输出:无
返回值：无
*/
void Motor_SetMicrostep(uint16_t step)
{
	uint8_t val;
	uint32_t micro;
	switch(step)
	{
		case 1:	  val = 8; break;
		case 2:   val = 7; break;
		case 4:   val = 6; break;
		case 8:   val = 5; break;
		case 16:  val = 4; break;
		case 32:  val = 3; break;
		case 64:  val = 2; break;
		case 128: val = 1; break;
		case 256: val = 0; break;
	}
	micro = TMC5160_ReadData(0x6C);
	micro &= ~(0x0f<<24);
	micro |= (val<<24);

	TMC5160_SPIWriteInt(0x6C,micro);
}

/*
函数功能：设置运行阶段电流
函数说明：最大电流=((0.325/0.22)/根号2)*((CS+1)/32)=1.1A/2=550mA
					默认CS=15
					
输入：电流参数(mA)
输出:无
返回值：无
*/
void Motor_SetPhaseCurrent(uint16_t current)
{
	//设置GLOBALSCALER微调电流
	TMC5160_SPIWriteInt(0x0B,(current*255)/2350);
}


/*
函数功能：电机起动速
输入：速度(ustep/s)
输出:无
返回值：无
*/
void Motor_SetVSTART(uint32_t uspeed)
{
	TMC5160_SPIWriteInt(0x23,uspeed);
}
/*
函数功能：设置加速度1阶段
输入：速度(ustep/s)
输出:无
返回值：无
*/
void Motor_SetA1(uint32_t uspeed)
{
	TMC5160_SPIWriteInt(0x24,ConvertACC_ustepss_To_usteptt(uspeed));
}
/*
函数功能：设置加减速阶段1速度阀值
输入：速度(ustep/s)
输出:无
返回值：无
*/
void Motor_SetV1(uint32_t uspeed)
{
	TMC5160_SPIWriteInt(0x25,uspeed);
}
/*
函数功能：设置加速度2阶段
输入：速度(ustep/s)
输出:无
返回值：无
*/
void Motor_SetAMAX(uint32_t uspeed)
{
	TMC5160_SPIWriteInt(0x26,ConvertACC_ustepss_To_usteptt(uspeed));
}
/*
函数功能：设置目标速度
输入：速度(ustep/s)
输出:无
返回值：无
*/
void Motor_SetVMAX(uint32_t uspeed)
{
	TMC5160_SPIWriteInt(0x27,Convert_usteps_To_ustept(uspeed));
}
/*
函数功能：VMAX和V1之间的减速度
输入：速度(ustep/s)
输出:无
返回值：无
*/
void Motor_SetDMAX(uint32_t uspeed)
{
	TMC5160_SPIWriteInt(0x28,ConvertACC_ustepss_To_usteptt(uspeed));
}
/*
函数功能：V1和VSTOP之间的减速
输入：速度(ustep/s)
输出:无
返回值：无
*/
void Motor_SetD1(uint32_t uspeed)
{
	TMC5160_SPIWriteInt(0x2A,ConvertACC_ustepss_To_usteptt(uspeed));
}
/*
函数功能：电机停止速
输入：速度(ustep/s)
输出:无
返回值：无
*/
void Motor_SetVSTOP(uint32_t uspeed)
{
	TMC5160_SPIWriteInt(0x2B,uspeed);
}

/*
函数功能：获取当前位置
输入：无
输出:无
返回值：当前位置
*/
int32_t Get_MotorPosition(void)
{
	int32_t Position;
	Position = TMC5160_ReadData(0x21);
	//因有时读出位置为0，需避免读出错误
	if(Position==0)
		Position = TMC5160_ReadData(0x21);
	return Position;
}
/*
函数功能：设置当前位置
输入：当前的位置
输出:无
返回值：
*/
void Set_MotorPosition(int32_t position)
{
	Motor_SetVMAX(0);
	TMC5160_SPIWriteInt(0x20,1);		//速度模式
	TMC5160_SPIWriteInt(0x21,position);
}
/*
函数功能：获取当前速度
输入：无
输出:无
返回值：当前位置
*/
int32_t Get_MotorSpeed(void)
{
	int32_t speed;
	
	speed=TMC5160_ReadData(0x22);
	speed=speed<<8;
	speed/=256;
	return speed;
}

/*
函数功能：查询斜率和开关状态
输入：无
输出:无
返回值：运行状态
*/
//uint32_t Plunger_Get_RAMP_Status(void)
//{
//	return Plunger_TMC5160_ReadData(0x35);
//}
//uint32_t Valve_Get_RAMP_Status(void)
//{
//	return Plunger_TMC5160_ReadData(0x35);
//}
uint32_t Get_RAMP_Status(void)
{
	return TMC5160_ReadData(0x35);
}
/*
函数功能：查询电机状态
输入：无
输出:无
返回值：	bit31:0运行，1停止
				bit30:A相开路
				bit29:B相开路
				bit28:B相短路
				bit27:A相短路
				bit26:过温预警
				bit25:过温指示
				bit24:电机堵转
				bit13:B相相对电源短路
				bit12:A相相对电源短路
*/
uint32_t Get_Motor_Status(void)
{
	return TMC5160_ReadData(0x6F);
}

/*
函数功能：清除到达目标位置中断
输入：细分参数
输出:无
返回值：无
*/
void Motor_ClearReachPosition_Status(void)
{
	TMC5160_SPIWriteInt(0x35,0x00000080);
}