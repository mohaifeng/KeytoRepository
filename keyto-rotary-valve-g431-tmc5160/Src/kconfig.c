#include "kconfig.h"
#include "string.h"

SysRunCont_type  SysRunCont;

extern SysConfig_type  *pSysConfig;
static uint32_t  fac_us=0;

void Set_None_ReductionBox_PortAndOptPosition(uint8_t num)
{
	uint8_t i;
	
	pSysConfig->ValveConfig.PortNum = num;
	pSysConfig->ValveConfig.OptGapNum = num;
	pSysConfig->MotorValveConfig.MaxSpeed=100*pSysConfig->MotorValveConfig.MicroStep; 
	pSysConfig->MotorValveConfig.RunSpeed=100*pSysConfig->MotorValveConfig.MicroStep;
	pSysConfig->MotorValveConfig.Acceleration = 2000*pSysConfig->MotorValveConfig.MicroStep;  	
	pSysConfig->MotorValveConfig.Deceleration = 2000*pSysConfig->MotorValveConfig.MicroStep;
	pSysConfig->ValveConfig.FWD_ValveOptocouplerGapStep=2*pSysConfig->MotorValveConfig.MicroStep;
	pSysConfig->ValveConfig.OptInterval=(pSysConfig->ValveConfig.MaxStep/pSysConfig->ValveConfig.OptGapNum)*pSysConfig->MotorValveConfig.MicroStep;
	pSysConfig->MotorValveConfig.PositionDir=1;

	pSysConfig->ValveConfig.ResetPort=1;
	for(i=0;i<pSysConfig->ValveConfig.OptGapNum;i++)
		pSysConfig->ValveConfig.OptPosition[i]=((i*pSysConfig->ValveConfig.MaxStep)/pSysConfig->ValveConfig.OptGapNum+((pSysConfig->ValveConfig.MaxStep/pSysConfig->ValveConfig.OptGapNum)/2))*pSysConfig->MotorValveConfig.MicroStep;
	
	for(i=0;i<pSysConfig->ValveConfig.PortNum;i++)
	{		
		pSysConfig->ValveConfig.Port[i].MotorPosition=(i*pSysConfig->ValveConfig.MaxStep)/pSysConfig->ValveConfig.PortNum*pSysConfig->MotorValveConfig.MicroStep;	
		pSysConfig->ValveConfig.Port[i].Port = i+1;
	}
}

