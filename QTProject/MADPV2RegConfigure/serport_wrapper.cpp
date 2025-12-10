#include "serport_wrapper.h"
#include <QThread>
#include <QDebug>

SerialPortWrapper::SerialPortWrapper(QObject *parent): QObject(parent)
{
    m_serialPort=new QSerialPort(this);
    m_config.dataBits=QSerialPort::Data8;//8个数据位
    m_config.parity=QSerialPort::NoParity;//无校验位
    m_config.stopBits=QSerialPort::OneStop;//1个停止位
    m_config.flowControl=QSerialPort::NoFlowControl;//无流控制
    m_config.readTimeout=500;
    m_config.writeTimeout=2000;
    m_config.responseTimeout=2000;
    frameintegritycheckenable=false;
    SetupConnections();
}
SerialPortWrapper::~SerialPortWrapper()
{

}

void SerialPortWrapper::SetupConnections()
{
    // 连接串口信号
    // 设置响应超时定时器（单次触发）
    m_responsefinishTimer->setSingleShot(true);
    connect(m_responsefinishTimer, &QTimer::timeout,this, &SerialPortWrapper::ResponseFinish);
    connect(m_serialPort, &QSerialPort::readyRead,this, &SerialPortWrapper::OnReadyRead);
    connect(m_serialPort, &QSerialPort::errorOccurred,this, &SerialPortWrapper::HandleError);
}

//打开串口
bool SerialPortWrapper::OpenPort(const SerialConfig &config)
{
    ClosePort();  // 先关闭已打开的端口
    m_config = config;
    m_serialPort->setPortName(m_config.portName);
    m_serialPort->setBaudRate(m_config.baudRate);
    m_serialPort->setDataBits(m_config.dataBits);
    m_serialPort->setParity(m_config.parity);
    m_serialPort->setStopBits(m_config.stopBits);
    m_serialPort->setFlowControl(m_config.flowControl);

    m_serialPort->setReadBufferSize(1024 * 1024);  // 1MB缓冲区
    if (!m_serialPort->open(QIODevice::ReadWrite))
    {
        m_lastError = SerialPortWrapper::OpenFailed;
        m_lastErrorString = QString("无法打开串口 %1: %2").arg(config.portName,m_serialPort->errorString());
        emit errorOccurred(m_lastError, m_lastErrorString);
        return false;
    }
    // 清空缓冲区
    m_readBuffer.clear();
    m_lastError = SerialPortWrapper::NoError;
    m_lastErrorString.clear();
    emit portOpened(config.portName);
    emit debugMessage(QString("串口 %1 已打开，波特率: %2").arg(config.portName,config.baudRate));
    return true;
}

//关闭串口
void SerialPortWrapper::ClosePort()
{
    QString portName = m_serialPort->portName();
    if (m_serialPort->isOpen())
    {
        m_serialPort->close();
    }
    emit portClosed();
    emit debugMessage(QString("串口 %1 已关闭").arg(portName));
}

bool SerialPortWrapper::IsOpen() const
{
    return m_serialPort->isOpen();
}

// 获取可用串口
QList<QSerialPortInfo> SerialPortWrapper::GetAvailablePorts()
{
    return QSerialPortInfo::availablePorts();
}

QStringList SerialPortWrapper::GetAvailablePortNames()
{
    QStringList ports;
    foreach (const QSerialPortInfo &info, GetAvailablePorts())
    {
        ports << info.portName();
    }
    return ports;
}
//注册数据完整性判断回调函数
void SerialPortWrapper::RegisterFrameIntegrityCheckCallback(FrameIntegrityCheckCallback callback)
{
    if(callback != nullptr)
    {
        frameintegritycheckenable=true;
    }
    else
    {
        frameintegritycheckenable=false;
    }
    m_callback=callback;
}

