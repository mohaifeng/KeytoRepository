#include "protocol.h"

#include "modbus.h"
#include "simulate_slave_I2C_driver.h"

uint8_t AssignTemporaryAddressFlag=0;           //分配临时地址标志
uint8_t AssignTemporaryAddressUpUIDFlag=0;      //接收到分配地址信息，上传本机UID数据标志
const uint8_t ConfigPassword[4]={0xFF,0xEE,0xBB,0xAA};
extern SysConfig_type  *pSysConfig;
uint8_t CommunicationMode=0;        //0:232,1:485,2:CAN
const uint8_t InputLoadMode[8]={0x1F,0xF1,0x1F,0xF1,0x1F,0xF1,0x1F,0xF1};
const uint8_t Scan_UID_CMD[8]={0x55,0xF1};                    //扫描UID
const uint8_t AssignTemporaryAddress_CMD[8]={0x55,0xF2};      //分配临时地址
extern  uint32_t Send_Scan_UID_TimeCont;
/*
函数功能：和校验
*/ 
uint16_t SumVerify16(uint8_t *data , uint8_t len)
{
  int i;
  uint16_t sum=0;
  for(i=0;i<len;i++)
  {
    sum+=data[i];
  }
  return sum;
}
/*
函数功能：和校验
*/ 
uint8_t SumVerify8(uint8_t *data , uint8_t len)
{
  int i;
  uint8_t sum=0;
  for(i=0;i<len;i++)
  {
    sum+=data[i];
  }
  return sum;
}


//获取UID
uint32_t HAL_GetUIDw0(void)
{
   return(READ_REG(*((uint32_t *)UID_BASE)));
}

uint8_t Bound_Change(uint32_t Bound)
{
	switch(Bound)
	{
		case 0:
			return 1;
		break;
		case 1:
			return 2;
		break;
		case 2:
			return 3;
		break;
		case 3:
			return 4;
		break;
		case 4:
			return 5;
		break;
	}
	return 1;
}


extern FDCAN_HandleTypeDef hfdcan1;
void reset_canFilter(void)
{
	FDCAN_FilterTypeDef  sFilterConfig;

	/*##-2- Configure the CAN Filter ###########################################*/
  sFilterConfig.IdType = FDCAN_STANDARD_ID;
  sFilterConfig.FilterIndex = 0;
  sFilterConfig.FilterType = FDCAN_FILTER_MASK;
  sFilterConfig.FilterConfig = FDCAN_FILTER_TO_RXFIFO0;
  sFilterConfig.FilterType = FDCAN_FILTER_MASK;
	#ifdef FOR_NCS_VERSION
	uint32_t ID = SysConfig.CommunicationConfig.Add;
	sFilterConfig.FilterID1= (ID)<<4; 
	#else
	sFilterConfig.FilterID1=0x0200+SysConfig.CommunicationConfig.Add;
	#endif	
  sFilterConfig.FilterID2=0x7FF;            
  if (HAL_FDCAN_ConfigFilter(&hfdcan1, &sFilterConfig) != HAL_OK)
  {
    /* Filter configuration Error */
    Error_Handler();
  }
	sFilterConfig.IdType = FDCAN_EXTENDED_ID;
  sFilterConfig.FilterIndex = 0;
  sFilterConfig.FilterType = FDCAN_FILTER_MASK;
  sFilterConfig.FilterConfig = FDCAN_FILTER_TO_RXFIFO0;
  sFilterConfig.FilterType = FDCAN_FILTER_MASK;
	sFilterConfig.FilterID1=0x05000000;					
  sFilterConfig.FilterID2=0xFF000000;             
  if (HAL_FDCAN_ConfigFilter(&hfdcan1, &sFilterConfig) != HAL_OK)
  {
    /* Filter configuration Error */
    Error_Handler();
  }
	//设置为只允许过滤列表ID通过
	HAL_FDCAN_ConfigGlobalFilter(&hfdcan1,FDCAN_REJECT, FDCAN_REJECT, FDCAN_REJECT_REMOTE, FDCAN_REJECT_REMOTE);
}

uint8_t SendSymbolFlag;
uint8_t stranalyse(char *src)
{
	SendSymbolFlag = 2;
	uint8_t count = 0;
	while(*src != '\0')
	{
		if(*src == '\n')
			SendSymbolFlag = 1;
		if(*src == '\r')
			SendSymbolFlag = 0;

		src++;
		count++;
		if(count>10)
			return 2;
	}
		return SendSymbolFlag;
}
void symboldel(char* src,char* tar)
{
	if(*src == 'P' || *src == '+' || *src == '-' || *src == 'X')
	{
		*tar = *src;
		tar++;
		src++;
	}
	else
	{
		strcpy(tar,src);
		return;
	}
	while(1)
	{
		if((*(src) >= '0'&& *(src) <= '9') || (*(src) >= 'A'&& *(src) <= 'F'))
		{
			*tar = *src;
			tar++;
		}
		
		src++;
		if(*src == '\0')
		{
			*tar = '\0';
			return;
		}
	}
}

