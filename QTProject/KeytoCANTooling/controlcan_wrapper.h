#ifndef CONTROLCAN_WRAPPER_H
#define CONTROLCAN_WRAPPER_H

#include <QObject>
#include <QLibrary>
#include <QVector>

#ifdef Q_OS_WIN
#include <windows.h>
#endif

#define RESERVED    0

// ControlCAN.dll 数据类型定义
typedef struct _VCI_BOARD_INFO {
    uint16_t hw_Version;
    uint16_t fw_Version;
    uint16_t dr_Version;
    uint16_t in_Version;
    uint16_t irq_Num;
    uint8_t can_Num;
    char str_Serial_Num[20];
    char str_hw_Type[40];
    uint16_t reserved[4];
} VCI_BOARD_INFO, *PVCI_BOARD_INFO;

typedef struct _VCI_CAN_OBJ {
    uint32_t ID;
    uint32_t TimeStamp;
    uint8_t TimeFlag;
    uint8_t SendType;
    uint8_t RemoteFlag;
    uint8_t ExternFlag;
    uint8_t DataLen;
    uint8_t Data[8];
    uint8_t Reserved[3];
} VCI_CAN_OBJ, *PVCI_CAN_OBJ;

typedef struct _VCI_INIT_CONFIG {
    uint32_t AccCode;
    uint32_t AccMask;
    uint32_t Reserved;
    uint8_t Filter;
    uint8_t Timing0;
    uint8_t Timing1;
    uint8_t Mode;
} VCI_INIT_CONFIG, *PVCI_INIT_CONFIG;

typedef struct _VCI_ERR_INFO
{
    uint32_t ErrCode;
    uint8_t Passive_ErrData[3];
    uint8_t ArLost_ErrData;
} VCI_ERR_INFO, *PVCI_ERR_INFO;

class ControlCANWrapper : public QObject
{
    Q_OBJECT

public:
    explicit ControlCANWrapper(QObject *parent = nullptr);
    ~ControlCANWrapper();

    // DLL加载管理
    bool LoadCANLibrary(const QString &dllPath);
    void UnloadCANLibrary();
    bool IsLoaded() const;
    // 设备操作函数
    bool OpenDevice(uint32_t deviceType, uint32_t deviceIndex, uint32_t reserved=RESERVED);//此函数用以打开设备。注意一个设备只能打开一次。
    bool CloseDevice(uint32_t deviceType, uint32_t deviceIndex);//此函数用以关闭设备。
    bool GetBoardInfo(uint32_t deviceType, uint32_t deviceIndex, VCI_BOARD_INFO *pInfo);//此函数用以获取设备信息
    // CAN通道操作
    bool InitCAN(uint32_t deviceType, uint32_t deviceIndex, uint32_t canIndex, VCI_INIT_CONFIG *pInitConfig);//此函数用以初始化指定的CAN通道。有多个CAN通道时，需要多次调用。
    bool StartCAN(uint32_t deviceType, uint32_t deviceIndex, uint32_t canIndex);//此函数用以启动CAN卡的某一个CAN通道。有多个CAN通道时，需要多次调用。
    bool ResetCAN(uint32_t deviceType, uint32_t deviceIndex, uint32_t canIndex);//此函数用以复位CAN。主要用与 VCI_StartCAN配合使用，无需再初始化，即可恢复CAN卡的正常状态。

    // 数据收发
    bool Transmit(uint32_t deviceType, uint32_t deviceIndex, uint32_t canIndex,VCI_CAN_OBJ *pSend, uint32_t len);//发送函数。返回值为实际发送成功的帧数。
    int Receive(uint32_t deviceType, uint32_t deviceIndex, uint32_t canIndex,VCI_CAN_OBJ *pReceive, uint32_t len, int waitTime=RESERVED);//接收函数。此函数从指定的设备CAN通道的接收缓冲区中读取数据。
    // 工具函数
    QString ErrorCodeToString(int errorCode);
signals:
    void errorOccurred(const QString &errorMessage);
    void canMessageReceived(const QVector<VCI_CAN_OBJ> &messages);

private:
    QLibrary m_library;
    // 函数指针定义
    typedef int (*VCI_OpenDeviceFunc)(uint32_t, uint32_t, uint32_t);
    typedef int (*VCI_CloseDeviceFunc)(uint32_t, uint32_t);
    typedef int (*VCI_InitCANFunc)(uint32_t, uint32_t, uint32_t, PVCI_INIT_CONFIG);
    typedef int (*VCI_ReadBoardInfoFunc)(uint32_t, uint32_t, PVCI_BOARD_INFO);
    typedef int (*VCI_StartCANFunc)(uint32_t, uint32_t, uint32_t);
    typedef int (*VCI_ResetCANFunc)(uint32_t, uint32_t, uint32_t);
    typedef int (*VCI_TransmitFunc)(uint32_t, uint32_t, uint32_t, PVCI_CAN_OBJ, uint32_t);
    typedef int (*VCI_ReceiveFunc)(uint32_t, uint32_t, uint32_t, PVCI_CAN_OBJ, uint32_t, int);
    typedef int (*VCI_ClearBufferFunc)(uint32_t, uint32_t, uint32_t);

    // 函数指针实例
    VCI_OpenDeviceFunc VCI_OpenDevice;
    VCI_CloseDeviceFunc VCI_CloseDevice;
    VCI_InitCANFunc VCI_InitCAN;
    VCI_ReadBoardInfoFunc VCI_ReadBoardInfo;
    VCI_StartCANFunc VCI_StartCAN;
    VCI_ResetCANFunc VCI_ResetCAN;
    VCI_TransmitFunc VCI_Transmit;
    VCI_ReceiveFunc VCI_Receive;
    VCI_ClearBufferFunc VCI_ClearBuffer;
};

#endif // CONTROLCAN_WRAPPER_H
