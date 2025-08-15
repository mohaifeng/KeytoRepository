#include "modbus.h"


Modbus_List_t Modbus_List = 
{
	.num = 0,
};

ModbusStruct_t ModBusDat;
extern uint8_t RxUsartNum;
extern Valve_Handle_t ValveControl;

uint16_t PID = 0,UID = 0,VERSION = 0;


/*
函数功能:crc校验
*/
uint16_t GetCrcData(uint8_t *buffer, uint32_t len)
{
  uint16_t wcrc = 0XFFFF;//16bit CRC寄存器预置
  uint8_t temp,CRC_L,CRC_H;//定义数组
  uint32_t i = 0, j = 0;//计数
  for (i = 0; i < len; i++)//循环计算每个数据
  {
    temp = *buffer & 0X00FF;//将8位数据域CRC寄存器异或
    buffer++;//指针地址增加指向下一个地址
    wcrc ^= temp;//将数据存入CRC寄存器
    for (j = 0; j < 8; j++)//循环计算数据
    {
      if (wcrc & 0X0001)//计算右移出的数据是不是1，如果是1则与多项式异或
      {
				wcrc >>= 1;//先将数据右移1位
				wcrc ^= 0XA001;//与多项式进行异或
      }
      else//如果不是1则直接移出
      {
				wcrc >>= 1;
      }
    }
  }
	
  CRC_L = wcrc&0xff;
  CRC_H = wcrc >> 8;
  return ((CRC_L << 8) | CRC_H);
}

uint16_t BoundToValue(uint32_t bound)
{
	switch(bound)
	{
		case 9600:
			return 0;
		break;
		case 19200:
			return 1;
		break;
		case 38400:
			return 2;
		break;
		case 57600:
			return 3;
		break;
		case 115200:
			return 4;
		break;
		default:
			return 0;
	}
}

uint32_t ValueToBound(uint16_t value)
{
	switch(value)
	{
		case 0:
			return 9600;
		break;
		case 1:
			return 19200;
		break;
		case 2:
			return 38400;
		break;
		case 3:
			return 57600;
		break;
		case 4:
			return 115200;
		break;
		default:
			return 9600;
	}
}

uint16_t Get_SystemStatus(uint8_t mode)
{
	uint16_t ReturnValue=0,ErrorStatus,ret;

	if(ValveControl.bState==VALVE_IDLE || ValveControl.bState==VALVE_FAULT_IDLE)
			ReturnValue=0;
	else
			ReturnValue=1;
	if(ValveControl.bState==VALVE_FAULT_IDLE )
	{
			ErrorStatus=ValveControl.hFaultNow;
			if(ErrorStatus == 4)            //驱动芯片故障
					ReturnValue|=(1<<8);         
			if(ErrorStatus == 2)			//光耦故障
					ReturnValue|=(1<<9);
			if(ErrorStatus == 2)			//堵转
					ReturnValue|=(1<<10);
			if(ErrorStatus == 3)			//运行过程中光耦错误
					ReturnValue|=(1<<11);
			if(ErrorStatus == 5 || ErrorStatus == 6)			//位置错误
					ReturnValue|=(1<<12);
	}
	return ReturnValue;
}

void Modbus_Ack(uint16_t *data,uint16_t len)
{
  uint16_t CalculateVerify;
  uint8_t sendData[8];
  
  sendData[0]= SysConfig.CommunicationConfig.Add;
  sendData[1]=MODBUS_WRITE;
  sendData[2]=len;
  sendData[3]=(*data>>8);
  sendData[4]=(*data&0xFF);
  CalculateVerify=GetCrcData(sendData,len+3);
  sendData[5]=CalculateVerify>>8;
  sendData[6]=CalculateVerify&0xff;
  SendCommunicationData(RxUsartNum,sendData,7);
}