/*
函数功能：解析协议
输入参数：Mode:0:232,1:485,2:CAN
          data:接收数据
          len:数据长度
返回值：  -1:帧头错误
          -2:帧尾错误
          -3:地址错误
          -4:校验错误
*/
int ResolutionProtocol(uint8_t Mode,uint8_t *data , uint8_t len)
{
  uint32_t Data32;
	uint8_t bootflag;
	char src[32],tar[32];
  if(memcmp(data,InputLoadMode,8)==0)
  {
		bootflag = Bound_Change(SysConfig.CommunicationConfig.RS232_BaudRate);
    Flash_WriteIntoBootLoadModeFlag(bootflag);
    //__set_FAULTMASK(1);   // 关闭所有中断
    NVIC_SystemReset();     // 复位
		return 0;
  }
	int i=0;
	while(i<len)
	{
		if(data[i] == 0xAA)
		{
			UART_ResolutionGeneral_Protocol(&data[i],8);
		}
		i++;
	}
	if(data[0] == SysConfig.CommunicationConfig.Add)
			ModBusAnalysis(data,len);
  //老化测试扫描分配临时ID
  else if(data[0]==0x55 &&( data[1]==0xF1 || data[1]==0xF2))
  {    
    Scan_Resolution(data,len);
  }
  else if(data[0]==0xCC)
  {
    ResolutionSmartSV_Protocol(Mode,data,len);
  }
#if USE_TITAN_PROTOCOL
  else if( ((data[0]>='A') && (data[0]<='Z') )|| (data[0]=='+') || (data[0]=='-') )    
  {
		memset(src,0,sizeof(src));
		memset(tar,0,sizeof(tar));
		if(len>10)
			return -1;
		memcpy(src,data,len);
		if(stranalyse(src)>1)
			return -1;
		symboldel(src,tar);
		ResolutionTiTan_Protocol(Mode,(uint8_t *)tar,strlen(tar));
    //ResolutionTiTan_Protocol(Mode,data,len);
  }
#endif
  return 0;
}
/*
函数功能：解析协议SmartSV协议
输入参数：Mode:0:232,1:485,2:CAN
          data:接收数据
          len:数据长度
返回值：  -1:帧头错误
          -2:帧尾错误
          -3:地址错误
          -4:校验错误
*/
int ResolutionSmartSV_Protocol(uint8_t Mode,uint8_t *data , uint8_t len)
{
  uint16_t CalculateVerify;
  ValveState_t MotorStatus;
  Protocol_Handle_t RxProtocol;
  uint8_t data8,i;
  uint16_t data16,RxData16;
	uint16_t temp16;
	uint8_t user_data[55];
  uint32_t data32;
  uint16_t Mask16,Match16;
  int ReturnStatus=-1;
  uint8_t SaveFlag=0;
  RxProtocol.Head=data[0];
  RxProtocol.Add=data[1];
  RxProtocol.Cmd=data[2];
//  //工厂设置指令
  if(RxProtocol.Cmd<=0x10 || RxProtocol.Cmd ==0XFC || RxProtocol.Cmd ==0X19)
  {
    memcpy(RxProtocol.code,&data[3],4);
    memcpy(RxProtocol.data,&data[7],4);
    RxProtocol.end=data[11];
    RxProtocol.verify=((uint16_t)data[13]<<8)|data[12];
    CalculateVerify=SumVerify16(data,12);
    if(memcmp(RxProtocol.code,ConfigPassword,4)!=0)
      ReturnStatus=-2;  
  }
	else if(RxProtocol.Cmd == 0x81)
	{
		memcpy(user_data,&data[3],50);
		RxProtocol.end=data[53];
		CalculateVerify=SumVerify16(data,54);
		RxProtocol.verify=((uint16_t)data[55]<<8)|data[54];
	}
  else
  {
    memcpy(RxProtocol.data,&data[3],2);   
    RxProtocol.end=data[5];
    RxProtocol.verify=((uint16_t)data[7]<<8)|data[6];
    CalculateVerify=SumVerify16(data,6);
  } 
  if(RxProtocol.Head!=0xCC)
  {
    ReturnStatus=-2;   
  }
  if(RxProtocol.end!=0xDD)
  {
    ReturnStatus=-2; 
  }
  if(RxProtocol.Add!=pSysConfig->CommunicationConfig.Add)
  {
    ReturnStatus=-2; 
  }
  if(CalculateVerify!=RxProtocol.verify)
  {
    ReturnStatus=-2; 
  }
  if(ReturnStatus==-1)
  {
    switch(RxProtocol.Cmd)
    {   
      case 0x44:                  //电机通过码盘转动，自动选择最优路径
      case 0x46:
      case 0x47:
        MotorStatus=Valve_GetFaultStatus();
        if(MotorStatus==VALVE_FAULT||MotorStatus==VALVE_FAULT_IDLE)
        {
          data8=Valve_GetFaultFlag();
          if(((data8&0x01)==0x01)|| ((data8&0x03)==0x03))           //光耦错误
            ReturnStatus=3;
          else if((data8&0x02)==0x02)        //堵转错误
            ReturnStatus=5;  
          else if(((data8&0x05)==0x05)|| ((data8&0x06)==0x06) || ((data8&0x07)==0x07))        //位置错误
            ReturnStatus=6;           
          else 
            ReturnStatus=0xff;                              //电机错误
        }
        else if(MotorStatus!=VALVE_IDLE)
          ReturnStatus=4;           //电机忙
        else
        {
          data8=RxProtocol.data[0];
          if(data8>=1&&data8<=pSysConfig->ValveConfig.PortNum)
          {
            if(RxProtocol.Cmd==0x44)				
								Valve_SetPort(data8);
            else if(RxProtocol.Cmd==0x46)
              Valve_SetPortDir(0,data8);
            else if(RxProtocol.Cmd==0x47)
              Valve_SetPortDir(1,data8);
            if(GetUartNum()==1)
              ReturnStatus=0xfe;
            else             
              ReturnStatus=0;
          }
          else
            ReturnStatus=0x02;  
        }
        break;
      case 0x45:                  //复位
        Valve_AutoZeroMode();
        if(GetUartNum()==1)
          ReturnStatus=0xfe;
        else             
          ReturnStatus=0;
        break;
      case 0x49:                  //强停
        Valve_ImmediatelyStopMotor();
        ReturnStatus=0;
        break;
      case 0x4A:                  //查询电机状态
        MotorStatus=Valve_GetFaultStatus();
        if(MotorStatus==VALVE_IDLE)
          ReturnStatus=0;
        else if(MotorStatus==VALVE_FAULT||MotorStatus==VALVE_FAULT_IDLE)
        {
          data8=Valve_GetFaultFlag();
          if(((data8&0x01)==0x01)|| ((data8&0x03)==0x03))              //光耦错误
            ReturnStatus=3;
          else if((data8&0x02)==0x02)        //堵转错误
            ReturnStatus=5;  
          else if(((data8&0x05)==0x05) || ((data8&0x06)==0x06) || ((data8&0x07)==0x07) )        //位置错误
            ReturnStatus=6;          
          else 
            ReturnStatus=0xff;                              //电机错误
        }
        else
          ReturnStatus=4;           //电机忙
        break;
      case 0x3e :                  //查询当前通道位置     
        ResponseSmartSV_Send(0,Valve_GetCurrent_Channel());
        break;
      case 0x00:                   //设置地址
        data8=RxProtocol.data[0];
        SetProtocolAdd(data8);  
        ReturnStatus=0;
        SaveFlag=1;      
        break;
      case 0x01:                   //设置232波特率
        data8=RxProtocol.data[0];
        SetRS232BaudRate(data8); 
        ReturnStatus=0;
        SaveFlag=1;
        break;
      case 0x02:                  //设置485波特率
        data8=RxProtocol.data[0];
        if(data8<=4)
        {
          SetRS485BaudRate(data8);
          ReturnStatus=0;
          SaveFlag=1;
        }
        else
          ReturnStatus=0x02;         
        break;
      case 0x07:              //设置最大转速
        data16=(RxProtocol.data[0])|((uint16_t)RxProtocol.data[1]<<8);
        if(data16>=5&&data16<=600)
        {
					pSysConfig->MotorValveConfig.MaxSpeed = (data16/60)*200*pSysConfig->MotorValveConfig.MicroStep;
          pSysConfig->MotorValveConfig.RunSpeed = (data16/60)*200*pSysConfig->MotorValveConfig.MicroStep;
          ReturnStatus=0;
          SaveFlag=1;
        }
        else
          ReturnStatus=0x02;       
        break;
      case 0x0A:              //设置码盘一圈计数
        if(RxProtocol.data[0]>1)
        {
          pSysConfig->ValveConfig.PortNum=RxProtocol.data[0];
					Set_Multi_ReductionBox_PortAndOptPosition(pSysConfig->ValveConfig.PortNum);
          ReturnStatus=0;
          SaveFlag=1;
        }
        else
          ReturnStatus=0x02; 
        break;
      case 0x0B:              //设置复位速度
        data16=(RxProtocol.data[0])|((uint16_t)RxProtocol.data[1]<<8);
        if(data16>=5&&data16<=600)
        {
          pSysConfig->ValveConfig.ResetSpeed=(data16/60)*200;
          ReturnStatus=0;
          SaveFlag=1;
        }
        else
          ReturnStatus=0x02;       
        break;
      case 0x0C:              //设置复位方向
        if(RxProtocol.data[0]<=1)
        {
          pSysConfig->MotorValveConfig.PositionDir=RxProtocol.data[0];
          ReturnStatus=0;
          SaveFlag=1;
        }
        else
          ReturnStatus=0x02;  
        break;
      case 0x0E:              //设置上电自动复位
        if(RxProtocol.data[0]<=1)
        {
          pSysConfig->AutoResetZeroEn=RxProtocol.data[0];
          ReturnStatus=0;
          SaveFlag=1;
        }
        else
          ReturnStatus=0x02;  
        break;
      case 0x0F:              //设置光缝大小
        if(RxProtocol.data[0]>=1)
        {
					SysConfig.ValveConfig.FWD_ValveOptocouplerGapStep=RxProtocol.data[0]*pSysConfig->MotorValveConfig.MicroStep;
          ReturnStatus=0;
          SaveFlag=1;
        }
        else
          ReturnStatus=0x02;  
        break;    
      case 0x20 :             //查询地址    
        ResponseSmartSV_Send(0,pSysConfig->CommunicationConfig.Add);
        ReturnStatus=-1;
        break;
      case 0x21 :             //查询RS232波特率    
        ResponseSmartSV_Send(0,pSysConfig->CommunicationConfig.RS232_BaudRate);
        ReturnStatus=-1;
        break;
      case 0x22 :             //查询RS485波特率    
        ResponseSmartSV_Send(0,pSysConfig->CommunicationConfig.RS485_BaudRate);
        ReturnStatus=-1;
        break;
			case 0x23 :             //查询CAN波特率    
        ResponseSmartSV_Send(0,500);
        ReturnStatus=-1;
        break;
      case 0x27 :             //查询最大转速    
        ResponseSmartSV_Send(0,(pSysConfig->MotorValveConfig.RunSpeed/pSysConfig->MotorValveConfig.MicroStep*60)/200);
        ReturnStatus=-1;
        break;
      case 0x2A :             //查询码盘一圈计数  
        ResponseSmartSV_Send(0,pSysConfig->ValveConfig.PortNum);
        ReturnStatus=-1;
        break;
      case 0x2B :             //查询复位速度
        ResponseSmartSV_Send(0,(pSysConfig->ValveConfig.ResetSpeed*60)/200);
        ReturnStatus=-1;
        break;
      case 0x2C :             //查询复位方向
        ResponseSmartSV_Send(0,pSysConfig->MotorValveConfig.PositionDir);
        ReturnStatus=-1;
        break;
      case 0x2E :             //查询上电复位
        ResponseSmartSV_Send(0,pSysConfig->AutoResetZeroEn);
        ReturnStatus=-1;
        break;
      case 0x2F :             //查询光缝大小
        ResponseSmartSV_Send(0,SysConfig.ValveConfig.FWD_ValveOptocouplerGapStep/pSysConfig->MotorValveConfig.MicroStep);
        ReturnStatus=-1;
        break;
			case 0x30 :             //查询CAN目标地址
        ResponseSmartSV_Send(0,SysConfig.CommunicationConfig.CAN_TargetAdd);
        ReturnStatus=-1;
        break;
			case 0x3F :             //查询版本号
				temp16 = SYS_SOFT_VERSIONS%1000;
        ResponseSmartSV_Send(0,temp16);
        ReturnStatus=-1;
        break;
      case 0xEE :             //查询芯片序号
        data32=HAL_GetUIDw0();
        data16=((data32>>8)&0xff00)|(data32&0xff);
        ResponseSmartSV_Send(0,data16);
        ReturnStatus=-1;
        break;
      case 0x19 :             //设置密码
        data16=(RxProtocol.data[0])|((uint16_t)RxProtocol.data[1]<<8);
        pSysConfig->Password=data16;
        ReturnStatus=0;
        break;       
      case 0x39 :             //查询密码
        data32=pSysConfig->Password;
        ResponseSmartSV_Send(0,data32);
        ReturnStatus=-1;
        break;
			case 0x80:
				ResponseSmartSV_UserData_Send(0);
				ReturnStatus=-1;
			break;
			case 0x81:
				memcpy(SysConfig.UserData,user_data,50);
				ReturnStatus=0;
			break;
      case 0xfc :             //加密数据保存
        WriteSysConfigToFlash(pSysConfig);
        ReturnStatus=0;
        break;
      default:
        ReturnStatus=-1;
      break;
    }
  }
  if(SaveFlag==1)
  {
    WriteSysConfigToFlash(pSysConfig);
  }
  if(ReturnStatus>=0)
  {
    ResponseSmartSV_Send(ReturnStatus,0);
    return ReturnStatus;
  }
  return 0;
}
void ResponseSmartSV_Send(uint8_t status , uint16_t data)
{
  uint16_t CalculateVerify;
  uint8_t sendData[8];
  sendData[0]=0xCC;
  sendData[1]=pSysConfig->CommunicationConfig.Add;
  sendData[2]=status;
  sendData[3]=data&0xFF;
  sendData[4]=data>>8;
  sendData[5]=0xDD;
  CalculateVerify=SumVerify16(sendData,6);
  sendData[6]=CalculateVerify&0xff;
  sendData[7]=CalculateVerify>>8;
  SendCommunicationData(CommunicationMode,sendData,8);
}

