
#ifndef CANBUS_H
#define CANBUS_H

#if defined(_WIN32) || defined(__CYGWIN__) || defined(_WIN32_WCE)
#define LIBUSB_CALL WINAPI
#else
#define LIBUSB_CALL
#endif

typedef  struct  _Dev_Info {
    char            HW_Type[32];    //�豸�ͺ� �ַ���
    char            HW_Ser[32];     //�豸���к� �ַ��� 
    char            HW_Ver[32];     //Ӳ���汾 �ַ���
    char            FW_Ver[32];     //����汾 �ַ���
    char            MF_Date[32];    //�������� �ַ���
} Dev_Info, *PDev_Info;

typedef  struct  _Can_Msg {
    unsigned int    ID;            //����ID
    unsigned int    TimeStamp;     //ʱ���
    char            FrameType;     //֡����
    char            DataLen;       //��Ч�ֽ���
    char	        Data[8];       //��������
    char	        ExternFlag;    //��չ֡��ʶ��0��׼֡,1��չ֡
    char	        RemoteFlag;    //Զ��֡��ʶ��0����֡,1Զ��֡
    char            BusSatus;      //����״̬
    char            ErrSatus;      //����״̬
    char            TECounter;     //���ʹ������
    char            RECounter;     //���մ������
}Can_Msg, *P_Can_Msg;

typedef struct _Can_Status {
    char            BusSatus;      //����״̬
    char            ErrSatus;      //����״̬
    char            TECounter;     //���ʹ������
    char            RECounter;     //���մ������
}Can_Status, *P_Can_Status;

typedef struct _Can_Config {
    unsigned int    Baudrate;      //������ 
    unsigned int    Configs;       //������Ϣ��0x01��ͨ�ڲ����� 0x02���߻��� 0x04�Զ��ش�
    unsigned int    Model;         //����ģʽ��0 ����ģʽ,1 ����ģʽ,2 ��Ĭģʽ,3 ��Ĭ����ģʽ
}Can_Config, *P_Can_Config;

#ifdef __cplusplus
extern "C" {
#endif

int __stdcall   Reg_HotPlug_Func(void(*pfunc)(void));   //�Ȱβ庯�� 
int __stdcall   CAN_ScanDevice(void);                   //ɨ��CAN�豸 
int __stdcall   CAN_OpenDevice(unsigned int devNum);    //��CAN�豸 
int __stdcall   CAN_CloseDevice(unsigned int devNum);   //�ر�CAN�豸 
int __stdcall   CAN_ReadDevInfo(unsigned int devNum, PDev_Info devinfo);  //��ȡCAN�豸��Ϣ 
int __stdcall   CAN_Init(unsigned int devNum,P_Can_Config pInitConfig);   //��ʼ��CAN�豸
int __stdcall   CAN_SetFilter(unsigned int devNum,char namber, char type, unsigned int ftID, unsigned int ftMask, char enable); //����Ӳ������
int __stdcall   CAN_Reset(unsigned int devNum);         //��λCAN�豸
int __stdcall   CAN_GetReceiveNum(unsigned int devNum); //��ȡ���ջ������н��յ�����δ����ȡ��֡����
int __stdcall   CAN_Transmit(unsigned int devNum, P_Can_Msg canmsg, unsigned int items, int timeou);  //����CAN����
int __stdcall   CAN_TransmitRt(unsigned int devNum, P_Can_Msg canmsg, unsigned int items, unsigned int *txitems, int timeou);   //��ʱ����CAN����
int __stdcall   CAN_Receive(unsigned int devNum,P_Can_Msg canmsg, int Len, int timeou);  //����CAN����
int __stdcall   CAN_GetStatus(unsigned int devNum,P_Can_Status status);                  //��ȡCAN�豸״̬
#ifdef __cplusplus
}
#endif

#endif



