#include "valve_control.h"
#include "tmc5160.h"
#include "stdlib.h"
#include "encoder.h"
#include "protocol.h"
//打开调试参数
#define DEBUG_VALVE	0
//24通随机通道和补偿值老化
#define CHANNEL_24_RAND_AGING	1

Valve_Handle_t ValveControl=
{	
  .bState=VALVE_IDLE,
  .CurrentMotorPosition=0,
  .CurrentPort=0,
	.hFaultNow=0,
  .TargetPositon=-1,
  .TargetPort=0xff,
  .autoZeroMode=false,
  .AgingTestMode=false,
  .ZeroMarkFlag=false,
	.MoveFinishCallBackExe=0,
	.ErrorAutoResetCont=0,
	.ErrorAutoResetFlag=0,
	.ErrorResetChannel=0,
};



static uint32_t SysStatus=0;

uint32_t ValveFaultTimeCont=0;
uint8_t ValveFaultLedFlashingTimes=0;
uint8_t ValveFlashingTimes=0;
uint8_t PortInitFlag = 0;
extern uint32_t ValveStartCont;
extern uint32_t ValvePositionOptTriggerFlagCont;
extern uint32_t ValvePositionOptTriggerTimeCont;
extern uint32_t ValveZeroOptTriggerFlagCont;
extern uint32_t ValveControlTaskCont;
extern SysConfig_type  *pSysConfig;
#if DEBUG_VALVE
typedef struct
{
	int32_t StartPosition;
	int32_t MoveStep;
	uint8_t oldPort;
	uint8_t TargetPort;
	int32_t TargetPositon;
	uint8_t CurrentOptCont;
	uint8_t CurrentOpt2Cont;
	uint8_t StartOptTriggerFlag;
	uint8_t TargetOptCont;
	uint8_t InvalFlag;
	uint8_t Dir;
	int32_t TriggerMotorPosition[25];
} Valve_Move_t;
Valve_Move_t Valve_Move[50];
uint8_t ValvePtr=0;
#endif
/**********************AGING**********************
*************************************************/
uint8_t Aging_Mode = 0;
uint8_t Aging_Next = 1;
uint8_t Aging_Channel = 1;
uint8_t Rank_A = 0;
uint8_t Rank_B = 0;
uint8_t Rank_C = 0;
uint8_t RankB_Buf[11] = {24,1,2,2,24,2,1,21,1,6,1};
uint8_t Rank_Cnt = 0;
uint16_t Rank_Count = 0;
uint16_t Delay_Time = 0;
uint8_t CmdMode=0;
uint8_t AddDir=0;
void Aging_RankA(void)
{
	switch(Rank_A)
	{
		case 0:
			if(ValveControl.bState == VALVE_IDLE)
			{
				Rank_A = 1;
			}
		break;
		case 1:
//			CmdMode=rand()%3;
//			if(CmdMode==1)
//				Valve_SetPortDir(0,Aging_Channel);
//			else if(CmdMode==2)
//				Valve_SetPortDir(1,Aging_Channel);
//			else
				Valve_SetPort(Aging_Channel);
			if(AddDir==0)
				Aging_Channel++;
			else
			{
				if(Aging_Channel==1)
					Aging_Channel=24;
				else
					Aging_Channel--;
			}
			Rank_A = 2;
		break;
		case 2:
			Delay_Time++;
			if(Delay_Time>=(50/VALVE_CONTROL_TASK_TIME))
			{
				Delay_Time = 0;
				if(Aging_Channel>24&&AddDir==0)
					Rank_A = 3;
				else if(Aging_Channel==1&&AddDir==1)
					Rank_A = 3;
				else
					Rank_A = 0;
			}
		break;
		case 3:
			Aging_Channel = 1;
			Aging_Mode++;
			if(AddDir==0)
			{
				AddDir=1;
			}
			else
			{
				AddDir=0;
			}
			Rank_A = 0;
		break;
		
	}
}

void Aging_RankB(void)
{
	switch(1)
	{
		case 0:
			if(ValveControl.bState == VALVE_IDLE)
			{
				Rank_B = 1;
			}
		break;
		case 1:
			Aging_Channel = (rand()%SysConfig.ValveConfig.PortNum)+1;
			
			CmdMode=rand()%3;
			if(CmdMode==1)
				Valve_SetPortDir(0,Aging_Channel);
			else if(CmdMode==2)
				Valve_SetPortDir(1,Aging_Channel);
			else
				Valve_SetPort(Aging_Channel);
			Rank_B = 0;
		break;
		case 2:
			Delay_Time++;
			if(Delay_Time>=(5000/VALVE_CONTROL_TASK_TIME))
			{
				Delay_Time = 0;
				Rank_B = 3;
			}
		break;
		case 3:
			Rank_Cnt = 0;
			Rank_Count++;
//			if(Rank_Count>10)
//				Rank_B = 4;
//			else
				Rank_B = 0;
		break;
		case 4:
			Rank_Count = 0;
			Rank_B = 0;
			Aging_Mode++;
			Aging_Channel = 1;
		break;
	}
}
void Aging_RankC(void)
{
	switch(Rank_C)
	{
		case 0:
			if(ValveControl.bState == VALVE_IDLE)
			{
				Rank_C = 1;
			}
		break;
		case 1:
			Aging_Channel = RankB_Buf[Rank_Cnt];
			CmdMode=rand()%3;
			if(CmdMode==1)
				Valve_SetPortDir(0,Aging_Channel);
			else if(CmdMode==2)
				Valve_SetPortDir(1,Aging_Channel);
			else
				Valve_SetPort(Aging_Channel);
			Rank_C = 2;
		break;
		case 2:
			Delay_Time++;
			if(Delay_Time>=(100/VALVE_CONTROL_TASK_TIME))
			{
				Delay_Time = 0;
				Rank_Cnt++;
				if(Rank_Cnt >= 11)
					Rank_C = 3;
				else
					Rank_C = 0;
			}
		break;
		case 3:
			Rank_C = 0;
			Rank_Cnt = 0;
			Rank_Count++;
			if(Rank_Count>3)
				Rank_C = 4;
			else
				Rank_C = 0;
		break;
		case 4:
			Rank_Count = 0;
			Rank_C = 0;
			Aging_Mode++;
			Aging_Channel = 1;
		break;
	}
}