void Modbus_Send_multi(uint16_t *data , uint16_t len)
{
  uint16_t CalculateVerify;
  uint8_t sendData[8];
  sendData[0]=SysConfig.CommunicationConfig.Add;
  sendData[1]=MODBUS_READ;
  sendData[2]=len; 
  sendData[3]=(*data>>8);
  sendData[4]=(*data&0xFF); 
  CalculateVerify=GetCrcData(sendData,len+3);
  sendData[5]=CalculateVerify>>8;
  sendData[6]=CalculateVerify&0xff;
  SendCommunicationData(RxUsartNum,sendData,7);
}

void Modbus_Send(uint16_t *data , uint16_t len)
{
  uint16_t CalculateVerify,i;
  uint8_t sendData[100];
  
  sendData[0]= SysConfig.CommunicationConfig.Add;
  sendData[1]=MODBUS_READ;
  sendData[2]=len*2;
  for(i=0;i<len;i++)
  {
    sendData[3+i*2]=((*data)>>8);
    sendData[4+i*2]=((*data)&0xFF);
    data++;
  }
 
  CalculateVerify=GetCrcData(sendData,(3+len*2));
  sendData[5+(len-1)*2]=CalculateVerify>>8;
  sendData[6+(len-1)*2]=CalculateVerify&0xff;
  SendCommunicationData(RxUsartNum,sendData,len*2+5);
}



int ModBusSetPosition(ModbusStruct_t *pData)
{
	if(pData->authority!=6)
		return -1;
	if(ValveControl.bState == VALVE_IDLE)
	{
		return Valve_SetPort(pData->data);
	}
	else
		return 1;
	return 0;
}

int ModBusSetPositionDir0(ModbusStruct_t *pData)
{
	if(pData->authority!=6)
		return -1;
	if(ValveControl.bState == VALVE_IDLE)
	{
		return Valve_SetPortDir(0,pData->data);
	}
	else
		return 1;
	return 0;
}

int ModBusSetPositionDir1(ModbusStruct_t *pData)
{
	if(pData->authority!=6)
		return -1;
	if(ValveControl.bState == VALVE_IDLE)
	{
		return Valve_SetPortDir(1,pData->data);
	}
	else
		return 1;
	return 0;
}


int ModBusReturnZero(ModbusStruct_t *pData)
{
	if(pData->authority!=6)
		return -1;
	if(ValveControl.bState == VALVE_IDLE)
	{
		if(ValveControl.bState == VALVE_IDLE || ValveControl.bState==VALVE_FAULT_IDLE)
			Valve_AutoZeroMode();
	}
	else
		return 1;
	return 0;
}



int ModBusMotorStop(ModbusStruct_t *pData)
{
	if(pData->authority!=6)
		return -1;
	Valve_ImmediatelyStopMotor();
	return 0;
}

int ModBusClearError(ModbusStruct_t *pData)
{
	if(pData->authority!=6)
		return -1;
	Value_ClearFault();
	return 0;
}


int ModBusRestoreFactory(ModbusStruct_t *pData)
{
	if(pData->authority!=6)
		return -1;
	if(pData->data==0x1234)
	{
		SysConfig.InitFlag=0;
		if(WriteSysConfigToFlash(&SysConfig)!=0)
			return 1;
		else					
			return 0;
	}
	else
		return 1;
	return 0;
}

int ModBusSetingSave(ModbusStruct_t *pData)
{
	if(pData->authority!=6)
		return -1;
	if(pData->data==0x1234)
	{
		if(WriteSysConfigToFlash(&SysConfig)!=0)
			return 1;
		else					
			return 0;
	}
	else
		return 1;
	return 0;
}


