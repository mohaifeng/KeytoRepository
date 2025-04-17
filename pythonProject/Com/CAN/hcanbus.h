
#ifndef CANBUS_H
#define CANBUS_H

#if defined(_WIN32) || defined(__CYGWIN__) || defined(_WIN32_WCE)
#define LIBUSB_CALL WINAPI
#else
#define LIBUSB_CALL
#endif

typedef  struct  _Dev_Info {
    char            HW_Type[32];    //设备型号 字符串
    char            HW_Ser[32];     //设备序列号 字符串 
    char            HW_Ver[32];     //硬件版本 字符串
    char            FW_Ver[32];     //软件版本 字符串
    char            MF_Date[32];    //生产日期 字符串
} Dev_Info, *PDev_Info;

typedef  struct  _Can_Msg {
    unsigned int    ID;            //报文ID
    unsigned int    TimeStamp;     //时间戳
    char            FrameType;     //帧类型
    char            DataLen;       //有效字节数
    char	        Data[8];       //报文数据
    char	        ExternFlag;    //扩展帧标识：0标准帧,1扩展帧
    char	        RemoteFlag;    //远程帧标识：0数据帧,1远程帧
    char            BusSatus;      //总线状态
    char            ErrSatus;      //错误状态
    char            TECounter;     //发送错误计数
    char            RECounter;     //接收错误计数
}Can_Msg, *P_Can_Msg;

typedef struct _Can_Status {
    char            BusSatus;      //总线状态
    char            ErrSatus;      //错误状态
    char            TECounter;     //发送错误计数
    char            RECounter;     //接收错误计数
}Can_Status, *P_Can_Status;

typedef struct _Can_Config {
    unsigned int    Baudrate;      //波特率 
    unsigned int    Configs;       //配置信息：0x01接通内部电阻 0x02离线唤醒 0x04自动重传
    unsigned int    Model;         //工作模式：0 正常模式,1 环回模式,2 静默模式,3 静默环回模式
}Can_Config, *P_Can_Config;

#ifdef __cplusplus
extern "C" {
#endif

int __stdcall   Reg_HotPlug_Func(void(*pfunc)(void));   //热拔插函数 
int __stdcall   CAN_ScanDevice(void);                   //扫描CAN设备 
int __stdcall   CAN_OpenDevice(unsigned int devNum);    //打开CAN设备 
int __stdcall   CAN_CloseDevice(unsigned int devNum);   //关闭CAN设备 
int __stdcall   CAN_ReadDevInfo(unsigned int devNum, PDev_Info devinfo);  //读取CAN设备信息 
int __stdcall   CAN_Init(unsigned int devNum,P_Can_Config pInitConfig);   //初始化CAN设备
int __stdcall   CAN_SetFilter(unsigned int devNum,char namber, char type, unsigned int ftID, unsigned int ftMask, char enable); //设置硬件屏蔽
int __stdcall   CAN_Reset(unsigned int devNum);         //复位CAN设备
int __stdcall   CAN_GetReceiveNum(unsigned int devNum); //获取接收缓冲区中接收到但尚未被读取的帧数量
int __stdcall   CAN_Transmit(unsigned int devNum, P_Can_Msg canmsg, unsigned int items, int timeou);  //发送CAN报文
int __stdcall   CAN_TransmitRt(unsigned int devNum, P_Can_Msg canmsg, unsigned int items, unsigned int *txitems, int timeou);   //定时发送CAN报文
int __stdcall   CAN_Receive(unsigned int devNum,P_Can_Msg canmsg, int Len, int timeou);  //接收CAN报文
int __stdcall   CAN_GetStatus(unsigned int devNum,P_Can_Status status);                  //获取CAN设备状态
#ifdef __cplusplus
}
#endif

#endif