void Aging(void)
{
	Aging_RankB();
#if CHANNEL_24_RAND_AGING == 0
	if(SysConfig.ValveConfig.PortNum==24)
	{
		//TickDelay(50);
		Aging_Channel++;
		if(Aging_Channel>24)
			Aging_Channel=1;
		Valve_SetPort(Aging_Channel);
		return ;	
	}
#endif
//	switch(Aging_Mode)
//	{
//		case 0:
//			Aging_RankA();
//		break;
//		case 1:
//			Aging_RankB();
//		break;
//		case 2:
//			Aging_RankC();
//		break;
//		case 3:
//			Delay_Time++;
//			if(Delay_Time>=(2000/VALVE_CONTROL_TASK_TIME))
//				Aging_Mode++;
//		break;
//		case 4:
//			Aging_Mode = 0;
//#if CHANNEL_24_RAND_AGING == 1
//		//设置随机补偿值
//		if(SysConfig.ValveConfig.PortNum==24)
//		{
//			pSysConfig->ValveConfig.FlowClockwisePhaseCompensation=(int)(rand()%1280)-640;
//			pSysConfig->ValveConfig.FlowAnticlockwisePhaseCompensation=(int)(rand()%1280)-640;	
//		}
//#endif
//		break;
//	}
}

/*************************************************
设置阀下一个运行状态
*************************************************/
void Valve_NextState( ValveState_t bState )
{
  if(ValveControl.ZeroMarkFlag!=1&&(bState==VALVE_START||bState==VALVE_DIR_START||bState==VALVE_RUN))
    return ;
  ValveControl.bState = bState;
	ValveControlTaskCont=VALVE_CONTROL_TASK_TIME;
}
uint8_t Valve_GetZeroMark(void)
{
	return ValveControl.ZeroMarkFlag;
};
/*************************************************
函数功能：获取阀当前错误
*************************************************/
uint8_t Valve_GetFaultFlag(void)
{
	return ValveControl.hFaultNow;
}
void Value_ClearFault(void)
{
	ValveControl.hFaultNow=0;
	LED_OFF();
	Valve_NextState(VALVE_IDLE);
}

/***********************
函数功能：通过软件延时，单位us
函数说明：
************************/
void OptDelay(uint32_t delay_time)
{
	delay_time=delay_time*5;
	while(delay_time--)
		__NOP();
}

/**********获取当前位置*************/
int32_t Get_CurrentPosition(void)
{
	int32_t CurrentPos;
	if(SysConfig.WorkMode == 1)
	{
		#if (USED_ENCODER == 1)
		CurrentPos=Encoder_GetPositionCont();
		#endif
	}
	else
	{	
		CurrentPos=Get_MotorPosition();		
	}
	return CurrentPos;
}
uint8_t Valve_GetFaultStatus(void)
{
	return (uint8_t)ValveControl.bState;
}
/*************************************************
函数功能：设置阀当前错误
函数说明：
hFaultNow:
3:置零光耦位置检测位置光耦计数错误
4：位置光耦检测移动步数错误
*************************************************/
void Valve_SetFaultFlag(uint8_t error)
{
	if(Valve_ErrorAutoReset()==0)
	{
		Valve_ImmediatelyStopMotor();
		ValveControl.hFaultNow=error;
		Valve_NextState(VALVE_FAULT);
	}
}
/*************************************************
获取阀当前运行状态
*************************************************/
uint16_t Valve_GetStatus( void )
{
	return ValveControl.RunStatus;
}

void Valve_Delay(uint32_t d)
{
  while(d--);
}
/********************************************************************/

void Valve_AutoZeroMode(void)
{
	Valve_NextState(VALVE_AUTO_ZERO);
}

uint8_t Valve_GetCurrent_Channel(void)
{
	if(SysConfig.ValveConfig.PortNum == 10 && !SysConfig.Not_Change)
		return ChangePositionBack(ValveControl.CurrentPort);
	else
		return ValveControl.CurrentPort;
}