void Set_Multi_ReductionBox_PortAndOptPosition(uint8_t num)
{
	uint8_t i;
	
	if((pSysConfig->ValveConfig.Reduction_Ratio == 10) && (num == 4))
	{
		pSysConfig->ValveConfig.PortNum = num;
		pSysConfig->ValveConfig.OptGapNum = 16;
		pSysConfig->ValveConfig.ResetPort=1;
		
		pSysConfig->ValveConfig.OptInterval=(pSysConfig->ValveConfig.MaxStep/pSysConfig->ValveConfig.OptGapNum)*pSysConfig->MotorValveConfig.MicroStep;
		
		for(i=0;i<pSysConfig->ValveConfig.OptGapNum;i++)
		{
			pSysConfig->ValveConfig.OptPosition[i]=((i*pSysConfig->ValveConfig.MaxStep)/pSysConfig->ValveConfig.OptGapNum+(pSysConfig->ValveConfig.MaxStep/pSysConfig->ValveConfig.OptGapNum/2))*pSysConfig->MotorValveConfig.MicroStep;
		}
		pSysConfig->ValveConfig.Port[0].MotorPosition=0;	
		pSysConfig->ValveConfig.Port[0].Port = 1;
		pSysConfig->ValveConfig.Port[1].MotorPosition=500*pSysConfig->MotorValveConfig.MicroStep;	
		pSysConfig->ValveConfig.Port[1].Port = 2;
		pSysConfig->ValveConfig.Port[2].MotorPosition=1000*pSysConfig->MotorValveConfig.MicroStep;	
		pSysConfig->ValveConfig.Port[2].Port = 3;
		pSysConfig->ValveConfig.Port[3].MotorPosition=1500*pSysConfig->MotorValveConfig.MicroStep;	
		pSysConfig->ValveConfig.Port[4].Port = 4;
	}
	else
	{
		pSysConfig->ValveConfig.PortNum = num;
		pSysConfig->ValveConfig.OptGapNum = num;
		pSysConfig->ValveConfig.Reduction_Ratio = (float)pSysConfig->ValveConfig.MaxStep/200.0;
		
		if(pSysConfig->ValveConfig.Reduction_Ratio>1)
			pSysConfig->ValveConfig.Using_ReductionBox = 1;
		else
			pSysConfig->ValveConfig.Using_ReductionBox = 0;

		pSysConfig->MotorValveConfig.MaxSpeed=pSysConfig->ValveConfig.MaxStep/2*pSysConfig->MotorValveConfig.MicroStep; 
		pSysConfig->MotorValveConfig.RunSpeed=pSysConfig->ValveConfig.MaxStep/2*pSysConfig->MotorValveConfig.MicroStep; 
		pSysConfig->ValveConfig.MaxGapStep = ((uint8_t)(1.4f*pSysConfig->ValveConfig.Reduction_Ratio));
		pSysConfig->ValveConfig.FWD_ValveOptocouplerGapStep=((uint16_t)pSysConfig->ValveConfig.Reduction_Ratio)*pSysConfig->MotorValveConfig.MicroStep;
		pSysConfig->ValveConfig.OptInterval=(pSysConfig->ValveConfig.MaxStep/pSysConfig->ValveConfig.OptGapNum)*pSysConfig->MotorValveConfig.MicroStep;
		pSysConfig->MotorValveConfig.PositionDir=1;

		pSysConfig->ValveConfig.ResetPort=1;
		for(i=0;i<pSysConfig->ValveConfig.OptGapNum;i++)
			pSysConfig->ValveConfig.OptPosition[i]=((i*pSysConfig->ValveConfig.MaxStep)/pSysConfig->ValveConfig.OptGapNum+((pSysConfig->ValveConfig.MaxStep/pSysConfig->ValveConfig.OptGapNum)/2))*pSysConfig->MotorValveConfig.MicroStep;
		
		for(i=0;i<pSysConfig->ValveConfig.PortNum;i++)
		{		
			pSysConfig->ValveConfig.Port[i].MotorPosition=(i*pSysConfig->ValveConfig.MaxStep)/pSysConfig->ValveConfig.PortNum*pSysConfig->MotorValveConfig.MicroStep;	
			pSysConfig->ValveConfig.Port[i].Port = i+1;
		}
	}
	
	if(pSysConfig->ValveConfig.PortNum > 12)
	{
		pSysConfig->MotorValveConfig.MaxSpeed = pSysConfig->ValveConfig.MaxStep*pSysConfig->MotorValveConfig.MicroStep;
		pSysConfig->MotorValveConfig.MinSpeed = pSysConfig->MotorValveConfig.MaxSpeed;
	}
	else
		pSysConfig->MotorValveConfig.MaxSpeed = (pSysConfig->ValveConfig.MaxStep/2)*pSysConfig->MotorValveConfig.MicroStep;
	pSysConfig->MotorValveConfig.RunSpeed = pSysConfig->MotorValveConfig.MaxSpeed;
}

void setValueChannel(uint8_t number)
{
		Set_Multi_ReductionBox_PortAndOptPosition(number);
}		