uint32_t StringToHex32(char *str)
{
  uint32_t Data=0;
  char *p = str;             //直针p初始化为指向str
  uint8_t len = strlen(str); //获取字符串中的字符个数	
  while(*p != '\0') {        //结束符判断
    if ((*p >= '0') && (*p <= '9')) //当前字符为数字0~9时
      Data=(Data<<4)|(*p - '0');			
    else if ((*p >= 'A') && (*p <= 'Z')) //当前字符为大写字母A~Z时
        Data=(Data<<4)|(*p - 'A' + 10);			
    else if ((*p >= 'a') && (*p <= 'z')) //当前字符为小写字母a~z时
        Data=(Data<<4)|(*p - 'a' + 10);
    else
      return Data;
    p++;    //指向下一个字符
  }
  return Data;
}
/*
函数功能：解析协议TiTan协议
输入参数：Mode:0:232,1:485,2:CAN
          data:接收数据
          len:数据长度
返回值：  -1:帧头错误
          -2:帧尾错误
          -3:地址错误
          -4:校验错误
*/
uint8_t SendTitanFlag = 0;
int ResolutionTiTan_Protocol(uint8_t Mode,uint8_t *data , uint8_t len)
{
  uint8_t Status=0;
  char Cmd;
  char TmpStr[10];
  uint8_t Data8,i;
	int Data32;
  Cmd=data[0]; 
  Status=CheckSysStatus();
  if(Status==1)
  {
		if(SendSymbolFlag)
		{
			ResponseTiTan_Send("*",1);
		}
    ResponseTiTan_Send("**\r",3);
    return 0;
  }
  else if(Status==2)
  {
		if(Cmd == 'S' || Cmd == 'E')
		{
			Data8 = Valve_GetFaultFlag();
		  if(Data8 == 0x01)
			  Data8 = 0x63;
		  else
			  Data8 = 0x42;
			if(NonVolatileMemory)
				Data8 = 0x58;
			memset(TmpStr,0,sizeof(TmpStr));	
			sprintf(TmpStr,"%02X\r",Data8);
			ResponseTiTan_Send(TmpStr,3);
		}
		else if(Cmd == 'M')
		{
			ResponseTiTan_Send("--\r",3);
		}
		return 0;
  } 
  else
  switch(Cmd)
  {   
    case '+':
    case '-':
    case 'P':                  //电机通过码盘转动，自动选择最优路径   
      Data8=StringToHex32((char *)(&data[1]));
      if(Data8>=DEFAULT_FLOW_MIN_POSITION&&Data8<=pSysConfig->ValveConfig.PortNum)
      {
        if(Cmd=='P')
          Valve_SetPort(Data8);
        else if(Cmd=='+')
          Valve_SetPortDir(0,Data8);
        else 
          Valve_SetPortDir(1,Data8);
        if(SendSymbolFlag)
				{
					ResponseTiTan_Send("*",1);
				}
      }
      else
      {
        ResponseTiTan_Send("++\r",3);
      }
    break;
   case 'S':                  //查询当前位置
		memset(TmpStr,0,sizeof(TmpStr));
		Data8=Valve_GetCurrent_Channel();
    sprintf(TmpStr,"%02X\r",Data8);
    ResponseTiTan_Send(TmpStr,3);  
    break;
	 case 'E':                  //查询当前状态
		memset(TmpStr,0,sizeof(TmpStr));
		Data8=0;
    sprintf(TmpStr,"%02X\r",Data8);
    ResponseTiTan_Send(TmpStr,3);  
    break;
   case 'N':                   //设置地址
      
    break;
   case 'M':                  //复位
			Data32=atoi((char *)(&data[1]));
			if(Data32>=200&&Data32<=4000)
				pSysConfig->ValveConfig.ResetSpeed=Data32;
			else
				pSysConfig->ValveConfig.ResetSpeed=(pSysConfig->ValveConfig.MaxStep/2);
      Valve_AutoZeroMode();
			if(SendSymbolFlag)
			{
				ResponseTiTan_Send("*",1);
			}
    break;
   case 'X':
		 Data8=StringToHex32((char *)(&data[1]));
     if(Data8>=1&&Data8<=4)
     {
       SetRS232BaudRate(Data8-1);    //设置232波特率
       SetRS485BaudRate(Data8-1);  //设置485波特率
			 WriteSysConfigToFlash(pSysConfig);
			 ResponseTiTan_Send("\r",1);
     }
     break;
	 case 'Q':
		 Data8 = 0x01;
		 sprintf(TmpStr,"%02X\r",Data8);
     ResponseTiTan_Send(TmpStr,3);
	 break;
//   case 'Y':
//     Data8=StringToHex32((char *)(&data[1]));
//     if(Data8>1)
//     {
//			 pSysConfig->ValveConfig.PortNum = Data8;
//			 Set_Multi_ReductionBox_PortAndOptPosition(pSysConfig->ValveConfig.PortNum);
//       WriteSysConfigToFlash(pSysConfig);
//       ResponseTiTan_Send("\r",1);
//     }
//		 break;
	 case 'R':
		 ResponseTiTan_Send("107\r",4);
	 break;
    default :

    break;
  }
	SendSymbolFlag = 0;
	SendTitanFlag = 1;
  return 0;
}