//计算运行过程中光耦需要触发的个数
uint8_t Valve_GetOptTriggerCont(void)
{
	int i,OptPosition;
	uint8_t InversionFlag=ValveControl.OneOptTrigeFlagNoCont;
	uint8_t CurrentOptCont=ValveControl.CurrentOptocoupler2Cont;
	ValveControl.TargetTriggerOptCont=0;
	for(i=0;i<pSysConfig->ValveConfig.OptGapNum;i++)
	{
		ValveControl.TargetTriggerOptCont++;
		if(InversionFlag&&CurrentOptCont!=0)
		{
			InversionFlag=0;
		}
		else
		{
			InversionFlag=0;
			if(ValveControl.MoveDirection==0)
			{
				
				if(CurrentOptCont>=pSysConfig->ValveConfig.OptGapNum)
					CurrentOptCont=1;
				else
					CurrentOptCont++;
			}
			else
			{
				
				if(CurrentOptCont<=1)
					CurrentOptCont=pSysConfig->ValveConfig.OptGapNum;
				else
					CurrentOptCont--;
			}
		}
		OptPosition=pSysConfig->ValveConfig.OptPosition[CurrentOptCont-1];
		if((CurrentOptCont==pSysConfig->ValveConfig.OptGapNum)&&\
		(ValveControl.TargetPositon<pSysConfig->ValveConfig.OptInterval/2)&&\
		(ValveControl.TargetPositon>=0))
		{
			OptPosition=OptPosition-SysConfig.ValveConfig.MaxStep*pSysConfig->MotorValveConfig.MicroStep;
		}
		//避免反向补偿值为负数时，切换1通道出现切换到最大通道问题
		else if(CurrentOptCont==1&&\
			ValveControl.TargetPositon>(SysConfig.ValveConfig.MaxStep*pSysConfig->MotorValveConfig.MicroStep-pSysConfig->ValveConfig.OptInterval))
		{
			OptPosition=OptPosition+SysConfig.ValveConfig.MaxStep*pSysConfig->MotorValveConfig.MicroStep;
		}
		if(abs(OptPosition-ValveControl.TargetPositon)<pSysConfig->ValveConfig.OptInterval)
		{
			break;
		}
	}
	return ValveControl.TargetTriggerOptCont;
}
/***********************************************
函数说明：安全检查任务
					检查出错ValveControl.RunStatus赋值
					bit0：是否忙
					bit1: 是否未初始化
					bit8-bit15：柱塞泵故障
						1:置零错误
						2:堵转
检测到故障后通过LED闪烁指示
************************************************/
void Valve_SafetyTask(void)
{
	uint32_t Status=0;
	if(ValveControl.bState!=VALVE_IDLE&&ValveControl.bState!=VALVE_FAULT_IDLE)
		Status|=0x01;
	if(ValveControl.ZeroMarkFlag==0)
		Status|=0x02;
	//故障检测
	if(ValveControl.bState==VALVE_AUTO_ZERO_RUN)
  {
    if(ValvePositionOptTriggerFlagCont>3000||
			(ValveZeroOptTriggerFlagCont>(pSysConfig->ValveConfig.MaxStep/pSysConfig->ValveConfig.ResetSpeed)*1000+3000))    //光耦错误检查
       Valve_SetFaultFlag(1);//上电光耦错误
  }
  else if(ValveControl.bState==VALVE_RUN)
  {
    if(ValvePositionOptTriggerFlagCont>2000)    //堵转检测 
       Valve_SetFaultFlag(2);//堵转错误
  }
	if(ValveControl.bState!=VALVE_IDLE)
	{
		ValveControl.MotorHardStatus=Get_Motor_Status();
//		//出现错误再次读取避免读错
//		if(STM.MotorHardStatus&0x7f003000)
//		{
//			STM.MotorHardStatus=Get_Motor_Status();
//		}
	}
	//需添加错误检测代码
	if(ValveControl.hFaultNow)
		Status|=((uint32_t )ValveControl.hFaultNow)<<8;
	SysStatus=Status;
	if(SysStatus!=ValveControl.RunStatus)
	{
		ValveControl.RunStatus=SysStatus;		
	}
}
uint16_t WaitCnt = 0;
int8_t currentport;
int8_t targetport;
void Valve_ControlTask(void)
{
	int32_t postion,speed;
  Valve_SafetyTask();
  switch((uint16_t)ValveControl.bState)
  {
    case VALVE_IDLE:
			if(SysConfig.AutoAgingEnable)
			{
				WaitCnt++;
				if(WaitCnt>=5000/VALVE_CONTROL_TASK_TIME)
				{
					WaitCnt = 0;
					Aging();
				}
			}
    break;
    case VALVE_AUTO_ZERO:
			LED_ON();
			ValveControl.hFaultNow=0;
			ValveControl.CurrentPort=0;
			ValvePositionOptTriggerFlagCont=0;
			ValveZeroOptTriggerFlagCont=0;
			ValveControl.TriggerOptFlag=0;
      ValveControl.ZeroMarkFlag=false;
      ValveControl.autoZeroMode=true;
			ValveControl.TargetPort=0;
      ValveControl.TargetPositon=0;
      ValveControl.MoveDirection=0;
//			if(SysConfig.ValveConfig.Using_ReductionBox)
				Valve_SpeedMove(pSysConfig->ValveConfig.ResetSpeed*pSysConfig->MotorValveConfig.MicroStep);// 速度模式开始找光耦零位
			////延时一定时间后开始检测触发，否则容易出现误触发
//			HAL_NVIC_DisableIRQ(EXTI0_IRQn);
//			TickDelay(100);
//			HAL_NVIC_EnableIRQ(EXTI0_IRQn);
      Valve_NextState(VALVE_AUTO_ZERO_RUN);
      ValveControl.OneOptTrigeFlagNoCont=0;
      ValveControl.OldMoveDirection=ValveControl.MoveDirection;
			ValveControl.PositionMoveFlag=0;
			ValveControl.ResetTimeCont=25;//125ms缓冲期
      break;
    case VALVE_AUTO_ZERO_RUN:
      //置零光耦处理中停止电机
			//等待运动到位
			if(ValveControl.ZeroMarkFlag==true)
			{
				ValveControl.ResetTimeCont--;
				if(ValveControl.ResetTimeCont==0)
				{
					Set_MotorPosition(0);						//位置清零
					if(ValveControl.ErrorAutoResetFlag)
					{
						ValveControl.ErrorAutoResetFlag=0;
						Valve_SetPort(ValveControl.ErrorResetChannel);
					}
					else
					{
							ValveControl.CurrentPort=1;
							ValveControl.TargetPort = 1;
							if(SysConfig.ValveConfig.PortNum!=10)
							{
								if(SysConfig.ValveConfig.FlowAnticlockwisePhaseCompensation||SysConfig.ValveConfig.FlowClockwisePhaseCompensation)
								{
									Valve_SetPort(SysConfig.ValveConfig.PortNum);
									PortInitFlag = 1;
								}
								else
								{
									Valve_SetPort(SysConfig.ValveConfig.ResetPort);
								}
							}
							else
							{
								Valve_SetPort(SysConfig.ValveConfig.ResetPort);
							}
					}
				}
			}
			break;
    case VALVE_START:
			LED_ON();
			ValveStartCont=0;
			ValvePositionOptTriggerFlagCont=0;
			ValveZeroOptTriggerFlagCont=0;
			ValveControl.TriggerOptFlag=0;
			//Motor_SetVMAX(SysConfig.MotorValveConfig.MaxSpeed,MOTOR_VALVE);   
			//获取当前位置和目标位置进行对比
			postion = Get_MotorPosition();
			ValveControl.StartPosition=postion;
			if(postion<0)
				postion=SysConfig.ValveConfig.MaxStep*pSysConfig->MotorValveConfig.MicroStep+postion;

			
			if(SysConfig.ValveConfig.PortNum == 10)
			{
				if(!SysConfig.Not_Change)//旧版10通阀头
				{
					currentport = ChangePositionBack(ValveControl.CurrentPort);
					targetport = ChangePositionBack(ValveControl.TargetPort);
				}
				else
				{
					currentport = ValveControl.CurrentPort;
					targetport = ValveControl.TargetPort;
				}
			}
			else
			{
				currentport = ValveControl.CurrentPort;
				targetport = ValveControl.TargetPort;
			}

			if(abs(targetport-currentport)<(SysConfig.ValveConfig.PortNum/2))
			{
				if(targetport>currentport)
				{
					ValveControl.MoveDirection=0;
					speed=SysConfig.MotorValveConfig.RunSpeed;
				}
				else
				{
					ValveControl.MoveDirection=1;
					speed=0-SysConfig.MotorValveConfig.RunSpeed;
				}
			}
			else if(abs(targetport-currentport)>(SysConfig.ValveConfig.PortNum/2))
			{
				if(targetport<currentport)
				{
					ValveControl.MoveDirection=0;
					speed=SysConfig.MotorValveConfig.RunSpeed;
				}
				else
				{
					ValveControl.MoveDirection=1;
					speed=0-SysConfig.MotorValveConfig.RunSpeed;
				}
			}
			else//对角切换
			{
				if(targetport>currentport)
				{
					ValveControl.MoveDirection=0;
					speed=SysConfig.MotorValveConfig.RunSpeed;
				}
				else
				{
					ValveControl.MoveDirection=1;
					speed=0-SysConfig.MotorValveConfig.RunSpeed;
				}
			}
			ValveControl.Move_uStep=(pSysConfig->ValveConfig.FWD_ValveOptocouplerGapStep+(SysConfig.ValveConfig.MaxStep*pSysConfig->MotorValveConfig.MicroStep)/pSysConfig->ValveConfig.OptGapNum)/2;			
			if(ValveControl.MoveDirection==0)
			{
				ValveControl.Move_uStep+=pSysConfig->ValveConfig.FlowAnticlockwisePhaseCompensation;
				ValveControl.TargetPositon+=pSysConfig->ValveConfig.FlowAnticlockwisePhaseCompensation;
				if(pSysConfig->ValveConfig.Using_ReductionBox)
				{
					if(ValveControl.TargetPositon<0)
						ValveControl.TargetPositon=SysConfig.ValveConfig.MaxStep*pSysConfig->MotorValveConfig.MicroStep+ValveControl.TargetPositon;
				}
				else
				{
					if(ValveControl.TargetPositon<=0)//防止目标地址是0时，切换错误
						ValveControl.TargetPositon=pSysConfig->ValveConfig.MaxStep*pSysConfig->MotorValveConfig.MicroStep+ValveControl.TargetPositon;
				}
			}
			else
			{
				ValveControl.Move_uStep+=pSysConfig->ValveConfig.FlowClockwisePhaseCompensation;
				ValveControl.TargetPositon+=pSysConfig->ValveConfig.FlowClockwisePhaseCompensation;
				if(ValveControl.TargetPositon<0)
					ValveControl.TargetPositon=SysConfig.ValveConfig.MaxStep*pSysConfig->MotorValveConfig.MicroStep+ValveControl.TargetPositon;
			}
			ValveControl.PositionMoveFlag=0;
			ValveControl.StartOptTriggerFlag=ValveGet_Hole2OptInputState();
			if(ValveControl.OldMoveDirection!=ValveControl.MoveDirection)
			{
				//检测当前光耦是否为触发状态,如果是触发状态不用忽略第一个光耦
				if(ValveControl.StartOptTriggerFlag==0&&ValveControl.CurrentOptocoupler2Cont!=0)
				//if(ValveControl.CurrentOptocoupler2Cont!=0)
				{
					ValveControl.OneOptTrigeFlagNoCont=1;
				}
				ValveControl.OldMoveDirection=ValveControl.MoveDirection;
			}
			Valve_GetOptTriggerCont();
			ValveControl.CurrentTriggerOptCont=0;
			ValveControl.StartBeforeOptocoupler2Cont=ValveControl.CurrentOptocoupler2Cont;
			Valve_NextState(VALVE_RUN);
			Valve_SpeedMove(speed);// 速度模式开始运动到指定通道
			Valve_AddRunTimes();
			ValveControl.CurrentTriggerOptCont=0;
			
			#if DEBUG_VALVE
			memcpy(Valve_Move[ValvePtr].TriggerMotorPosition,ValveControl.TriggerMotroPosition,sizeof(ValveControl.TriggerMotroPosition));
			ValvePtr++;
			if(ValvePtr==50)
				ValvePtr=0;
			Valve_Move[ValvePtr].StartOptTriggerFlag=ValveControl.StartOptTriggerFlag;
			Valve_Move[ValvePtr].oldPort=ValveControl.CurrentPort;
			Valve_Move[ValvePtr].TargetPort=ValveControl.TargetPort;
			Valve_Move[ValvePtr].Dir=ValveControl.MoveDirection;
			Valve_Move[ValvePtr].CurrentOpt2Cont=ValveControl.CurrentOptocoupler2Cont;
			Valve_Move[ValvePtr].TargetOptCont=ValveControl.TargetTriggerOptCont;
			Valve_Move[ValvePtr].TargetPositon=ValveControl.TargetPositon/pSysConfig->MotorValveConfig.MicroStep;
			Valve_Move[ValvePtr].MoveStep=ValveControl.Move_uStep/pSysConfig->MotorValveConfig.MicroStep;
			Valve_Move[ValvePtr].InvalFlag=ValveControl.OneOptTrigeFlagNoCont;
			Valve_Move[ValvePtr].StartPosition=ValveControl.StartPosition/pSysConfig->MotorValveConfig.MicroStep;
			memset(ValveControl.TriggerMotroPosition,0,sizeof(ValveControl.TriggerMotroPosition));
			#endif
			break;
		case VALVE_DIR_START:
			LED_ON();
			ValvePositionOptTriggerFlagCont=0;
			ValveZeroOptTriggerFlagCont=0;
			ValveControl.TriggerOptFlag=0;
			//Motor_SetVMAX(SysConfig.MotorValveConfig.MaxSpeed,MOTOR_VALVE);		
			ValveControl.StartPosition=Get_MotorPosition();		
			//获取当前位置和目标位置进行对比
			if(ValveControl.MoveDirection==0)
				speed=SysConfig.MotorValveConfig.RunSpeed;
			else
				speed=0-(int)SysConfig.MotorValveConfig.RunSpeed;
			ValveControl.Move_uStep=(pSysConfig->ValveConfig.FWD_ValveOptocouplerGapStep+(SysConfig.ValveConfig.MaxStep*pSysConfig->MotorValveConfig.MicroStep)/pSysConfig->ValveConfig.OptGapNum)/2;
			
			if(ValveControl.MoveDirection==0)
			{
				ValveControl.Move_uStep+=pSysConfig->ValveConfig.FlowAnticlockwisePhaseCompensation;
				ValveControl.TargetPositon+=pSysConfig->ValveConfig.FlowAnticlockwisePhaseCompensation;
				if(pSysConfig->ValveConfig.Using_ReductionBox)
				{
					if(ValveControl.TargetPositon<0)
						ValveControl.TargetPositon=SysConfig.ValveConfig.MaxStep*pSysConfig->MotorValveConfig.MicroStep+ValveControl.TargetPositon;
				}
				else
				{
					if(ValveControl.TargetPositon<=0)
						ValveControl.TargetPositon=pSysConfig->ValveConfig.MaxStep*pSysConfig->MotorValveConfig.MicroStep+ValveControl.TargetPositon;
				}
			}
			else
			{
				ValveControl.Move_uStep+=pSysConfig->ValveConfig.FlowClockwisePhaseCompensation;
				ValveControl.TargetPositon+=pSysConfig->ValveConfig.FlowClockwisePhaseCompensation;
				if(ValveControl.TargetPositon<0)
					ValveControl.TargetPositon=SysConfig.ValveConfig.MaxStep*pSysConfig->MotorValveConfig.MicroStep+ValveControl.TargetPositon;
			}
			
			ValveControl.PositionMoveFlag=0;
			ValveControl.StartOptTriggerFlag=ValveGet_Hole2OptInputState();
			if(ValveControl.OldMoveDirection!=ValveControl.MoveDirection)
			{
				//检测当前光耦是否为触发状态,如果是触发状态不用忽略第一个光耦
				if(ValveControl.StartOptTriggerFlag==0&&ValveControl.CurrentOptocoupler2Cont!=0)
//				if(ValveControl.CurrentOptocoupler2Cont!=0)
				{
					ValveControl.OneOptTrigeFlagNoCont=1;
				}
				ValveControl.OldMoveDirection=ValveControl.MoveDirection;
			}			
			Valve_GetOptTriggerCont();
			
			ValveControl.StartBeforeOptocoupler2Cont=ValveControl.CurrentOptocoupler2Cont;
			Valve_NextState(VALVE_RUN);
			Valve_SpeedMove(speed);// 速度模式开始找光耦零位
			Valve_AddRunTimes();
			ValveControl.CurrentTriggerOptCont=0;
			
			#if DEBUG_VALVE			
			memcpy(Valve_Move[ValvePtr].TriggerMotorPosition,ValveControl.TriggerMotroPosition,sizeof(ValveControl.TriggerMotroPosition));
			ValvePtr++;
			if(ValvePtr==50)
				ValvePtr=0;
			Valve_Move[ValvePtr].StartOptTriggerFlag=ValveControl.StartOptTriggerFlag;
			Valve_Move[ValvePtr].oldPort=ValveControl.CurrentPort;
			Valve_Move[ValvePtr].TargetPort=ValveControl.TargetPort;
			Valve_Move[ValvePtr].Dir=ValveControl.MoveDirection;
			Valve_Move[ValvePtr].CurrentOptCont=ValveControl.CurrentTriggerOptCont;
			Valve_Move[ValvePtr].CurrentOpt2Cont=ValveControl.CurrentOptocoupler2Cont;
			Valve_Move[ValvePtr].TargetOptCont=ValveControl.TargetTriggerOptCont;
			Valve_Move[ValvePtr].TargetPositon=ValveControl.TargetPositon/pSysConfig->MotorValveConfig.MicroStep;
			Valve_Move[ValvePtr].MoveStep=ValveControl.Move_uStep/pSysConfig->MotorValveConfig.MicroStep;
			Valve_Move[ValvePtr].InvalFlag=ValveControl.OneOptTrigeFlagNoCont;
			Valve_Move[ValvePtr].StartPosition=ValveControl.StartPosition/pSysConfig->MotorValveConfig.MicroStep;
			memset(ValveControl.TriggerMotroPosition,0,sizeof(ValveControl.TriggerMotroPosition));		
			#endif
		break;
    case VALVE_RUN:
			//等待运动到位
			//在速度模式不读取状态，避免出现读出和读位置函数冲突
			if(ValveControl.PositionMoveFlag==1)
			{
				
				ValveControl.MotorRAMPStatus=Get_RAMP_Status();
				//if((ValveControl.MotorRAMPStatus&0x00000280)&&(ValveControl.MotorHardStatus&0x80000000))
				if((ValveControl.MotorRAMPStatus&0x00000200))
				{
					Motor_ClearReachPosition_Status();
					Valve_NextState(VALVE_STOP);					
				}
			}
    break;
    case VALVE_STOP:
			LED_OFF();
			//Motor_HardStop();
			#if (USED_ENCODER == 1)
			Encoder_StopComparedCapture();
			#endif
			ValveControl.CurrentPort=ValveControl.TargetPort;
			Set_MotorPosition(ValveControl.TargetPositon);						//位置清零
			#if (USED_ENCODER == 1)
			Encoder_StopComparedCapture();
			#endif
			if(PortInitFlag)
			{
				WaitCnt++;
				if(WaitCnt>=120/VALVE_CONTROL_TASK_TIME)
				{
					WaitCnt = 0;
					PortInitFlag = 0;
					Valve_SetPort(SysConfig.ValveConfig.ResetPort);
				}
				return;
			}
			
			WaitCnt++;
			if(WaitCnt>=10/VALVE_CONTROL_TASK_TIME)
			{
				WaitCnt = 0;
				Valve_NextState(VALVE_IDLE);
				ValveControl.ErrorAutoResetCont=0;
				if(SendTitanFlag)
				{
					SendTitanFlag = 0;
					ResponseTiTan_Send("\r",1);
				}	
			}
		break;
		case VALVE_FAULT:
			LED_OFF();
      Motor_HardStop();
			Valve_NextState(VALVE_FAULT_IDLE);
			ValveFaultTimeCont=0;
			ValveFlashingTimes=ValveControl.hFaultNow;
			#if (USED_ENCODER == 1)
			Encoder_StopComparedCapture();
			#endif
			ValveControl.CurrentMotorPosition=Get_MotorPosition();
		break;
		case VALVE_FAULT_IDLE:
			ValveFaultTimeCont++;      
      if(ValveFaultTimeCont>=250/VALVE_CONTROL_TASK_TIME)
      {
        ValveFaultTimeCont=0;   
        if(ValveFaultLedFlashingTimes<ValveFlashingTimes*2)
        {
          LED_Toggle();
        }
        else if(ValveFaultLedFlashingTimes==ValveFlashingTimes*2)
        {
          LED_OFF();          
        }
        else if(ValveFaultLedFlashingTimes>=ValveFlashingTimes*2+8)
        {
          ValveFaultLedFlashingTimes=0;
        }
        ValveFaultLedFlashingTimes++;
      }
      break;
  }
}


