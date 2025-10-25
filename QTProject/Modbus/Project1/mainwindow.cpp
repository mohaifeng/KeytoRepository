#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QDebug>  // 添加这行
#include <QMessageBox>
#include <QThread>
#include <QDateTime>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    // 初始扫描串口
    RefreshSerialPorts();
    setModbus(new Modbus());
    PushButtonConfigSetEnabled(false);
    // 连接定时器信号到刷新槽
    connect(SerialRefreshTimer, &QTimer::timeout, this, &MainWindow::RefreshSerialPorts);
    // 当串口的接收缓冲区有数据时，readyRead信号会被发射
    connect(SerialPort, &QSerialPort::readyRead, this, &MainWindow::onReadyRead);
    RegisterPushButton();//注册需被控制按钮；
    // 设置定时器，每100ms刷新一次
    SerialRefreshTimer->start(100);
    // 连接状态管理信号
    // 初始状态：未连接
    ui->LogplainTextEdit->setLineWrapMode(QPlainTextEdit::WidgetWidth);//日志框自动换行
    modbus = new Modbus();  // 使用Qt对象树
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::RegisterPushButton()//需被控制按钮注册函数，通过删除不需要被控制的按钮，比较方便
{
    ControledButtons = findChildren<QPushButton*>();
    ControledButtons.removeOne(ui->pushButton);
    ControledButtons.removeOne(ui->ClearLogpushButton);
}

void MainWindow::setModbus(Modbus *m)
{
    modbus = m;
}


void MainWindow::PushButtonConfigSetEnabled(bool isok)
{
    // 遍历处理
    for (QPushButton *button : ControledButtons)
    {
        button->setEnabled(isok);
    }
}

void MainWindow::RefreshSerialPorts()
{
    if(!SerialPort->isOpen())
    {
        PushButtonConfigSetEnabled(false);
    }
    // 获取当前选中的串口（用于保持选择）
    sernum = ui->SerialcomboBox->currentText();
    baudrate = ui->BaudratecomboBox->currentText().toInt();
    // 清空ComboBox
    ui->SerialcomboBox->clear();
    // 获取当前可用串口
    int selectIndex = -1;
    QList<QSerialPortInfo> portList = QSerialPortInfo::availablePorts();
    if (!portList.isEmpty())
    {
        // 添加可用串口到ComboBox
        foreach (const QSerialPortInfo &portInfo, portList)
        {
            ui->SerialcomboBox->addItem(portInfo.portName());
            // 检查是否是之前选择的端口
            if (portInfo.portName() == sernum)
            {
                selectIndex = ui->SerialcomboBox->count() - 1;
            }
        }
    }
    // 设置选择
    if (selectIndex >= 0)
    {
        ui->SerialcomboBox->setCurrentIndex(selectIndex);
    }
    else if (!portList.isEmpty())
    {
        // 如果之前的选择不存在，选择第一个
        ui->SerialcomboBox->setCurrentIndex(0);
    }
}

bool MainWindow::OpenSerialPort(const QString &portName,const int &baudrate)
{
    SerialPort->setPortName(portName);
    SerialPort->setBaudRate(baudrate);
    SerialPort->setDataBits(QSerialPort::Data8);
    SerialPort->setParity(QSerialPort::NoParity);
    SerialPort->setStopBits(QSerialPort::OneStop);
    SerialPort->setFlowControl(QSerialPort::NoFlowControl);
    if (SerialPort->open(QIODevice::ReadWrite))
    {
        return true;
    }
    else
    {
        return false;
    }
}
//关闭串口
void MainWindow::CloseSerialPort()
{
    if (SerialPort->isOpen())
    {
        SerialPort->close();
    }
}

bool MainWindow::SendData(const QByteArray &data)
{
    if (SerialPort->isOpen() && SerialPort->isWritable())
    {
        qint64 bytesWritten = SerialPort->write(data);
        if (bytesWritten == -1||bytesWritten != data.size())
        {
            LogPrint("发送失败");
            return false;
        }
        // 确保数据被发送
        if (SerialPort->waitForBytesWritten(1000))
        {
            QString hexString = data.toHex(' ').toUpper();
            QString logEntry = QString("[%1]: %2").arg("Tx:").arg(hexString);
            LogPrint(logEntry);
            return true;
        }
        else
        {
            LogPrint("等待写入超时");
            return false;
        }
    }
    return false;
}

void MainWindow::SendString(const QString &text)
{
    QByteArray data = text.toUtf8();  // 或者使用 toLocal8Bit()
    SendData(data);
}

QByteArray MainWindow::hexStringToByteArray(const QString &hexString)
{
    QByteArray data;
    QString hex = hexString.trimmed();
    hex = hex.replace(" ", "");
    if (hex.length() % 2 != 0)
    {
        return QByteArray();
    }
    for (int i = 0; i < hex.length(); i += 2) {
        bool ok;
        char byte = static_cast<char>(hex.mid(i, 2).toInt(&ok, 16));
        if (!ok) {
            return QByteArray();
        }
        data.append(byte);
    }
    return data;
}

void MainWindow::SendHex(const QString &text)
{
    QByteArray data = hexStringToByteArray(text);
    if (!data.isEmpty())
    {
        SendData(data);
    }
}