void TiTanResponse(void)
{
  char TmpStr[10];
  uint8_t Data8,i;
  Data8=Valve_GetCurrent_Channel();
  memset(TmpStr,0,sizeof(TmpStr));
  sprintf(TmpStr,"%02X\r",Data8);
  ResponseTiTan_Send(TmpStr,3);
}


/*
函数功能：检测系统状态
输出参数：0：空闲
          1：忙
          2：故障
*/
uint8_t CheckSysStatus(void)
{
  ValveState_t MotorStatus;
  MotorStatus=Valve_GetFaultStatus();
  if(MotorStatus==VALVE_IDLE)
    return 0;
  else if(MotorStatus==VALVE_FAULT||MotorStatus==VALVE_FAULT_IDLE)
  {
    return 2;
  }
  else
    return 1;           //电机忙
}
void ResponseTiTan_Send(char *Data,uint8_t len)
{
  uint8_t sendData[8];
  memset(sendData,0,sizeof(sendData));
  memcpy(sendData,Data,len);
  SendCommunicationData(CommunicationMode,sendData,len);
}
/*
函数功能：处理通用协议指令
输入参数：cmd：命令
          Value：参数值
返回值：  返回参数
*/
int ResolutionGeneral_Cmd(uint8_t cmd , int32_t Value)
{
	int ReturnValue=0,i;
	uint16_t Status;
	uint16_t ErrorStatus;
	
	int16_t microMode;
	int16_t compensationValue;
		
	switch(cmd)
	{
      //切换到指定通道
      case 0x01:	
      case 0x02:
      case 0x03:
          Status = Valve_GetStatus();
          if(Status==0)
          {
            if(Value>=1&&Value<=pSysConfig->ValveConfig.PortNum)
            {
              if(cmd==0x01)
								Valve_SetPort(Value);
              else if(cmd==0x02)
                Valve_SetPortDir(0,Value);
              else if(cmd==0x03)
                Valve_SetPortDir(1,Value);
              ReturnValue=0;
            }
            else
              ReturnValue=0x04;
          }
          else
          {
            ReturnValue=Status;
          }
      break;
//      //置零
      case 0x05:
          Valve_AutoZeroMode();
          ReturnValue=0;
      break;
      //停止
      case 0x06:	
          Valve_ImmediatelyStopMotor();
          ReturnValue=0;
      break;
//      //清除故障信息
      case 0x07:
          Value_ClearFault();
          ReturnValue=0;
      break;
			case 0x09://新旧10通转换是否开启
				if(Value<=1)
				{
					pSysConfig->Not_Change = Value;
					ReturnValue = 0	;
				}
				else
					ReturnValue = 1;
			break;
//      //设置参数指令
      case 0x51:				 //最大速度
          if(Value>=50&&Value<=10000)
          {
              SysConfig.MotorValveConfig.MaxSpeed=Value*SysConfig.MotorValveConfig.MicroStep;
              SysConfig.MotorValveConfig.RunSpeed=Value*SysConfig.MotorValveConfig.MicroStep;
              ReturnValue=0;
          }
          else
          {
              ReturnValue=1;
          }
      break;
      case 0x52:				 //设置最小速度
          if(Value>=0&&Value<=5000)
          {
              SysConfig.MotorValveConfig.MinSpeed=Value*SysConfig.MotorValveConfig.MicroStep;
							Motor_SetVSTOP(SysConfig.MotorValveConfig.MinSpeed);
              ReturnValue=0;
          }
          else
              ReturnValue=1;
      break;
      case 0x53:				 //设置加速度系数
          if(Value>=50&&Value<=400000)
          {
						SysConfig.MotorValveConfig.Acceleration=(Value*SysConfig.MotorValveConfig.MicroStep);
						TMC5160_SPIWriteInt(0x24, 	ConvertACC_ustepss_To_usteptt(SysConfig.MotorValveConfig.Acceleration));
						TMC5160_SPIWriteInt(0x26, 	ConvertACC_ustepss_To_usteptt(SysConfig.MotorValveConfig.Acceleration));
						ReturnValue=0;
          }
          else
          {
						ReturnValue=1;
          }
      break;
      case 0x54:				 //设置减速度系数
          if(Value>=50&&Value<=400000)
          {
						SysConfig.MotorValveConfig.Deceleration=(Value*SysConfig.MotorValveConfig.MicroStep);
						TMC5160_SPIWriteInt(0x28, 	ConvertACC_ustepss_To_usteptt(SysConfig.MotorValveConfig.Deceleration));					//DMAX VMAX-V1 700
						TMC5160_SPIWriteInt(0x2A, 	ConvertACC_ustepss_To_usteptt(SysConfig.MotorValveConfig.Deceleration));
						ReturnValue=0;
          }
          else
          {
						ReturnValue=1;
          }
      break;
      case 0x55:				 //设置相电流
          if(Value>=100&&Value<=2350)
          {
						SysConfig.MotorValveConfig.PhaseCurrent=Value;
						Motor_SetPhaseCurrent(Value);
						ReturnValue=0;
          }
          else
          {
						ReturnValue=1;
          }
      break;      
      case 0x58:              //设置通道个数   
          if(Value>=1&&Value<=30)
          {
						setValueChannel(Value);			
            ReturnValue=0;
          }
          else
          {
            ReturnValue=1;
          }
      break;
			case 0x59:					//设置复位通道
				if(Value == 0 || Value > SysConfig.ValveConfig.PortNum)
					ReturnValue=1;
				else
				{
					SysConfig.ValveConfig.ResetPort = Value;
					ReturnValue=0;
				}			
			break;
			case 0x5A:					//查询复位通道
				ReturnValue = SysConfig.ValveConfig.ResetPort;
			break;
			case 0x5B:					//设置复位速度
				if(Value >= 50 && Value <= 4000)
				{
					pSysConfig->ValveConfig.ResetSpeed=Value;
					ReturnValue=0;
				}
				else
				{
					ReturnValue=1;
				}	
			break;
			case 0x5C:					//查询复位速度
				ReturnValue = pSysConfig->ValveConfig.ResetSpeed;
			break;
			case 0x5D:
				if(Value)
					pSysConfig->AutoResetZeroEn = 1;
				else
					pSysConfig->AutoResetZeroEn = 0;
				ReturnValue = 0;
			break;
      case 0x61:					//设置光缝宽度大小 
          SysConfig.ValveConfig.FWD_ValveOptocouplerGapStep=Value*pSysConfig->MotorValveConfig.MicroStep;
          ReturnValue=0;
      break;
			case 0x62://最大行程
				SysConfig.ValveConfig.MaxStep=Value;
				SysConfig.ValveConfig.Reduction_Ratio = 0;
        ReturnValue=0;
				break;
			case 0x63://设置光耦滤波步数
				SysConfig.ValveConfig.MaxGapStep=Value;
				break;			
			case 0x64://是否使用减速箱
				if(Value<3)
				{
					SysConfig.ValveConfig.Using_ReductionBox = Value;
          ReturnValue = 0;
				}
				else
					ReturnValue = 1;
      break;
      case 0x6E:				 //设置波特率
				if(Value==9600)
				{
					SysConfig.CommunicationConfig.RS232_BaudRate = 0;
					SysConfig.CommunicationConfig.RS485_BaudRate = 0;
					ReturnValue=0;
				}
				else if(Value==19200)
				{
					SysConfig.CommunicationConfig.RS232_BaudRate = 1;
					SysConfig.CommunicationConfig.RS485_BaudRate = 1;
					ReturnValue=0;
				}
				else if(Value==38400)
				{
					SysConfig.CommunicationConfig.RS232_BaudRate = 2;
					SysConfig.CommunicationConfig.RS485_BaudRate = 2;
					ReturnValue=0;
				}
				else if(Value==57600)
				{
					SysConfig.CommunicationConfig.RS232_BaudRate = 3;
					SysConfig.CommunicationConfig.RS485_BaudRate = 3;
					ReturnValue=0;
				}
				else if(Value==115200)
				{
					SysConfig.CommunicationConfig.RS232_BaudRate = 4;
					SysConfig.CommunicationConfig.RS485_BaudRate = 4;
					ReturnValue=0;
				}
				else
				{
					ReturnValue=1;
				}
      break;
      case 0x6D:				 //设置CAN波特率
          if(Value==100 || Value==125 || Value==250 || Value==500 || Value==800 || Value==1000)
          {
						SysConfig.CommunicationConfig.CAN_BaudRate=Value;
						ReturnValue=0;
          }
          else
          {
						ReturnValue=1;
          }
      break;
      case 0x6F:					//设置ID
					#ifdef FOR_NCS_VERSION
					ReturnValue=1;
					if(Value<=0x0F && Value>=0)
					{
						SysConfig.CommunicationConfig.Add = (Value&0x0F);
						reset_canFilter();
						ReturnValue = 0;
					}
					#else
          if(Value<=0xff && Value>=0)
          {
						SysConfig.CommunicationConfig.Add = Value;
						SysConfig.IIC_Addr = (uint8_t)Value * 2 + SI2C_INITADDR;	//通过设备地址来改写I2C地址
						reset_canFilter();
						ReturnValue=0;
          }
          else
						ReturnValue=1;
          #endif
      break;
			case 0x70:					//设置IIC地址
				if(Value<=0xff && Value>=0)
				{
					SysConfig.IIC_Addr=Value & 0xFE;
					ReturnValue = 0;
				}
				else
				{
					ReturnValue = 1;
				}
      break;
			case 0x9F:					//查询IIC地址
				ReturnValue = SysConfig.IIC_Addr;
				ReturnValue = 0;
			break;
			case 0x7F:
				SysConfig.Password = Value;
				ReturnValue = 0;
			break;
      case 0xEF:					//掉电保存
				if(Value==123456)
				{
					if(WriteSysConfigToFlash(&SysConfig)!=0)
						ReturnValue=1;
					else					
						ReturnValue=0;
				}
				else
				{
					ReturnValue=1;
				}
      break;
      case 0xEE:					//恢复出厂设置
				if(Value==123456)
				{
					SysConfig.InitFlag=0;  
					if(WriteSysConfigToFlash(&SysConfig)!=0)
						ReturnValue=1;
					else					
						ReturnValue=0;
				}
				else
				{
					ReturnValue=1;
				}
      break;
			case 0xEA:
				if(Value>=1)
					SysConfig.AutoAgingEnable = 1;
				else
					SysConfig.AutoAgingEnable = 0;
				ReturnValue=0;
			break;
			case 0xEB:
				ReturnValue = SysConfig.AutoAgingEnable;
			break;
			case 0x80://正向补偿(逆时针)
				if(abs(Value) < SysConfig.ValveConfig.OptInterval)
				{
					SysConfig.ValveConfig.FlowAnticlockwisePhaseCompensation = (int)Value;
					ReturnValue = 0;
				}
				else
					ReturnValue=0;
			break;
			case 0x81://反向补偿(顺时针)
				if(abs(Value) < SysConfig.ValveConfig.OptInterval)
				{
					SysConfig.ValveConfig.FlowClockwisePhaseCompensation = (int)Value;
					ReturnValue = 0;
				}
				else
					ReturnValue=0;
			break;
			case 0x88://读正向补偿(逆时针)
				ReturnValue = SysConfig.ValveConfig.FlowAnticlockwisePhaseCompensation;
			break;
			case 0x89://读反向补偿(逆时针)
				ReturnValue = SysConfig.ValveConfig.FlowClockwisePhaseCompensation;
			break;
      case 0x90:				 //查询状态		
				ReturnValue=0;
				Status = Valve_GetStatus();
				if(Status&0xff00)
				{
					ReturnValue=Status;						
				}
				else
				{
					if(Status&0x01)
						ReturnValue=1;
					else
						ReturnValue=0;
				}			
      break;
      case 0x91:				 //最大速度	
          ReturnValue=SysConfig.MotorValveConfig.RunSpeed/pSysConfig->MotorValveConfig.MicroStep;	
      break;
      case 0x92:				 //最小速度
          ReturnValue=SysConfig.MotorValveConfig.MinSpeed/pSysConfig->MotorValveConfig.MicroStep;	
      break;
      case 0x93:				 //加速度系数
          ReturnValue=SysConfig.MotorValveConfig.Acceleration/pSysConfig->MotorValveConfig.MicroStep;
      break;
      case 0x94:				 //减速度系数
          ReturnValue=SysConfig.MotorValveConfig.Deceleration/pSysConfig->MotorValveConfig.MicroStep;
      break;
      case 0x95:				 //相电流
          ReturnValue=SysConfig.MotorValveConfig.PhaseCurrent;
      break;
      case 0x98:					//通道最大数
          ReturnValue=SysConfig.ValveConfig.PortNum;
      break;		
      case 0x99:					//当前通道位置 
          ReturnValue=Valve_GetCurrent_Channel();
      break;
			case 0x9D:					//上电自动复为标志
          ReturnValue=SysConfig.AutoResetZeroEn;
      break;
			case 0xA1:
				ReturnValue = SysConfig.ValveConfig.FWD_ValveOptocouplerGapStep/SysConfig.MotorValveConfig.MicroStep;
			break;
			case 0xA2:
				ReturnValue = SysConfig.ValveConfig.MaxStep;
			break;
			case 0xA3:
				ReturnValue = SysConfig.ValveConfig.MaxGapStep;
			break;
			case 0xA4:
				ReturnValue = SysConfig.ValveConfig.Using_ReductionBox;
			break;
			case 0xA8://减速比
				ReturnValue = (uint32_t)(SysConfig.ValveConfig.Reduction_Ratio);
      break;
			case 0xA9:					//老10通道转换
          ReturnValue=pSysConfig->Not_Change;
      break;
			case 0xAD://读取CAN波特率
				ReturnValue = SysConfig.CommunicationConfig.CAN_BaudRate;
			break;
			case 0xAE://读取串口波特率
				if(SysConfig.CommunicationConfig.RS232_BaudRate==0)
					ReturnValue=9600;
				else if(SysConfig.CommunicationConfig.RS232_BaudRate==1)
					ReturnValue=19200;
				else if(SysConfig.CommunicationConfig.RS232_BaudRate==2)
					ReturnValue=38400;
				else if(SysConfig.CommunicationConfig.RS232_BaudRate==3)
					ReturnValue=57600;
				else if(SysConfig.CommunicationConfig.RS232_BaudRate == 4)
					ReturnValue=115200;
			break;
			case 0xAF:
				ReturnValue = SysConfig.Password;
			break;
      case 0xF0:					//设备ID 
          ReturnValue=HAL_GetUIDw0();
      break;
      case 0xF1:			//查询设备型号编码 
          ReturnValue=SYS_FACILITY_ID;
      break;
      case 0xF2:			//查询软件版本号 
          ReturnValue=SYS_SOFT_VERSIONS;
      break;
      default :return -5;
  }
  return ReturnValue;
}
/*
函数功能：通用协议解析协议
输入参数：Mode:0:232,1:485,2:CAN
          data:接收数据
          len:数据长度
返回值：  -1:帧头错误
          -2:地址错误
          -3:校验错误
					-4:长度错误
					-5：命令错误
*/
int UART_ResolutionGeneral_Protocol(uint8_t *data , uint8_t len)
{
  uint8_t sum,cyclic;
  int Value,ReturnValue=0;
  GeneralProtocol_Handle_t protocol;
	
  serial_t temp;
	
  protocol.Head=data[0];
  if(protocol.Head!=0xaa)
      return -1;
  protocol.Add=data[1];
  if(protocol.Add!=SysConfig.CommunicationConfig.Add)
      return -2;
  protocol.cmd=data[2];
  memcpy(protocol.data,&(data[3]),4);
  Value=((uint32_t)(protocol.data[0])<<24)|((uint32_t)(protocol.data[1])<<16)|((uint32_t)(protocol.data[2])<<8)|((uint32_t)(protocol.data[3]));
  protocol.verify=data[7];
  sum=SumVerify8(data,7);
  if(sum!=protocol.verify)
      return -3;
	
	temp.cmd = protocol.cmd;
	temp.value = Value;
	cmd_list_append(temp);
	
  return 0;
}
void UART_ReturnGeneralData(uint8_t add,int data)
{
	uint8_t send_data[8];
	send_data[0]=0xaa;
	send_data[1]=add;
	send_data[2]=(data>>24)&0xff;
	send_data[3]=(data>>16)&0xff;
	send_data[4]=(data>>8)&0xff;
	send_data[5]=data&0xff;
	send_data[6]=SumVerify8(send_data,6);
	SendCommunicationData(CommunicationMode,send_data,7);
}
//设置通讯协议地址
void SetProtocolAdd(uint8_t add)
{
  pSysConfig->CommunicationConfig.Add=add;
}
//设置RS232波特率
void SetRS232BaudRate(uint8_t baud)
{
  if(baud<=4)
    pSysConfig->CommunicationConfig.RS232_BaudRate=baud;
}
//设置RS485波特率
void SetRS485BaudRate(uint8_t baud)
{
  if(baud<=4)
    pSysConfig->CommunicationConfig.RS485_BaudRate=baud;
}
void Scan_Resolution(uint8_t *data , uint8_t len)
{
  uint32_t Data32,uid;
  uint8_t CalculateVerify;
  if(memcmp(data,Scan_UID_CMD,2)==0)
  {    
    CalculateVerify=SumVerify16(data,6);
    if(CalculateVerify==data[6] && data[7]==0xAA && AssignTemporaryAddressFlag==0)
    {
      Data32=(((uint32_t)data[2])<<24)|(((uint32_t)data[3])<<16)|(((uint32_t)data[4])<<8)|(uint32_t)data[5];     
      uid=HAL_GetDEVID();
      //匹配数据为加随机数延时
      //SoftDelay(((Data32&uid)+rand()%10)*10000);
      AssignTemporaryAddressUpUIDFlag=1;
      //Send_Scan_UID_TimeCont=((Data32&uid)%200)+rand()%10;
    }
  }
  else if(memcmp(data,AssignTemporaryAddress_CMD,2)==0)
  {
    CalculateVerify=SumVerify16(data,7);
    if(CalculateVerify==data[7] && data[8]==0xAA )
    {
      Data32=(((uint32_t)data[2])<<24)|(((uint32_t)data[3])<<16)|(((uint32_t)data[4])<<8)|(uint32_t)data[5];     
      uid=HAL_GetUIDw0();
      if(uid==Data32)
      {
         SetProtocolAdd(data[6]);
         AssignTemporaryAddressFlag=1;
      }
    }
  }
}
void ResponseScanUID_Send(void)
{
  uint8_t CalculateVerify;
  uint32_t uid;
  uid=HAL_GetDEVID();
  uint8_t sendData[8];
  sendData[0]=0x55;
  sendData[1]=0XF0;
  sendData[2]=(uid>>24)&0xff;
  sendData[3]=(uid>>16)&0xff;
  sendData[4]=(uid>>8)&0xff;
  sendData[5]=uid&0xff;
  CalculateVerify=SumVerify16(sendData,6);
  sendData[6]=CalculateVerify&0xff;
  sendData[7]=0XAA; 
  SendCommunicationData(CommunicationMode,sendData,8);
}