int ModBusGeneralWrite(ModbusStruct_t *pData)
{
	uint8_t i = 0;
	struct Modbus_Register_t *p,*pNext;
	int32_t temp;
	p = &(Modbus_List.Modbus_Register[pData->index]);
	pNext = p->nextnode;

	if(p->authority == ONLY_READ)
		return -1;
	if(p->reg == 0x0058)
	{
		if(pData->data>=1&&pData->data<=30)
			setValueChannel(pData->data);
		else
			return 1;
	}
	else if(p->reg == 0x006E)
	{
		SysConfig.CommunicationConfig.RS232_BaudRate = (pData->data);
		SysConfig.CommunicationConfig.RS485_BaudRate = (pData->data);
	}
	else if(p->reg == 0x006D)
	{
		if(pData->data == 100 || pData->data == 120 || pData->data == 250 || pData->data == 500 || pData->data == 800 || pData->data == 1000)
			SysConfig.CommunicationConfig.CAN_BaudRate=pData->data;
		else
			return 1;
 	}
	else
	{
		if(p->size == U8)
		{
			temp =  (uint8_t)(pData->data & 0xFF);
			if((temp >= p->floor) && (temp <= p->upper))
				*((uint8_t*)p->data) = (temp&0xFFFF);
			else
				return 1;
		}
		else if(p->size == S16)
		{
			temp = (int16_t)(pData->data & 0xFFFF);
			if((temp >= p->floor) && (temp <= p->upper))
				*((int16_t*)p->data) = (temp&0xFFFF);
			else
				return 1;
		}
		else
		{
			temp = (uint16_t)(pData->data & 0xFFFF);
			if((temp >= p->floor) && (temp <= p->upper))
			{
				if((p->reg > 0x0050) && (p->reg < 0x0055))
				{
					if(p->reg == 0x0051)
						SysConfig.MotorValveConfig.RunSpeed = (temp&0xFFFF)*SysConfig.MotorValveConfig.MicroStep;		
					*((uint32_t*)p->data) = (temp&0xFFFF)*SysConfig.MotorValveConfig.MicroStep;
				}
				else
					*((uint16_t*)p->data) = (temp&0xFFFF);
			}
			else
				return 1;
		}
		if((p->reg > 0x0050) && (p->reg < 0x0056))
			TMC5160_Init();

	}
	return 0;
}


int ModBusGeneralRead(ModbusStruct_t *pData)
{
	uint8_t i = 0;
	struct Modbus_Register_t *p,*pNext;
	uint16_t SBuff[100];
	p = &(Modbus_List.Modbus_Register[pData->index]);
	pNext = p->nextnode;
	memset(SBuff,0,sizeof(SBuff));
	if(!pData->data)
		return -1;
	while(i<pData->data)
	{
		if(p->reg == 0x0058)
			SBuff[i] = (uint16_t)SysConfig.ValveConfig.PortNum;
		else if(p->reg == 0x006D)
			SBuff[i] = SysConfig.CommunicationConfig.CAN_BaudRate;
		else if(p->reg == 0x006E)
			SBuff[i] = SysConfig.CommunicationConfig.RS232_BaudRate;
		else if(p->reg == 0x0090)
			SBuff[i] = Get_SystemStatus(0);
		else if(p->reg == 0x0091)
			SBuff[i] = Valve_GetCurrent_Channel();
		else
		{
			if(p->size == U8)
				SBuff[i] = (int16_t)(*((uint8_t*)p->data));
			else if(p->size == S16)
				SBuff[i] = (int16_t)(*((int16_t*)p->data));
			else
			{
				if((p->reg > 0x0050) && (p->reg < 0x0055))
					SBuff[i] = (uint16_t)((*((uint32_t*)p->data))/SysConfig.MotorValveConfig.MicroStep);
				else
					SBuff[i] = (uint16_t)(*((uint16_t*)p->data));
			}	
		}
		i++;		
		if(pNext == 0 || p->authority == ONLY_WRITE)
			break;
		p = pNext;
		pNext = p->nextnode;
	}
	Modbus_Send(SBuff,i);
	return -2;
}



