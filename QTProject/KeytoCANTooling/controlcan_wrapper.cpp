// controlcan_wrapper.cpp
#include "controlcan_wrapper.h"
#include <QDebug>
#include <QThread>

ControlCANWrapper::ControlCANWrapper(QObject *parent)
    : QObject(parent)
    , m_initialized(false)
    , VCI_OpenDevice(nullptr)
    , VCI_CloseDevice(nullptr)
    , VCI_InitCAN(nullptr)
    , VCI_ReadBoardInfo(nullptr)
    , VCI_StartCAN(nullptr)
    , VCI_ResetCAN(nullptr)
    , VCI_Transmit(nullptr)
    , VCI_Receive(nullptr)
    , VCI_ReadErrInfo(nullptr)
    , VCI_ClearBuffer(nullptr)
{
}

ControlCANWrapper::~ControlCANWrapper()
{
    unloadLibrary();
}

bool ControlCANWrapper::loadLibrary(const QString &dllPath)
{
    if (m_library.isLoaded()) {
        return true;
    }
    m_library.setFileName(dllPath);
    if (!m_library.load()) {
        qWarning() << "Failed to load ControlCAN.dll:" << m_library.errorString();
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
    VCI_ReadErrInfo = (VCI_ReadErrInfoFunc)m_library.resolve("VCI_ReadErrorInfo");
    VCI_ClearBuffer = (VCI_ClearBufferFunc)m_library.resolve("VCI_ClearBuffer");

    // 检查关键函数是否解析成功
    if (!VCI_OpenDevice || !VCI_CloseDevice || !VCI_InitCAN || !VCI_StartCAN ||
            !VCI_Transmit || !VCI_Receive) {
        qWarning() << "Failed to resolve one or more critical functions";
        unloadLibrary();
        return false;
    }

    m_initialized = true;
    qDebug() << "ControlCAN.dll loaded successfully";
    return true;
}

void ControlCANWrapper::unloadLibrary()
{
    if (m_library.isLoaded()) {
        m_library.unload();
        m_initialized = false;

        // 重置所有函数指针
        VCI_OpenDevice = nullptr;
        VCI_CloseDevice = nullptr;
        VCI_InitCAN = nullptr;
        VCI_ReadBoardInfo = nullptr;
        VCI_StartCAN = nullptr;
        VCI_ResetCAN = nullptr;
        VCI_Transmit = nullptr;
        VCI_Receive = nullptr;
        VCI_ReadErrInfo = nullptr;
        VCI_ClearBuffer = nullptr;
    }
}

bool ControlCANWrapper::isLoaded() const
{
    return m_library.isLoaded() && m_initialized;
}

int ControlCANWrapper::openDevice(uint32_t deviceType, uint32_t deviceIndex, uint32_t reserved)
{
    if (!VCI_OpenDevice) {
        emit errorOccurred("VCI_OpenDevice function not available");
        return -1;
    }

    int result = VCI_OpenDevice(deviceType, deviceIndex, reserved);
    if (result != 1) {
        emit errorOccurred(QString("Failed to open device. Error code: %1").arg(result));
    }
    return result;
}

int ControlCANWrapper::closeDevice(uint32_t deviceType, uint32_t deviceIndex)
{
    if (!VCI_CloseDevice) {
        emit errorOccurred("VCI_CloseDevice function not available");
        return -1;
    }

    int result = VCI_CloseDevice(deviceType, deviceIndex);
    if (result != 1) {
        emit errorOccurred(QString("Failed to close device. Error code: %1").arg(result));
    }
    return result;
}

int ControlCANWrapper::getBoardInfo(uint32_t deviceType, uint32_t deviceIndex, VCI_BOARD_INFO *pInfo)
{
    if (!VCI_ReadBoardInfo) {
        emit errorOccurred("VCI_ReadBoardInfo function not available");
        return -1;
    }
    return VCI_ReadBoardInfo(deviceType, deviceIndex, pInfo);
}

int ControlCANWrapper::initCAN(uint32_t deviceType, uint32_t deviceIndex, uint32_t canIndex, VCI_INIT_CONFIG *pInitConfig)
{
    if (!VCI_InitCAN) {
        emit errorOccurred("VCI_InitCAN function not available");
        return -1;
    }

    int result = VCI_InitCAN(deviceType, deviceIndex, canIndex, pInitConfig);
    if (result != 1) {
        emit errorOccurred(QString("Failed to init CAN. Error code: %1").arg(result));
    }
    return result;
}

int ControlCANWrapper::startCAN(uint32_t deviceType, uint32_t deviceIndex, uint32_t canIndex)
{
    if (!VCI_StartCAN) {
        emit errorOccurred("VCI_StartCAN function not available");
        return -1;
    }

    int result = VCI_StartCAN(deviceType, deviceIndex, canIndex);
    if (result != 1) {
        emit errorOccurred(QString("Failed to start CAN. Error code: %1").arg(result));
    }
    return result;
}

int ControlCANWrapper::resetCAN(uint32_t deviceType, uint32_t deviceIndex, uint32_t canIndex)
{
    if (!VCI_ResetCAN) {
        emit errorOccurred("VCI_ResetCAN function not available");
        return -1;
    }
    return VCI_ResetCAN(deviceType, deviceIndex, canIndex);
}

int ControlCANWrapper::transmit(uint32_t deviceType, uint32_t deviceIndex, uint32_t canIndex,
                                VCI_CAN_OBJ *pSend, uint32_t len)
{
    if (!VCI_Transmit) {
        emit errorOccurred("VCI_Transmit function not available");
        return -1;
    }
    return VCI_Transmit(deviceType, deviceIndex, canIndex, pSend, len);
}

int ControlCANWrapper::receive(uint32_t deviceType, uint32_t deviceIndex, uint32_t canIndex,
                               VCI_CAN_OBJ *pReceive, uint32_t len, int waitTime)
{
    if (!VCI_Receive) {
        emit errorOccurred("VCI_Receive function not available");
        return -1;
    }
    return VCI_Receive(deviceType, deviceIndex, canIndex, pReceive, len, waitTime);
}

int ControlCANWrapper::readErrInfo(uint32_t deviceType, uint32_t deviceIndex, uint32_t canIndex,
                                   VCI_ERR_INFO *pErrInfo)
{
    if (!VCI_ReadErrInfo) {
        emit errorOccurred("VCI_ReadErrorInfo function not available");
        return -1;
    }
    return VCI_ReadErrInfo(deviceType, deviceIndex, canIndex, pErrInfo);
}

QString ControlCANWrapper::errorCodeToString(int errorCode)
{
    switch (errorCode) {
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