void can_delay(uint32_t d)
{
	int i;
	while(d--)
		for(i=0;i<1000;i++)
			__nop();
}

/*启动bootloader*/                
void startBootloader(uint32_t id,uint8_t *data)
{
	if((id&0x1FFFFFFF) == 0x05FFFFFF)
	{
		uint32_t data1 = (data[0]<<24)|(data[1]<<16)|(data[2]<<8)|data[3];
		uint32_t data2 = (data[4]<<24)|(data[5]<<16)|(data[6]<<8)|data[7];
		switch(data1)
		{
			case 0x01000000 ://接收板卡型号
			if(data2 == SYS_FACILITY_ID)
			{
				uint8_t i;
				switch(SysConfig.CommunicationConfig.RS485_BaudRate)
				{
					case 0:	i = 1;break;
					case 1:	i = 2;break;
					case 2:	i = 3;break;
					case 4:	i = 4;break;
					default :		i = 1;break;
				}
				switch(SysConfig.CommunicationConfig.CAN_BaudRate)
				{
					case 100:	i |= 0x10;break;
					case 125:	i |= 0x20;break;
					case 250:	i |= 0x30;break;
					case 800:	i |= 0x50;break;
					case 1000:i |= 0x60;break;
					default :	i |= 0x40;break;
				}
				Flash_WriteIntoBootLoadModeFlag(i);
				NVIC_SystemReset();        /* 复位CPU */
			}
			break;
		}
	}
}


