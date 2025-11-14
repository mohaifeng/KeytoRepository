// controlcan_wrapper.cpp
#include "controlcan_wrapper.h"
#include <QDebug>
#include <QThread>

ControlCANWrapper::ControlCANWrapper(QObject *parent)
    : QObject(parent)
    , VCI_OpenDevice(nullptr)
    , VCI_CloseDevice(nullptr)
    , VCI_InitCAN(nullptr)
    , VCI_ReadBoardInfo(nullptr)
    , VCI_StartCAN(nullptr)
    , VCI_ResetCAN(nullptr)
    , VCI_Transmit(nullptr)
    , VCI_Receive(nullptr)
    , VCI_ClearBuffer(nullptr)
{
}

ControlCANWrapper::~ControlCANWrapper()
{
    UnloadCANLibrary();
}

bool ControlCANWrapper::LoadCANLibrary(const QString &dllPath)
{
    if (m_library.isLoaded())
    {
        return true;
    }
    m_library.setFileName(dllPath);
    if (!m_library.load())
    {
        emit errorOccurred(QString("Failed to load DLL: %1").arg(m_library.errorString()));
        return false;
    }
    // 解析所有函数
    VCI_OpenDevice = (VCI_OpenDeviceFunc)m_library.resolve("VCI_OpenDevice");
    VCI_CloseDevice = (VCI_CloseDeviceFunc)m_library.resolve("VCI_CloseDevice");
    VCI_InitCAN = (VCI_InitCANFunc)m_library.resolve("VCI_InitCAN");
    VCI_ReadBoardInfo = (VCI_ReadBoardInfoFunc)m_library.resolve("VCI_ReadBoardInfo");
    VCI_StartCAN = (VCI_StartCANFunc)m_library.resolve("VCI_StartCAN");
    VCI_ResetCAN = (VCI_ResetCANFunc)m_library.resolve("VCI_ResetCAN");
    VCI_Transmit = (VCI_TransmitFunc)m_library.resolve("VCI_Transmit");
    VCI_Receive = (VCI_ReceiveFunc)m_library.resolve("VCI_Receive");
    VCI_ClearBuffer = (VCI_ClearBufferFunc)m_library.resolve("VCI_ClearBuffer");
    // 检查关键函数是否解析成功
    if (!VCI_OpenDevice || !VCI_CloseDevice || !VCI_InitCAN || !VCI_StartCAN ||!VCI_Transmit || !VCI_Receive)
    {
        UnloadCANLibrary();
        return false;
    }
    return true;
}

void ControlCANWrapper::UnloadCANLibrary()
{
    if (m_library.isLoaded())
    {
        m_library.unload();
        // 重置所有函数指针
        VCI_OpenDevice = nullptr;
        VCI_CloseDevice = nullptr;
        VCI_InitCAN = nullptr;
        VCI_ReadBoardInfo = nullptr;
        VCI_StartCAN = nullptr;
        VCI_ResetCAN = nullptr;
        VCI_Transmit = nullptr;
        VCI_Receive = nullptr;
        VCI_ClearBuffer = nullptr;
    }
}

bool ControlCANWrapper::OpenDevice(uint32_t deviceType, uint32_t deviceIndex, uint32_t reserved)
{
    int result = VCI_OpenDevice(deviceType, deviceIndex, reserved);
    if (result != 1)
    {
        emit errorOccurred(QString("Failed to open device. Error code: %1:%2").arg(result).arg(ErrorCodeToString(result)));
        return false;
    }
    return true;
}

bool ControlCANWrapper::CloseDevice(uint32_t deviceType, uint32_t deviceIndex)
{
    int result = VCI_CloseDevice(deviceType, deviceIndex);
    if (result != 1)
    {
        emit errorOccurred(QString("Failed to close device. Error code: %1:%2").arg(result).arg(ErrorCodeToString(result)));
        return false;
    }
    return true;
}

bool ControlCANWrapper::GetBoardInfo(uint32_t deviceType, uint32_t deviceIndex, VCI_BOARD_INFO *pInfo)
{
    if(VCI_ReadBoardInfo(deviceType, deviceIndex, pInfo) != 1)
    {
       return false;
    }
    return true;
}

bool ControlCANWrapper::InitCAN(uint32_t deviceType, uint32_t deviceIndex, uint32_t canIndex, VCI_INIT_CONFIG *pInitConfig)
{
    int result = VCI_InitCAN(deviceType, deviceIndex, canIndex, pInitConfig);
    if (result != 1)
    {
        emit errorOccurred(QString("Failed to init CAN. Error code: %1:%2").arg(result).arg(ErrorCodeToString(result)));
        return false;
    }
    return true;
}

bool ControlCANWrapper::StartCAN(uint32_t deviceType, uint32_t deviceIndex, uint32_t canIndex)
{
    int result = VCI_StartCAN(deviceType, deviceIndex, canIndex);
    if (result != 1)
    {
        emit errorOccurred(QString("Failed to start CAN. Error code: %1:%2").arg(result).arg(ErrorCodeToString(result)));
        return false;
    }
    return true;
}

bool ControlCANWrapper::ResetCAN(uint32_t deviceType, uint32_t deviceIndex, uint32_t canIndex)
{
    int result = VCI_ResetCAN(deviceType, deviceIndex, canIndex);
    if (result != 1)
    {
        emit errorOccurred(QString("Failed to reset CAN. Error code: %1:%2").arg(result).arg(ErrorCodeToString(result)));
        return false;
    }
    return true;
}

bool ControlCANWrapper::Transmit(uint32_t deviceType, uint32_t deviceIndex, uint32_t canIndex,VCI_CAN_OBJ *pSend, uint32_t len)
{
    uint32_t result= VCI_Transmit(deviceType, deviceIndex, canIndex, pSend, len);
    if(result != len)
    {
        return false;
    }
    return true;
}

int ControlCANWrapper::Receive(uint32_t deviceType, uint32_t deviceIndex, uint32_t canIndex,VCI_CAN_OBJ *pReceive, uint32_t len, int waitTime)
{
    return VCI_Receive(deviceType, deviceIndex, canIndex, pReceive, len, waitTime);
}

QString ControlCANWrapper::ErrorCodeToString(int errorCode)
{
    switch (errorCode)
    {
    case 1: return "Success";
    case 0: return "Failed";
    case -1: return "USB/CAN device not found";
    case -2: return "USB/CAN device occupied";
    case -3: return "Device handle error";
    case -4: return "Data length error";
    case -5: return "Device not opened";
    case -6: return "Device initialization failed";
    case -7: return "Device not started";
    case -8: return "No message received";
    case -9: return "Buffer overflow";
    case -10: return "Device type error";
    case -11: return "Device index error";
    case -12: return "CAN index error";
    case -13: return "Channel error";
    case -14: return "Invalid parameter";
    case -15: return "Other error";
    default: return QString("Unknown error code: %1").arg(errorCode);
    }
}