/*
速度模式运行旋切阀
输入：speed(microstep/s)
输出：无
*/
void Valve_SpeedMove(int32_t uSpeed)
{
	TMC_DRV_ON();
#if TMC5160_STOP_MODE
	TMC_EXT_RELEASE();
#endif
	if(uSpeed>0)
	{
		TMC5160_SPIWriteInt(0x20,1);
		ValveControl.MoveDirection = 0;
	}
	else if(uSpeed<0)
	{
		TMC5160_SPIWriteInt(0x20,2);
		ValveControl.MoveDirection = 1;
	}
	else
	{
		Valve_NextState(VALVE_STOP);
		return;
	}
	Motor_SetVMAX(abs(uSpeed));
}
/*
定向位移模式运行阀
输入：uStep(uStep)
输出：无
*/
int Valve_StepMoveDir(int32_t uStep)
{
	int32_t postion,offset;
	uint8_t Dir;
	//获取当前位置和目标位置进行对比
	postion=ValveControl.CurrentMotorPosition;
	if(uStep>0)
		Dir = 0;
	else if(uStep<0)
		Dir = 1;
	else
	{
		Valve_ImmediatelyStopMotor();
		Valve_NextState(VALVE_STOP);
		return 1;
	}
	offset=postion+uStep;
	Motor_MovePosition(offset);		
	ValveControl.PositionMoveFlag=1;
	return 0;
}
uint8_t Valve_GetPort(void)
{
	return ValveControl.CurrentPort;
}