extern FDCAN_HandleTypeDef hfdcan1;;
FDCAN_TxHeaderTypeDef   CanTxHeader;
FDCAN_RxHeaderTypeDef   CanRxHeader;
uint8_t CanRxData[8],CanTxData[8];
uint32_t              CanTxMailbox;
void HAL_FDCAN_RxFifo0Callback(FDCAN_HandleTypeDef *hfdcan, uint32_t RxFifo0ITs)
{
	if(hfdcan->Instance==FDCAN1)
	{
		/* Get RX message */
		uint8_t FillLevel=HAL_FDCAN_GetRxFifoFillLevel(hfdcan, FDCAN_RX_FIFO0);
		uint8_t i=0;
		for(i=0;i<FillLevel;i++)
		{
			if (HAL_FDCAN_GetRxMessage(hfdcan, FDCAN_RX_FIFO0, &CanRxHeader, CanRxData) != HAL_OK)
			{
				/* Reception Error */
				return;
			}
			if(CanRxHeader.IdType == FDCAN_EXTENDED_ID)
				startBootloader(CanRxHeader.Identifier,CanRxData);
			else
				CAN_ResolutionGeneral_Protocol(CanRxData,(CanRxHeader.DataLength>>16));
		}
	}
}
/*
函数功能：通用协议解析协议
输入参数：Mode:0:232,1:485,2:CAN
          data:接收数据
          len:数据长度
返回值：  -1:帧头错误
          -2:地址错误
          -3:校验错误
					-4:长度错误
					-5：命令错误
*/
int CAN_ResolutionGeneral_Protocol(uint8_t *data , uint8_t len)
{
  uint8_t cmd;
  int Value,ReturnValue=0;     
  cmd=data[0];
  Value=(int)(((uint32_t)(data[4])<<24)|((uint32_t)(data[5])<<16)|((uint32_t)(data[6])<<8)|((uint32_t)(data[7])));
  ReturnValue=ResolutionGeneral_Cmd(cmd,Value);
//  if(ReturnValue>=0)
	#ifdef FOR_NCS_VERSION
	CAN_ReturnGeneralData(pSysConfig->CommunicationConfig.Add&0x0F,ReturnValue);
	#else
  CAN_ReturnGeneralData(pSysConfig->CommunicationConfig.Add + 0x100,ReturnValue);
	#endif
  return 0;
}
void CAN_ReturnGeneralData(uint16_t add,int data)
{
  uint8_t CanData[8];
  CanData[0]=(data>>24)&0xff;
  CanData[1]=(data>>16)&0xff;
  CanData[2]=(data>>8)&0xff;
  CanData[3]=data&0xff;  
  CanSendStdData(add,CanData,4);
}
HAL_StatusTypeDef CanSendStdData(uint32_t id, uint8_t *data,uint8_t len)
{
	HAL_StatusTypeDef status;
	//uint8_t i;
	uint8_t retry=5;
  CanTxHeader.IdType = FDCAN_STANDARD_ID;
	CanTxHeader.Identifier = pSysConfig->CommunicationConfig.Add;
  CanTxHeader.TxFrameType = FDCAN_DATA_FRAME;
  CanTxHeader.DataLength = (len<<16);
	CanTxHeader.ErrorStateIndicator=FDCAN_ESI_ACTIVE;            
  CanTxHeader.BitRateSwitch=FDCAN_BRS_OFF;               //关闭速率切换
  CanTxHeader.FDFormat=FDCAN_CLASSIC_CAN;                //传统的CAN模式
  CanTxHeader.TxEventFifoControl=FDCAN_NO_TX_EVENTS;     //无法送事件
	CanTxHeader.MessageMarker=0xdd;
	memcpy(CanTxData,data,len);
	while(retry--)
	{
		if(HAL_FDCAN_AddMessageToTxFifoQ(&hfdcan1,&CanTxHeader,CanTxData)!= HAL_OK)
		{   
			status =  1;
			if((id&0xff0000)!=0x040000)
				can_delay(100);
		}
		else
		{
			return 0;
		}
	}
	return status;
}

