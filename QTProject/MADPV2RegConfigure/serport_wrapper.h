#ifndef SERPORT_WRAPPER_H
#define SERPORT_WRAPPER_H

#include <QSerialPort>
#include <QSerialPortInfo>
#include <QTimer>
#include <QMutex>

class SerialPortWrapper: public QObject
{
    Q_OBJECT
public:
    //定义数据完整性检查函数指针类型
    typedef bool (*FrameIntegrityCheckCallback)(QByteArray &data);
    // 串口配置结构体
    struct SerialConfig
    {
        QString portName;
        qint32 baudRate ;
        QSerialPort::DataBits dataBits;
        QSerialPort::Parity parity;
        QSerialPort::StopBits stopBits;
        QSerialPort::FlowControl flowControl;
        int readTimeout;      // 读取超时(ms)
        int writeTimeout;     // 写入超时(ms)
        int responseTimeout;  // 响应超时(ms)
    };
    // 错误枚举
    enum SerialError
    {
        NoError = 0,
        PortNotOpen,
        OpenFailed,
        WriteFailed,
        ReadFailed,
        TimeoutError,
        ConfigError,
        IntegrityError
    };
    Q_ENUM(SerialError)  // 重要：添加这一行
public:
    explicit SerialPortWrapper(QObject *parent = nullptr);
    ~SerialPortWrapper();
    //基础功能
    bool OpenPort(const SerialConfig &config);
    void ClosePort();
    bool IsOpen() const;
    //获取可用串口
    QList<QSerialPortInfo> GetAvailablePorts();
    QStringList GetAvailablePortNames();
    //读写操作
    bool ByteArrayTransmit(const QByteArray &data);
    QByteArray ReadData(qint64 maxSize = 1024);
    QByteArray ReadAll();
    qint64 GetBytesAvailable() const;
    //同步读写带超时
    QByteArray ByteArrayTransmitWaitAck(const QByteArray &data, int timeout);
    QByteArray WaitForResponse(int timeout);
    // 配置操作
    bool SetBaudRate(qint32 baudRate);
    bool SetPortName(const QString &portName);
    bool SetDataBits(QSerialPort::DataBits dataBits);
    bool SetParity(QSerialPort::Parity parity);
    bool SetStopBits(QSerialPort::StopBits stopBits);
    SerialConfig CurrentConfig() const;
    // 错误处理
    QString GetLastErrorString() const;
    SerialError GetLastError() const;
    void ClearError();
    //注册回调函数
    void RegisterFrameIntegrityCheckCallback(FrameIntegrityCheckCallback callback);
private slots:
    void OnReadyRead();
    void ResponseFinish();
    void HandleError(QSerialPort::SerialPortError error);
private:
    //内部方法
    void SetupConnections();
    bool WaitForBytesWritten(int timeout);
private:
    // 创建串口对象
    QSerialPort *m_serialPort;
    SerialConfig m_config;
    // 数据缓冲区
    QByteArray m_readBuffer;
    // 错误处理
    SerialError m_lastError;
    QString m_lastErrorString;
    // 同步操作
    QMutex m_readMutex;
    QMutex m_writeMutex;
    QTimer *m_responseTimer = new QTimer(this);
    QTimer *m_responsefinishTimer = new QTimer(this);
    // 统计信息
    qint64 m_bytesReceived;
    qint64 m_bytesSent;
    // 帧完整性检查标志
    bool frameintegritycheckenable;
    bool responsefinishflag;
    // 成员变量 - 函数指针
    FrameIntegrityCheckCallback m_callback;
signals:
    // 数据相关信号
    void dataReceived(const QByteArray &data);
    void dataSent(const QByteArray &data);
    void frameReceived(const QByteArray &frame);  // 完整帧数据
    // 状态信号
    void portOpened(const QString &portName);
    void portClosed();
    void errorOccurred(SerialPortWrapper::SerialError error, const QString &errorString);
    // 调试信号
    void debugMessage(const QString &message);
};

#endif // SERPORT_WRAPPER_H