void SerialPortWrapper::OnReadyRead()
{
    if (!IsOpen())
    {
        return;
    }
    responsefinishflag=false;
    QMutexLocker locker(&m_readMutex);
    QByteArray newData = m_serialPort->readAll();
    if (newData.isEmpty())
    {
        return;
    }
    m_readBuffer.append(newData);
    m_bytesReceived += newData.size();
    //发射原始数据信号
    emit dataReceived(newData);
    emit debugMessage(QString("接收到 %1 字节数据").arg(newData.size()));
    // 重新启动超时定时器
    m_responsefinishTimer->start(5);  // 5ms超时（可根据波特率调整）
}

void SerialPortWrapper::ResponseFinish()
{
    if(!m_readBuffer.isEmpty())
    {
        if(frameintegritycheckenable)
        {
            if (m_callback(m_readBuffer))
            {
                emit frameReceived(m_readBuffer);
                emit debugMessage(QString("收到完整帧: %1").arg(m_readBuffer.toHex(' ').toUpper()));
            }
            else
            {
                m_lastError = SerialPortWrapper::IntegrityError;
                m_lastErrorString = QString("数据解析错误:%1").arg(m_readBuffer.toHex());
                emit errorOccurred(m_lastError, m_lastErrorString);
            }
        }
        else
        {
            emit frameReceived(m_readBuffer);
            emit debugMessage(QString("收到完整帧: %1").arg(m_readBuffer.toHex(' ').toUpper()));
        }
    }
}
// 等待写入完成
bool SerialPortWrapper::WaitForBytesWritten(int timeout)
{
    if (!m_serialPort->waitForBytesWritten(timeout))
    {
        return false;
    }
    return true;
}
// 写入数据
bool SerialPortWrapper::ByteArrayTransmit(const QByteArray &data)
{
    if (!IsOpen())
    {
        m_lastError = SerialPortWrapper::PortNotOpen;
        m_lastErrorString = "串口未打开";
        emit errorOccurred(m_lastError, m_lastErrorString);
        return false;
    }
    QMutexLocker locker(&m_writeMutex);
    qint64 bytesWritten = m_serialPort->write(data);
    if (bytesWritten == -1||bytesWritten !=data.size())
    {
        m_lastError = SerialPortWrapper::WriteFailed;
        m_lastErrorString = m_serialPort->errorString();
        emit errorOccurred(m_lastError, m_lastErrorString);
        return false;
    }
    if (!WaitForBytesWritten(m_config.writeTimeout))
    {
        m_lastError = SerialPortWrapper::TimeoutError;
        m_lastErrorString = "写入超时";
        emit errorOccurred(m_lastError, m_lastErrorString);
        return false;
    }
    m_bytesSent += bytesWritten;
    emit dataSent(data);
    emit debugMessage(QString("发送数据: %1").arg(data.toHex(' ').toUpper()));
    return true;
}
QByteArray SerialPortWrapper::ReadData(qint64 maxSize)
{
    if (!IsOpen())
    {
        m_lastError = PortNotOpen;
        m_lastErrorString = "串口未打开";
        return QByteArray();
    }
    QMutexLocker locker(&m_readMutex);
    return m_serialPort->read(maxSize);
}
QByteArray SerialPortWrapper::ReadAll()
{
    return ReadData(GetBytesAvailable());
}
qint64 SerialPortWrapper::GetBytesAvailable() const
{
    return m_serialPort->bytesAvailable();
}
// 等待响应
QByteArray SerialPortWrapper::WaitForResponse(int timeout)
{
    m_readBuffer.clear();
    int waitTime = (timeout > 0) ? timeout : m_config.responseTimeout;
    QEventLoop loop;
    connect(this, &SerialPortWrapper::dataReceived, &loop, &QEventLoop::quit);
    connect(m_responseTimer, &QTimer::timeout, &loop, &QEventLoop::quit);
    m_responseTimer->setSingleShot(true);
    m_responseTimer->start(waitTime);
    loop.exec();
    m_responseTimer->stop();
    return m_readBuffer;
}