void Param_Init(void)
{
	pSysConfig->IntoBootMode=0;
	pSysConfig->InitFlag = FLASH_INIT_FLAG;
//	strcpy(pSysConfig->FirmwareVersion,SYS_VERSION);
	pSysConfig->IIC_Addr = 0x0E;
	pSysConfig->i2c_valve_profile = 0x1A;
	pSysConfig->i2c_valvecmd = 0x01;
	#ifdef FOR_NCS_VERSION
	pSysConfig->CommunicationConfig.Add=15;
	#else
	pSysConfig->CommunicationConfig.Add=0;
	#endif
  pSysConfig->CommunicationConfig.RS232_BaudRate=0;
  pSysConfig->CommunicationConfig.RS485_BaudRate=0;
	#ifdef FOR_NCS_VERSION
  pSysConfig->CommunicationConfig.CAN_BaudRate=125; //125k
	#else
	pSysConfig->CommunicationConfig.CAN_BaudRate=500; //500k
	#endif
  pSysConfig->CommunicationConfig.CAN_TargetAdd=0;

	pSysConfig->EncoderConfig.PulseNumber=4000;
	pSysConfig->EncoderConfig.Direction=0;
	pSysConfig->EncoderConfig.EncoderEn = 0;
	
	pSysConfig->MotorValveConfig.PositionDir=1;
	pSysConfig->MotorValveConfig.MicroStep = 32;
	pSysConfig->MotorValveConfig.PhaseCurrent=1500;  
	pSysConfig->MotorValveConfig.MinSpeed = 10*pSysConfig->MotorValveConfig.MicroStep;
	pSysConfig->MotorValveConfig.MaxSpeed=1000*pSysConfig->MotorValveConfig.MicroStep; 
	pSysConfig->MotorValveConfig.Acceleration = 20000*pSysConfig->MotorValveConfig.MicroStep;  	
	pSysConfig->MotorValveConfig.Deceleration = 20000*pSysConfig->MotorValveConfig.MicroStep;
	pSysConfig->MotorValveConfig.RunSpeed=1000*pSysConfig->MotorValveConfig.MicroStep;
	pSysConfig->ValveConfig.ResetSpeed=1000;
	pSysConfig->ValveConfig.MaxGapStep = 20;
	
	pSysConfig->ValveConfig.ErrorAutoResetAllowCont=0;
  pSysConfig->ValveConfig.FWD_ValveOptocouplerGapStep=14*pSysConfig->MotorValveConfig.MicroStep;
	pSysConfig->ValveConfig.FlowClockwisePhaseCompensation=0;
  pSysConfig->ValveConfig.FlowAnticlockwisePhaseCompensation=0;
	
	pSysConfig->ValveConfig.Using_ReductionBox = 1;
	pSysConfig->ValveConfig.Reduction_Ratio = 10;
	pSysConfig->ValveConfig.MaxStep = 2000;
	
	pSysConfig->Not_Change = 0;
	
	pSysConfig->AutoResetZeroEn=1;
	pSysConfig->AutoAgingEnable = 0;
	setValueChannel(10);
	
}


void SysTick_Init(uint8_t SYSCLK)
{
 	HAL_SYSTICK_CLKSourceConfig(SYSTICK_CLKSOURCE_HCLK);//SysTick频率为HCLK 
	fac_us=SYSCLK;						//保存1us所需的计数次数  
}			

void delay_us(uint32_t nus)
{		
	uint32_t ticks;
	uint32_t told,tnow,tcnt=0;
	uint32_t reload=SysTick->LOAD;				//LOAD的值	    	 
	ticks=nus*fac_us; 						//需要的节拍数 
	told=SysTick->VAL;        				//刚进入时的计数器值
	while(1)
	{
		tnow=SysTick->VAL;	
		if(tnow!=told)
		{	    
			if(tnow<told)
				tcnt+=told-tnow;	//这里注意一下SYSTICK是一个递减的计数器就可以了.
			else 
				tcnt+=reload-tnow+told;	    
			told=tnow;
			if(tcnt>=ticks)
				break;			//时间超过/等于要延迟的时间,则退出.
		}  
	}
}

void delay_ms(uint16_t nms)
{
	uint32_t i;
	for(i=0;i<nms;i++)
		delay_us(1000);
}


uint32_t my_abs(int32_t data)
{
	if(data<0)
		return ~(data)+1;
	return data;
}


uint8_t ChangePositionBack(uint8_t p)
{
  switch(p)
  {
    case 1:return 3;
    case 2:return 4;
    case 3:return 5;
    case 4:return 6;
    case 5:return 7;
    case 6:return 8;
    case 7:return 9;
    case 8:return 10;
    case 9:return 1;
    case 10:return 2;
  }
  return 0;
}
uint8_t ChangePosition(uint8_t p)
{
  switch(p)
  {
    case 1:return 9;
    case 2:return 10;
    case 3:return 1;
    case 4:return 2;
    case 5:return 3;
    case 6:return 4;
    case 7:return 5;
    case 8:return 6;
    case 9:return 7;
    case 10:return 8;
  }
  return 0;
}