/*
函数功能：设置阀端口，自动最优路径
输入参数：channel:端口号
输出参数：1：已经是当前位置
					0：设置成功
					-1：设置失败，没有该端口号
*/
int Valve_SetPort(uint8_t channel)
{
	uint8_t i,num;
	int32_t positoin;		
	if(SysConfig.ValveConfig.PortNum == 10 && !SysConfig.Not_Change)
		channel = ChangePosition(channel);

	if(channel>SysConfig.ValveConfig.PortNum||channel==0)
	{
		return -1;
	}
	if(ValveControl.CurrentPort == channel)
	{
		Valve_NextState(VALVE_STOP);
		return 1;
	}
	for(i=0;i<pSysConfig->ValveConfig.PortNum;i++)
	{	
		if(channel==pSysConfig->ValveConfig.Port[i].Port)
		{
			ValveControl.TargetPort=channel;
			ValveControl.TargetPositon=pSysConfig->ValveConfig.Port[i].MotorPosition;			
			Valve_NextState(VALVE_START);
			Valve_SafetyTask();		//更新状态	
			return 0;
		}
	}
}
/*
函数功能：设置阀端口，指定方向
输入参数：channel:端口号
输出参数：1：已经是当前位置
					0：设置成功
					-1：设置失败，没有该端口号
*/
int Valve_SetPortDir(uint8_t dir,uint8_t channel)
{
	uint8_t i,num;
	int32_t positoin;		
	if(SysConfig.ValveConfig.PortNum == 10 && !SysConfig.Not_Change)
		channel = ChangePosition(channel);

	if(channel>SysConfig.ValveConfig.PortNum||channel==0)
	{
		return -1;
	}
	if(ValveControl.CurrentPort == channel)
	{
		Valve_NextState(VALVE_STOP);
		return 1;
	}
	for(i=0;i<pSysConfig->ValveConfig.PortNum;i++)
	{	
		if(channel==pSysConfig->ValveConfig.Port[i].Port)
		{
			ValveControl.TargetPort=channel;
			ValveControl.TargetPositon=pSysConfig->ValveConfig.Port[i].MotorPosition;
			ValveControl.MoveDirection=dir;
			Valve_NextState(VALVE_DIR_START);
			Valve_SafetyTask();		//更新状态	
			return 0;	
		}
	}
}
/**************************
函数功能：立即停止电机
***************************/
void Valve_ImmediatelyStopMotor(void)
{
  Motor_HardStop();
  Valve_NextState(VALVE_STOP);
}