int ModBusGeneralCallback(ModbusStruct_t *pData)
{
	if(pData->authority == MODBUS_READ)
		return ModBusGeneralRead(pData);
	else if(pData->authority == MODBUS_WRITE)
		return ModBusGeneralWrite(pData);
}


ModbusStruct_t Oparam;
void ModBusAnalysis(uint8_t *data,uint8_t len)
{
	uint16_t checksum,i;
	int ret = 0;
	int (*pfunc)(ModbusStruct_t*);
	pfunc=0;
	if(len!=8)
		return;
	Oparam.authority = data[1];
	if(Oparam.authority < 0x03  || Oparam.authority > 0x06)//modbus function code
		return;
	Oparam.reg_addr = (data[2]<<8|data[3]);//modbus regaddr
	if(!Oparam.reg_addr)
		return;
	Oparam.data = (data[4]<<8|data[5]);//read:data length  write:data
	Oparam.checksum = (data[6]<<8|data[7]);
	checksum = GetCrcData(data,len-2);
	if(checksum!=Oparam.checksum)
		return;
	
	for(i=0;i<Modbus_List.num;i++)
	{
		if(Oparam.reg_addr == Modbus_List.Modbus_Register[i].reg)
		{
			pfunc = Modbus_List.Modbus_Register[i].callback;
			Oparam.index = i;
			break;
		}
	}
	
	if(pfunc == 0)
		return;
	ret = pfunc(&Oparam);
	
	if(ret >= 1)
		Modbus_Ack((uint16_t*)&ret,2);
	else if(ret == 0)
		sentUartData(data,len);
}


void ModbusCmdRegister(uint16_t reg,Access_t authority,TypeSize_t size,uint16_t floor,uint16_t upper,uint16_t *data,struct Modbus_Register_t *next,func cb)
{
	Modbus_List.Modbus_Register[Modbus_List.num].reg = reg;
	Modbus_List.Modbus_Register[Modbus_List.num].authority = authority;
	Modbus_List.Modbus_Register[Modbus_List.num].size = size;
	Modbus_List.Modbus_Register[Modbus_List.num].floor = floor;
	Modbus_List.Modbus_Register[Modbus_List.num].upper = upper;
	Modbus_List.Modbus_Register[Modbus_List.num].data = data;
	Modbus_List.Modbus_Register[Modbus_List.num].nextnode = next;
	Modbus_List.Modbus_Register[Modbus_List.num].callback = cb;
	Modbus_List.num++;
}