#if 0
void ResponseTiTan_I2C_Send(char *Data,uint8_t len)
{
  uint16_t CalculateVerify;
  uint8_t sendData[8];
  memset(sendData,0,sizeof(sendData));
  memcpy(sendData,Data,len);
  SendCommunicationData(CommunicationMode,sendData,len);
}
/*
函数功能：解析I2C协议
输入参数：Add:地址
          data:接收数据
          len:数据长度
返回值：  -1:帧头错误
          -2:帧尾错误
          -3:地址错误
          -4:校验错误
*/
int ResolutionTiTan_I2C_Protocol(uint8_t *data , uint8_t len)
{
  uint8_t Status=0;
  char Cmd;
  char TmpStr[10];
  uint8_t Data8;
  Cmd=data[0]; 
  Status=CheckSysStatus();
  if(Status==1)
  {
    ResponseTiTan_Send("**\r",3);
    return 0;
  }
  else if(Status==2)
  {
    ResponseTiTan_Send("--\r",3);
    return 0;
  } 
  else
  switch(Cmd)
  {   
    case '+':
    case '-':
    case 'P':                  //电机通过码盘转动，自动选择最优路径 
      Data8=data[1];
      if(Data8>=1&&Data8<=10)
      {
        if(Cmd=='P')
          FlowSetPosition(Data8);
        else if(Cmd=='+')
          FlowSetPositionDir(Data8,0);
        else 
          FlowSetPositionDir(Data8,1);
      }
      else
      {

      }
    break;
   case 'S':                  //查询当前位置
    Data8=FlowGetPort();
    break;
   case 'N':                   //设置地址
    pSysConfig->CommunicationConfig.I2C_Add=Data8;  
    WriteSysConfigToFlash(pSysConfig);
    break;
   case 'M':                  //复位
      FlowAutoZeroMode();
    break;
   case 'X':
     if(Data8>=1&&Data8<=4)
     {
        SetRS232BaudRate(Data8-1);    //设置232波特率
        SetRS485BaudRate(Data8-1);  //设置485波特率
     }
     break;
    default:

    break;
  }
  return 0;
}
#endif

void ResponseSmartSV_UserData_Send(uint8_t status)
{
  uint16_t CalculateVerify;
  uint8_t sendData[64];
  sendData[0]=0xCC;
  sendData[1]=pSysConfig->CommunicationConfig.Add;
  sendData[2]=status;
	memcpy(&sendData[3],SysConfig.UserData,50);
  sendData[53]=0xDD;
  CalculateVerify=SumVerify16(sendData,54);
  sendData[54]=CalculateVerify&0xff;
  sendData[55]=CalculateVerify>>8;
  SendCommunicationData(CommunicationMode,sendData,56);
}

cmd_list_t cmd_list;
void cmd_list_clear(void)
{
	cmd_list.head = 0;
	cmd_list.tail = 0;
	cmd_list.num = 0;
	for(uint8_t i=0;i<CMD_LIST_MAX;i++)
	{
		cmd_list.cmd[i].cmd = 0;
		cmd_list.cmd[i].value = 0;
	}
}


int cmd_list_append(serial_t cmd)
{
	if(cmd_list.num >= CMD_LIST_MAX)
		return -1;
	cmd_list.cmd[cmd_list.tail%CMD_LIST_MAX].cmd = cmd.cmd;
	cmd_list.cmd[cmd_list.tail%CMD_LIST_MAX].value = cmd.value;
	cmd_list.tail = (cmd_list.tail+1)%CMD_LIST_MAX;
	cmd_list.num =(cmd_list.num+1)%CMD_LIST_MAX;
	return 0;
}