/*
函数功能：获取位置光耦触发状态
函数说明：用于检测当前位置光耦是否触发
输入参数：无
输出参数：1：触发
          0：未触发
*/				
uint8_t ValveGet_Hole1OptInputState(void)
{
  #if USING_OPT_INVERTER
  if(HAL_GPIO_ReadPin(OW1_GPIO_Port, OW1_Pin)==GPIO_PIN_SET)
	#else
	if(HAL_GPIO_ReadPin(OW1_GPIO_Port, OW1_Pin)==GPIO_PIN_RESET)
	#endif 
  {
    return 1;
  } 
  else
  {
    return 0;
  }
}
/*
函数功能：	稳定获取光耦触发状态
函数说明：	用于检测当前位置光耦是否触发
输入参数：	cont：循环检测次数
					Status：触发状态,0：未触发，1：触发
输出参数：	0：状态与Status不一致
					1：状态与Status一致
*/				
uint8_t ValveFineGet_Hole1OptocouplerInputState(uint32_t cont,uint8_t Status)
{
	uint8_t TrigStatus;
	uint32_t OkCont=0;
	uint32_t ErrorCont=0;
	uint32_t TrigCont=cont;
	while(TrigCont--)
	{
		OptDelay(5);
		TrigStatus=ValveGet_Hole1OptInputState();
		if(TrigStatus!=Status)
		{
			ErrorCont++;
			if(ErrorCont>(cont*20)/100)
				return 0;
		}
		else
		{
			OkCont++;
		}
	}
	if(OkCont>(cont*80)/100)
		return 1;
	else
		return 0;
}
/*
函数功能：获取位置光耦触发状态
函数说明：用于检测当前位置光耦是否触发
输入参数：无
输出参数：1：触发
          0：未触发
*/				
uint8_t ValveGet_Hole2OptInputState(void)
{
	#if USING_OPT_INVERTER
  if(HAL_GPIO_ReadPin(OW2_GPIO_Port, OW2_Pin)==GPIO_PIN_SET)
	#else
	if(HAL_GPIO_ReadPin(OW2_GPIO_Port, OW2_Pin)==GPIO_PIN_RESET)
	#endif 
  {
    return 1;
  } 
  else
  {
    return 0;
  }
}
/*
函数功能：	稳定获取光耦触发状态
函数说明：	用于检测当前位置光耦是否触发
输入参数：	cont：循环检测次数
					Status：触发状态,0：未触发，1：触发
输出参数：	0：状态与Status不一致
					1：状态与Status一致
*/				
uint8_t ValveFineGet_Hole2OptocouplerInputState(uint32_t cont,uint8_t Status)
{
	uint8_t TrigStatus;
	uint32_t OkCont=0;
	uint32_t ErrorCont=0;
	uint32_t TrigCont=cont;
	while(TrigCont--)
	{
		OptDelay(10);
		TrigStatus=ValveGet_Hole2OptInputState();
		if(TrigStatus!=Status)
		{
			ErrorCont++;
			if(ErrorCont>(cont*20)/100)
				return 0;
		}
		else
		{
			OkCont++;
		}
	}
	if(OkCont>(cont*80)/100)
		return 1;
	else
		return 0;
}


/*
函数功能：置零光耦触发
*/
void Valve_ZeroOptTrig(void)
{
	uint8_t TrigeStatus;
	//OptDelay(1000);
	if((ValveControl.bState!=VALVE_AUTO_ZERO_RUN)&&(ValveControl.bState!=VALVE_RUN))
		return ;
	if(ValveFineGet_Hole1OptocouplerInputState(200,1))
	{
		ValveZeroOptTriggerFlag=1;
	}
}