// 同步发送并等待响应
QByteArray SerialPortWrapper::ByteArrayTransmitWaitAck(const QByteArray &data, int timeout)
{
    if (!IsOpen())
    {
        return QByteArray();
    }
    // 发送数据
    if (!ByteArrayTransmit(data))
    {
        return QByteArray();
    }
    if (WaitForResponse(timeout).isEmpty())
    {
        m_lastError = SerialPortWrapper::TimeoutError;
        m_lastErrorString = "等待响应超时";
        emit errorOccurred(m_lastError, m_lastErrorString);
    }
    else
    {
        QEventLoop loop;
        connect(this, &SerialPortWrapper::frameReceived, &loop, &QEventLoop::quit);
        connect(m_responseTimer, &QTimer::timeout, &loop, &QEventLoop::quit);
        m_responseTimer->setSingleShot(true);
        m_responseTimer->start(m_config.readTimeout);
        loop.exec();
        m_responseTimer->stop();
    }
    return m_readBuffer;
}

// 错误处理
void SerialPortWrapper::HandleError(QSerialPort::SerialPortError error)
{
    if (error == QSerialPort::NoError)
    {
        return;
    }
    m_lastError = static_cast<SerialError>(error);
    m_lastErrorString = m_serialPort->errorString();
    QString errorMsg;
    switch (error)
    {
    case QSerialPort::DeviceNotFoundError:
        errorMsg = "设备未找到";
        break;
    case QSerialPort::PermissionError:
        errorMsg = "权限不足";
        break;
    case QSerialPort::OpenError:
        errorMsg = "打开失败";
        break;
    case QSerialPort::WriteError:
        errorMsg = "写入错误";
        break;
    case QSerialPort::ReadError:
        errorMsg = "读取错误";
        break;
    case QSerialPort::ResourceError:
        errorMsg = "资源错误（设备被移除）";
        ClosePort();
        break;
    case QSerialPort::UnsupportedOperationError:
        errorMsg = "不支持的操作";
        break;
    case QSerialPort::TimeoutError:
        errorMsg = "操作超时";
        break;
    default:
        errorMsg = "未知错误";
    }
    emit errorOccurred(m_lastError, errorMsg);
    emit debugMessage(QString("串口错误: %1").arg(errorMsg));
}


bool SerialPortWrapper::SetBaudRate(qint32 baudRate)
{
    if (IsOpen() && !m_serialPort->setBaudRate(baudRate))
    {
        m_lastError = ConfigError;
        m_lastErrorString = "设置波特率失败";
        return false;
    }
    m_config.baudRate = baudRate;
    return true;
}
bool SerialPortWrapper::SetPortName(const QString &portName)
{
    if (IsOpen())
    {
        m_lastError = ConfigError;
        m_lastErrorString = "串口打开时不能修改端口名";
        return false;
    }
    m_config.portName = portName;
    m_serialPort->setPortName(portName);
    return true;
}
bool SerialPortWrapper::SetDataBits(QSerialPort::DataBits dataBits)
{
    if (IsOpen() && !m_serialPort->setDataBits(dataBits))
    {
        m_lastError = ConfigError;
        m_lastErrorString = "设置数据位失败";
        return false;
    }
    m_config.dataBits = dataBits;
    return true;
}
bool SerialPortWrapper::SetParity(QSerialPort::Parity parity)
{
    if (IsOpen() && !m_serialPort->setParity(parity))
    {
        m_lastError = ConfigError;
        m_lastErrorString = "设置校验位失败";
        return false;
    }
    m_config.parity = parity;
    return true;
}
bool SerialPortWrapper::SetStopBits(QSerialPort::StopBits stopBits)
{
    if (IsOpen() && !m_serialPort->setStopBits(stopBits))
    {
        m_lastError = ConfigError;
        m_lastErrorString = "设置停止位失败";
        return false;
    }
    m_config.stopBits = stopBits;
    return true;
}
SerialPortWrapper::SerialConfig SerialPortWrapper::CurrentConfig() const
{
    return m_config;
}

QString SerialPortWrapper::GetLastErrorString() const
{
    return m_lastErrorString;
}
SerialPortWrapper::SerialError SerialPortWrapper::GetLastError() const
{
    return m_lastError;
}
void SerialPortWrapper::ClearError()
{
    m_lastError = NoError;
    m_lastErrorString.clear();
}