bool MainWindow::isResponseComplete(const QByteArray &data)
{
    quint8 firstchar =static_cast<quint8>(data[0]);
    if (static_cast<quint8>(data[0])==modbus->addr)
    {

    }

    if (data.length() >= 4)
    { // 假设应答至少4字节
        return true;
    }
    return false;
}
//有串口数据时触发该槽函数
void MainWindow::onReadyRead()
{
    m_receivedData.append(SerialPort->readAll());
    qDebug() << "收到数据:" << m_receivedData.toHex(' ');
    LogPrint(m_receivedData);
    // 这里可以添加应答数据完整性检查
    if (isResponseComplete(m_receivedData)) {
        m_responseReceived = true;
        emit responseReady();  // 在这里发出信号！
    }
}

QByteArray MainWindow::WaitResponse(quint16 timeoutMs=500)
{
    // 清空接收缓冲区
    SerialPort->readAll();
    m_receivedData.clear();
    m_responseReceived = false;
    // 创建事件循环等待应答
    QEventLoop loop;
    QTimer timeoutTimer;
    connect(&timeoutTimer, &QTimer::timeout, &loop, &QEventLoop::quit);
    connect(this, &MainWindow::responseReady, &loop, &QEventLoop::quit);
    timeoutTimer.setSingleShot(true);
    timeoutTimer.start(timeoutMs);
    // 等待应答或超时
    loop.exec();
    if (!m_responseReceived)
    {
        return QByteArray();
    }
    qDebug() << "收到应答数据:" << m_receivedData.toHex(' ');
    return m_receivedData;
}

void MainWindow::LogPrint(const QString &text)
{
    QString timestamp = QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss.zzz");
    QString logEntry = QString("[%1]:%2").arg(timestamp).arg(text);

    ui->LogplainTextEdit->appendPlainText(logEntry);
}
//打开串口槽函数
void MainWindow::on_pushButton_clicked()
{
    QString buttonText = ui->pushButton->text();
    if (buttonText == "打开串口")
    {
        if(OpenSerialPort(sernum,baudrate))
        {
            ui->pushButton->setText("关闭串口");
            LogPrint("串口打开成功");
            PushButtonConfigSetEnabled(true);
        }
        else
        {
            // 信息提示
            ShowWarningDialog(SerialPort->errorString() );
        }
    }
    else
    {
        CloseSerialPort();
        LogPrint("串口关闭成功");
        ui->pushButton->setText("打开串口");
    }

}
// 警告框
void MainWindow::ShowWarningDialog(const QString &arg1 )
{
    QMessageBox::warning(nullptr, "警告", arg1);
}

void MainWindow::on_SerialcomboBox_activated(const QString &arg1)
{
    sernum=arg1;
}
//波特率选择槽函数
void MainWindow::on_BaudratecomboBox_activated(const QString &arg1)
{
    bool isok;
    baudrate=arg1.toInt(&isok);
}
//查询地址槽函数
void MainWindow::on_CheckAddr_clicked()
{  
    QString buttonText = ui->CheckAddr->text();
    if(buttonText == "查询")
    {
        qDebug()<<"进程进入";
        modbus->cmd=0x03;
        modbus->regaddr=0xF2;
        modbus->regnum=1;
        qDebug()<<"test1";
        QTimer::singleShot(GAP_TIME, this, &MainWindow::ScanfAddrLoop);
        qDebug()<<"test2";
        ui->CheckAddr->setText("停止");
    }
    else
    {
        ui->CheckAddr->setText("查询");
    }
}

// 定时器超时槽函数 -执行后若没有扫描到地址间隔50ms再次执行
void MainWindow::ScanfAddrLoop()
{
    QString buttonText = ui->CheckAddr->text();
    if(buttonText == "停止")
    {
        if(firstscanf)
        {
            modbus->addr=255;
            QByteArray sendmsg = modbus->ModbusSenddataConfig();
            ui->AddrlineEdit->setText(QString::number(modbus->addr));
            SendData(sendmsg);
            QByteArray resp= WaitResponse();
            if(!resp.isEmpty())
            {
                LogPrint("扫描到地址");
                return;
            }
            else
            {
                modbus->addr=0;
                firstscanf=false;
            }
        }
        if(modbus->addr<255)
        {
            QString buttonText = ui->CheckAddr->text();

            QByteArray sendmsg = modbus->ModbusSenddataConfig();
            ui->AddrlineEdit->setText(QString::number(modbus->addr));
            SendData(sendmsg);
            QByteArray resp= WaitResponse();
            if(!resp.isEmpty())
            {
                LogPrint("扫描到地址");
                firstscanf=true;
                return;
            }
            else
            {
                modbus->addr++;
                QTimer::singleShot(GAP_TIME, this, &MainWindow::ScanfAddrLoop);
            }
        }
        else
        {
            // 信息提示
            ShowWarningDialog("没有扫描到地址");
            firstscanf=true;
            modbus->addr=0;
            // 更新按钮状态
            ui->CheckAddr->setEnabled(false);
            ui->CheckAddr->setEnabled(true);
            ui->CheckAddr->setText("查询");
        }
    }
}

void MainWindow::on_AddrlineEdit_textEdited(const QString &arg1)
{
    targetaddr=arg1.toInt();
}

void MainWindow::on_ClearLogpushButton_clicked()
{
    ui->LogplainTextEdit->clear();
}

/****************************************************************/