/*
函数功能：置零光耦处理
*/
void Valve_ZeroOptFunction(void)
{
	uint8_t TrigeStatus;

	if(ValveControl.autoZeroMode)
	{
		ValveControl.autoZeroMode=false;
		ValveControl.ZeroMarkFlag=true;
		ValveControl.CurrentOptocoupler2Cont=0;
		Motor_HardStop();
//		Valve_ImmediatelyStopMotor();
//		SoftDelay(500000);
//		Set_MotorPosition(0);						//位置清零
//		if(ValveControl.ErrorAutoResetFlag)
//		{
//			ValveControl.ErrorAutoResetFlag=0;
//			Valve_SetPort(ValveControl.ErrorResetChannel);
//		}
//		else
//		{
//				ValveControl.CurrentPort=1;
//				ValveControl.TargetPort = 1;
//				if(SysConfig.ValveConfig.PortNum!=10)
//				{
//					if(SysConfig.ValveConfig.FlowAnticlockwisePhaseCompensation||SysConfig.ValveConfig.FlowClockwisePhaseCompensation)
//					{
//						Valve_SetPort(SysConfig.ValveConfig.PortNum);
//						PortInitFlag = 1;
//					}
//					else
//					{
//						Valve_SetPort(SysConfig.ValveConfig.ResetPort);
//					}
//				}
//				else
//				{
//					Valve_SetPort(SysConfig.ValveConfig.ResetPort);
//				}
//		}
	}
	else
	{
		//目标位置不在零位光耦附近,设置当前位置为0
		//if(((ValveControl.PositionMoveFlag==0))&&(ValveControl.TriggerOptFlag==1))
		if(ValveControl.TriggerOptFlag==1)
		//if((ValveControl.TriggerOptFlag==1)&&((ValveControl.PositionMoveFlag==0)||(ValveControl.TargetPort==1&&ValveControl.PositionMoveFlag==1)))
		{
			if(ValveControl.MoveDirection==0)
			{
				if(ValveControl.CurrentOptocoupler2Cont!=pSysConfig->ValveConfig.OptGapNum)
				{
					Valve_SetFaultFlag(3);	//运行过程中光耦错误
					return ;
				}
			}
			else 
			{
				if(ValveControl.CurrentOptocoupler2Cont!=1)
				{
					Valve_SetFaultFlag(3);//运行过程中光耦错误
					return ;
				}
			}
//			ValveControl.CurrentOptocoupler2Cont=0;	
		}
	}	
}
uint8_t ErrorCont1=0,ErrorCont2=0,ErrorCont3=0,ErrorCont4=0;



/*
位置光耦触发
*/
void Valve_Hole2OptTrig(void)
{ 
	uint8_t TrigeStatus;
	int32_t PositionDiff;
	int32_t CurrentOptPosition;
	if(ValveControl.bState==VALVE_AUTO_ZERO||ValveControl.bState==VALVE_AUTO_ZERO_RUN)
		return ;
	else if(ValveControl.ZeroMarkFlag==0)
		return ;
	else if(ValveControl.bState!=VALVE_RUN)
		return ;
//	OptDelay(5000/(SysConfig.MotorValveConfig.RunSpeed/32));
  if(ValveFineGet_Hole2OptocouplerInputState(200,1))//200
	{
		ValvePositionOptTriggerFlag=1;
	}
}
/*
函数功能：对孔光耦2处理,3个缺口
*/
int OldMotorPosition;