void cmd_list_execute(void)
{
	int ret;
	serial_t temp;
	if(cmd_list.num == 0)
		return;
	
	temp.cmd = cmd_list.cmd[cmd_list.head].cmd;
	temp.value = cmd_list.cmd[cmd_list.head].value;
	
	/*判断当前是否是空闲状态或错误和错误空闲状态，可以直接执行*/
	if((Valve_GetFaultStatus()<=1)||(Valve_GetFaultStatus()>=VALVE_FAULT))
	{
		ret=ResolutionGeneral_Cmd(temp.cmd,temp.value);
		UART_ReturnGeneralData(SysConfig.CommunicationConfig.Add,ret);
		/*成功执行，对列表已执行命令进行释放*/
		cmd_list.head = (cmd_list.head+1)%CMD_LIST_MAX;
		cmd_list.num--;
	}
	else
	{
		if(temp.cmd<0x06)//忙碌状态下如果遇到运动控制指令直接忽略
		{
			cmd_list.head = (cmd_list.head+1)%CMD_LIST_MAX;
			cmd_list.num--;
		}
		else if((temp.cmd==0x06) || (temp.cmd>=0x90))/*非空闲状态允许读基本和急停操作*/
		{
			ret=ResolutionGeneral_Cmd(temp.cmd,temp.value);
			UART_ReturnGeneralData(SysConfig.CommunicationConfig.Add,ret);
			/*成功执行，对列表已执行命令进行释放*/
			cmd_list.head = (cmd_list.head+1)%CMD_LIST_MAX;
			cmd_list.num--;
		}
	}
	
}

/***************************************从机I2C协议解析**************************************************/
bool SI2C_FlashSave_Flag = false;
bool SI2C_Control_Flag = false;
uint8_t SI2C_ControlBuf[2];
//Q指令
uint8_t idex_valveprofile = 0x1A;
//D指令
uint8_t idex_valvecommandmode = 0x01;
//X指令
uint8_t Savebaudrate = 0x0F;
uint8_t nowbaudrate = 0x0F;

//I2C切阀
void SI2C_Control_Task(void)
{
	if(false == SI2C_Control_Flag)
	{
		return ;
	}
	if('P' == SI2C_ControlBuf[0])
	{
		Valve_SetPort(SI2C_ControlBuf[1]);
	}
	else if('+' == SI2C_ControlBuf[0])
	{
		Valve_SetPortDir(1, SI2C_ControlBuf[1]);
	}
	else if('-' == SI2C_ControlBuf[0])
	{
		Valve_SetPortDir(0, SI2C_ControlBuf[1]);
	}
	SI2C_Control_Flag = false;
}

//掉电保存
void 	SI2C_FlashSave_Task(void)
{
	if(SI2C_FlashSave_Flag == false)
	{
		return ;
	}
	if(0x0F != Savebaudrate)
	{
		nowbaudrate = pSysConfig->CommunicationConfig.RS232_BaudRate;
		pSysConfig->CommunicationConfig.RS232_BaudRate = Savebaudrate;
		pSysConfig->CommunicationConfig.RS485_BaudRate = Savebaudrate;	
		Savebaudrate = 0x0F;
	}
	WriteSysConfigToFlash(&SysConfig);
	if(0x0F != nowbaudrate)
	{
		pSysConfig->CommunicationConfig.RS232_BaudRate = nowbaudrate;
		pSysConfig->CommunicationConfig.RS485_BaudRate = nowbaudrate;	
		nowbaudrate = 0x0F;
	}
	SI2C_FlashSave_Flag = false;
}

uint8_t Hexchar_To_Num(uint8_t data)
{
	uint8_t temp = data >> 4;
	if((temp > 0x09) || ((data & 0x0F) > 0x09))
	{
		return 0;
	}
	return (uint8_t)temp * 10U + (data & 0x0F);
}

//校验码计算 ，所有的异或
#define CSUM_CAL(data1, data2, data3)	 ((uint8_t)(data1 ^ data2 ^ data3))
/*************************************************
 *
 *	IDEX Titan协议SI2C解析
 *	Addr : cmd : value : Csum
 *	返回 : true：成功通信
 *				 false :解析出错
**************************************************/
bool Slave_IIC_Protocol(uint8_t *data)
{
	uint8_t si2ctxbuf[8] = {0};
//	uint8_t si2c_addr = data[0];			//接收的从机地址
	uint8_t si2c_cmd 	= data[1];			//接收的指令
	uint8_t si2c_data	= data[2];			//接收的数据
	uint8_t si2c_csum = data[3];			//接收的校验码
	uint8_t valve_si2c_addr = SI2C_ReadIICAddress();	//阀I2C地址

	//异或和校验
	if(si2c_csum != CSUM_CAL(si2c_cmd, si2c_data, valve_si2c_addr))
	{
		si2ctxbuf[0] = 0x44;	//校验报错
		SI2C_SendToMaster((char*)si2ctxbuf,1);
		return false;
	}	
	switch(((char)si2c_cmd))
	{
		case 'P':	//最优路径切换到指定位置
		case '+':	//逆时针切换到指定位置
		case '-':	//顺时针切换到指定位置
			if(VALVE_IDLE == Valve_GetFaultStatus())
			{
				SI2C_Control_Flag = true;
				SI2C_ControlBuf[0] = si2c_cmd;
				SI2C_ControlBuf[1] = si2c_data;
			}
			break;
		case 'S'://getport
			{
				uint8_t temp = Valve_GetFaultFlag();
				if(!temp)
				{
					si2ctxbuf[0] = Valve_GetCurrent_Channel();
				}
				else if(temp == 1)//error
					si2ctxbuf[0] = 0x99;
				else
					si2ctxbuf[0] = 0x66;
			}
			SI2C_SendToMaster((char*)si2ctxbuf,1);
			break;
		case 'N':	//设置从机地址
			if((si2c_data < 0x0E) || (si2c_data > 0xFE) || (si2c_data & 0x01))
			{
				return false;
			}
			pSysConfig->IIC_Addr = si2c_data;
			SI2C_FlashSave_Flag = true;
			break;
		case 'M':	//复位
			Valve_AutoZeroMode();
			break;
		case 'X':	//设置波特率
			if(si2c_data < 1 || si2c_data > 4)
			{
				return false;
			}
			Savebaudrate = si2c_data - 1;
			SI2C_FlashSave_Flag = true;
			break;
		case 'Q':	//read valve profile
			si2ctxbuf[0] = idex_valveprofile;
			SI2C_SendToMaster((char *)si2ctxbuf, 1);
			break;
		case 'R':	//程序版本
			si2ctxbuf[0] = SYS_SOFT_VERSIONS % 1000;			
			SI2C_SendToMaster((char *)si2ctxbuf, 1);
			break;
		case 'O':	//sets valve profile
			pSysConfig->i2c_valve_profile = si2c_data;
			SI2C_FlashSave_Flag = true;
			break;
		case 'F':	//sets valve command mode
			if(si2c_data < 0x01 || si2c_data > 0x05)
			{
				return false;
			}
			pSysConfig->i2c_valve_profile = si2c_data;
			SI2C_FlashSave_Flag = true;
			break;
		case 'E':	//read lastest valve error code
			si2ctxbuf[0] = 0x63;
			SI2C_SendToMaster((char *)si2ctxbuf, 1);
			break;
		case 'D':	//read valve command mode
			si2ctxbuf[0] = idex_valvecommandmode;
			SI2C_SendToMaster((char *)si2ctxbuf, 1);
			break;
		default:
			return false;
	}		
	return true;
}