void ModbusCmdInit(void)
{
	uint8_t i = 0;
	
	PID = (uint16_t)(SYS_FACILITY_ID&0xffff);
	UID = (uint16_t)(HAL_GetUIDw0() & 0xffff);
	VERSION |= ((SYS_SOFT_VERSIONS%1000)/100)<<8;
	VERSION |= ((SYS_SOFT_VERSIONS%100)/10)<<4;
	VERSION |= ((SYS_SOFT_VERSIONS)%10);
	
	ModbusCmdRegister(0x0001,ONLY_WRITE,U16,1,24,				0,&(Modbus_List.Modbus_Register[++i]),ModBusSetPosition);
	ModbusCmdRegister(0x0002,ONLY_WRITE,U16,1,24,				0,&(Modbus_List.Modbus_Register[++i]),ModBusSetPositionDir0);
	ModbusCmdRegister(0x0003,ONLY_WRITE,U16,1,24,				0,&(Modbus_List.Modbus_Register[++i]),ModBusSetPositionDir1);
	ModbusCmdRegister(0x0005,ONLY_WRITE,U16,0,1000,				0,&(Modbus_List.Modbus_Register[++i]),ModBusReturnZero);
	ModbusCmdRegister(0x0006,ONLY_WRITE,U16,0,0xFFFF,			0,&(Modbus_List.Modbus_Register[++i]),ModBusMotorStop);
	ModbusCmdRegister(0x0007,ONLY_WRITE,U16,0,0xFFFF,			0,&(Modbus_List.Modbus_Register[++i]),ModBusClearError);
	ModbusCmdRegister(0x0051,WRITE_READ,U16,0,	2000,			(uint16_t*)&SysConfig.MotorValveConfig.MaxSpeed ,&(Modbus_List.Modbus_Register[++i]),ModBusGeneralCallback);
	ModbusCmdRegister(0x0052,WRITE_READ,U16,0,	2000,			(uint16_t*)&SysConfig.MotorValveConfig.MinSpeed,&(Modbus_List.Modbus_Register[++i]),ModBusGeneralCallback);
	ModbusCmdRegister(0x0053,WRITE_READ,U16,50,10000,			(uint16_t*)&SysConfig.MotorValveConfig.Acceleration,&(Modbus_List.Modbus_Register[++i]),ModBusGeneralCallback);
	ModbusCmdRegister(0x0054,WRITE_READ,U16,50,10000,			(uint16_t*)&SysConfig.MotorValveConfig.Deceleration,&(Modbus_List.Modbus_Register[++i]),ModBusGeneralCallback);
	ModbusCmdRegister(0x0055,WRITE_READ,U16,100,24000,		&SysConfig.MotorValveConfig.PhaseCurrent,&(Modbus_List.Modbus_Register[++i]),ModBusGeneralCallback);
	ModbusCmdRegister(0x0058,WRITE_READ,U8, 0,	3,				0,&(Modbus_List.Modbus_Register[++i]),ModBusGeneralCallback);
//	ModbusCmdRegister(0x005A,WRITE_READ,U8, 1,	24,				(uint16_t*)&SysConfig.ValveConfig.ResetPort,&(Modbus_List.Modbus_Register[++i]),ModBusGeneralCallback);
//	ModbusCmdRegister(0x0061,WRITE_READ,U8, 0,	100,			0,&(Modbus_List.Modbus_Register[++i]),ModBusGeneralCallback);
//	ModbusCmdRegister(0x006C,WRITE_READ,U16,0,50000,			(uint16_t*)&SysConfig.TotalVolume,&(Modbus_List.Modbus_Register[++i]),ModBusGeneralCallback);//
	ModbusCmdRegister(0x006D,WRITE_READ,S16,0,10000,			0,&(Modbus_List.Modbus_Register[++i]),ModBusGeneralCallback);
	ModbusCmdRegister(0x006E,WRITE_READ,S16,0,4,					0,&(Modbus_List.Modbus_Register[++i]),ModBusGeneralCallback);
	ModbusCmdRegister(0x006F,WRITE_READ,U8, 0,255,				(uint16_t*)&SysConfig.CommunicationConfig.Add,0,ModBusGeneralCallback);i++;
	ModbusCmdRegister(0x0090,ONLY_READ, U16,0,0xFFFF,			0,&(Modbus_List.Modbus_Register[++i]),ModBusGeneralCallback);
	ModbusCmdRegister(0x0091,ONLY_READ, U16,0,0xFFFF,			0,0,ModBusGeneralCallback);i++;
	ModbusCmdRegister(0x00EE,ONLY_WRITE,S16,0,0xFFFF,			0,0,ModBusRestoreFactory);i++;
	ModbusCmdRegister(0x00EF,ONLY_WRITE,S16,0,0xFFFF,			0,0,ModBusSetingSave);i++;
	ModbusCmdRegister(0x00F0,ONLY_READ,U16, 0,0xFFFF,			&UID,&(Modbus_List.Modbus_Register[++i]),ModBusGeneralCallback);
	ModbusCmdRegister(0x00F1,ONLY_READ,U16, 0,0xFFFF,			&PID,&(Modbus_List.Modbus_Register[++i]),ModBusGeneralCallback);
	ModbusCmdRegister(0x00F2,ONLY_READ,U16, 0,0xFFFF,			&VERSION,0,ModBusGeneralCallback);	
}