void Valve_Hole2OptFunction(void)
{ 
	uint8_t TrigeStatus;
	int32_t PositionDiff;
	int32_t CurrentOptPosition;

	ValveControl.CurrentMotorPosition=Get_MotorPosition();
	
//	//检测启动时第一个光耦是否未检测到,检测第一个触发光耦和启动位置偏差是否大于两个光耦之间间距+20步
//	if(ValveControl.TriggerOptFlag==0&&\
//		(abs(ValveControl.CurrentMotorPosition-ValveControl.StartPosition)>\
//	pSysConfig->ValveConfig.OptInterval+20*pSysConfig->MotorValveConfig.MicroStep))
//	{
//		Valve_SetFaultFlag(7);//启动检测光耦错误
//		return ;
//	}
	//忽略停止时的抖动数据

	if((abs(OldMotorPosition-ValveControl.CurrentMotorPosition)<(pSysConfig->ValveConfig.MaxGapStep*pSysConfig->MotorValveConfig.MicroStep))&&ValveControl.PositionMoveFlag==1)
	{
//		if(pSysConfig->ValveConfig.Using_ReductionBox)
		if(pSysConfig->ValveConfig.Using_ReductionBox || SysConfig.ValveConfig.PortNum == 24)
		{
			ErrorCont1++;
			OldMotorPosition=ValveControl.CurrentMotorPosition;
			return ;
		}
	}		
//	else
//	{
//		if(ValveControl.PositionMoveFlag==1)
//    {
//			ErrorCont1++;
//			OldMotorPosition=ValveControl.CurrentMotorPosition;
//			return ;
//		}		
//	}
	//忽略第一个抖动数据
	if(ValveControl.TriggerOptFlag==1&&ValveControl.CurrentTriggerOptCont==1&&
		(abs(ValveControl.CurrentMotorPosition-ValveControl.StartPosition)<(SysConfig.ValveConfig.MaxGapStep*pSysConfig->MotorValveConfig.MicroStep)))
	{
//		if(SysConfig.ValveConfig.Using_ReductionBox)
		if(pSysConfig->ValveConfig.Using_ReductionBox || SysConfig.ValveConfig.PortNum == 24)
		{
			ErrorCont2++;
			return;
		}
	}

	if(SysConfig.ValveConfig.PortNum!=24)
	{
		//忽略抖动数据
		if(ValveControl.TriggerOptFlag==1&&(ValvePositionOptTriggerTimeCont<5))
		{
			ErrorCont3++;
			return ;
		}
	}
//	//启动前光耦处于触发状态，忽略第一个光耦触发距离短的状态
//	if(ValveControl.StartOptTriggerFlag)
//	{
//		ValveControl.StartOptTriggerFlag=0;
//		if(abs(ValveControl.CurrentMotorPosition-ValveControl.StartPosition)<pSysConfig->ValveConfig.OptInterval/2)
//		{
//			ErrorCont3++;
//			return ;
//		}
//	}
	OldMotorPosition=ValveControl.CurrentMotorPosition;
	ValveControl.TriggerMotroPosition[ValveControl.CurrentTriggerOptCont]=ValveControl.CurrentMotorPosition/pSysConfig->MotorValveConfig.MicroStep;
//	if(ValveControl.CurrentTriggerOptCont>1)
//	{
//		if(abs(ValveControl.TriggerMotroPosition[ValveControl.CurrentTriggerOptCont]-ValveControl.TriggerMotroPosition[ValveControl.CurrentTriggerOptCont-1])<2)
//		{	
//			//Valve_SetFaultFlag(9);
//			return ;
//		}
//	}
	ValvePositionOptTriggerTimeCont=0;
	//忽略第一个反向运动触发光耦位置计数改变
	if(ValveControl.OneOptTrigeFlagNoCont)
	{
		ValveControl.OneOptTrigeFlagNoCont=0;
		ValveControl.CurrentTriggerOptCont++;
	}
	else
	{
		ValveControl.OneOptTrigeFlagNoCont=0;	
		//计算上一个光耦触发位置与当前光耦触发位置差
		PositionDiff=abs(ValveControl.OldOptTrigeMotorPosition-ValveControl.CurrentMotorPosition);
		//避免出现干扰大时将上升沿检测为下降沿出现误计数,但是改功能启用后对24通产生影响
		if(SysConfig.ValveConfig.PortNum!=24)
		{
			if(SysConfig.ValveConfig.Using_ReductionBox)
			{
				if(ValveControl.TriggerOptFlag==1&&ValveControl.CurrentOptocoupler2Cont!=0&&abs(PositionDiff)<pSysConfig->ValveConfig.MaxGapStep*pSysConfig->MotorValveConfig.MicroStep)
				{
					ErrorCont4++;
					return ;
				}
			}
		}
		ValveControl.CurrentTriggerOptCont++;
		if(ValveControl.MoveDirection==0)
		{
			if(ValveControl.CurrentOptocoupler2Cont>=pSysConfig->ValveConfig.OptGapNum)
				ValveControl.CurrentOptocoupler2Cont=1;
			else
				ValveControl.CurrentOptocoupler2Cont++;
		}
		else
		{
			if(ValveControl.CurrentOptocoupler2Cont<=1)       
				ValveControl.CurrentOptocoupler2Cont=pSysConfig->ValveConfig.OptGapNum;
			else
				ValveControl.CurrentOptocoupler2Cont--;
		}
	}
	
	if((ValveControl.TriggerOptFlag==1)&&(ValveControl.autoZeroMode==0)&&ValveControl.OldOptTrigeMotorPosition!=0&&ValveControl.CurrentMotorPosition!=0)
	{
		//检测光耦触发后，电机运动的步数是否与实际运动步数偏差较大
		//置零光耦附近不做计算
		if((ValveControl.CurrentOptocoupler2Cont>=2)&&(ValveControl.CurrentOptocoupler2Cont<pSysConfig->ValveConfig.OptGapNum))
		{
			//偏差大时提示堵转
			if(abs(PositionDiff)<(pSysConfig->ValveConfig.OptInterval/2)||abs(PositionDiff)>(pSysConfig->ValveConfig.OptInterval+pSysConfig->ValveConfig.OptInterval/2))
			{
 				Valve_SetFaultFlag(5);//位置错误(误触发或者丢触发)
				return;
			}
		}
	}
	ValveControl.OldOptTrigeMotorPosition=ValveControl.CurrentMotorPosition;
	//设置第一个光耦已经触发标志
	ValveControl.TriggerOptFlag=1;
	//获取当前光耦触发理论电机位置
	CurrentOptPosition=pSysConfig->ValveConfig.OptPosition[ValveControl.CurrentOptocoupler2Cont-1];
	//避免最大位置光耦触发后移动到1通道出现位置值偏差大计算
	if((ValveControl.CurrentOptocoupler2Cont==pSysConfig->ValveConfig.OptGapNum)&&\
		(ValveControl.TargetPositon<pSysConfig->ValveConfig.OptInterval/2)&&\
	(ValveControl.TargetPositon>=0))
	{
		CurrentOptPosition=CurrentOptPosition-SysConfig.ValveConfig.MaxStep*pSysConfig->MotorValveConfig.MicroStep;
	}
	//避免反向补偿值为负数时，切换1通道出现切换到最大通道问题
	else if(ValveControl.CurrentOptocoupler2Cont==1&&\
		ValveControl.TargetPositon>(SysConfig.ValveConfig.MaxStep*pSysConfig->MotorValveConfig.MicroStep-pSysConfig->ValveConfig.OptInterval))
	{
		CurrentOptPosition=CurrentOptPosition+SysConfig.ValveConfig.MaxStep*pSysConfig->MotorValveConfig.MicroStep;
	}
	//检测是否切换到通道，开始切换到位置模式减速并移动指定距离到两个光耦缝之间
	if(ValveControl.PositionMoveFlag==0&&(abs(CurrentOptPosition-ValveControl.TargetPositon)<pSysConfig->ValveConfig.OptInterval))
	{
		//检测光耦触发计数是否正确
		if(ValveControl.CurrentTriggerOptCont!=ValveControl.TargetTriggerOptCont)
			Valve_SetFaultFlag(6);//位置错误
		else
		{
			//位置模式移动指定距离
			if(ValveControl.MoveDirection==0)
				Valve_StepMoveDir(ValveControl.Move_uStep);
			else
				Valve_StepMoveDir(0-ValveControl.Move_uStep);
		}
	}
}
uint8_t Valve_ErrorAutoReset(void)
{	
  
  if(pSysConfig->ValveConfig.ErrorAutoResetAllowCont&&(ValveControl.ErrorAutoResetCont<pSysConfig->ValveConfig.ErrorAutoResetAllowCont))
  {
    ValveControl.ErrorAutoResetCont++;
    ValveControl.ErrorAutoResetFlag=1;
    ValveControl.ErrorResetChannel=ValveControl.TargetPort;
		Value_ClearFault();
    Valve_ImmediatelyStopMotor();
    Valve_AutoZeroMode();
		return 1;
  }
	else
		return 0;
}

void Valve_AddRunTimes(void)
{
//	SysRunCont_type *pSysRunCont;
//	pSysRunCont=GetSysRunContFromFlash();
//	pSysRunCont->ValveCont++;
//	if(pSysRunCont->ValveCont%VALVE_RUN_CONT_MUL==0)
//	{
//		WriteRunContToFlash(pSysRunCont);
//	}
}


