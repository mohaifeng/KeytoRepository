#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QDebug>  // 添加这行
#include <QMessageBox>
#include <QThread>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    // 初始扫描串口
    RefreshSerialPorts();
    // 连接定时器信号到刷新槽
    connect(SerialRefreshTimer, &QTimer::timeout, this, &MainWindow::RefreshSerialPorts);
    // 设置定时器，每1秒刷新一次
    SerialRefreshTimer->start(1000);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::RefreshSerialPorts()
{
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
    else
    {
        ui->SerialcomboBox->setEnabled(false);
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
            qDebug() << "写入失败:" << SerialPort->errorString();
            return false;
        }
        // 确保数据被发送
        if (SerialPort->waitForBytesWritten(1000))
        {
            qDebug() << "数据发送成功:" << bytesWritten << "字节";
            return true;
        }
        else
        {
            qDebug() << "等待写入超时";
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
    // 根据你的协议判断数据是否完整
    // 示例1：简单的长度检查
    if (data.length() >= 4)
    { // 假设应答至少4字节
        return true;
    }
    return false;
}

void MainWindow::onReadyRead()
{
    m_receivedData.append(SerialPort->readAll());
    qDebug() << "收到数据:" << m_receivedData.toHex(' ');

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

    if (!m_responseReceived) {
        qDebug() << "等待应答超时";
        return QByteArray();
    }

    qDebug() << "收到应答数据:" << m_receivedData.toHex(' ');
    return m_receivedData;
}

void MainWindow::on_pushButton_clicked()
{
    QString buttonText = ui->pushButton->text();
    if (buttonText == "打开串口")
    {
        if(OpenSerialPort(sernum,baudrate))
        {
            ui->pushButton->setText("关闭串口");
            qDebug() << "串口" << sernum << "打开成功";
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
        qDebug() << "串口已关闭";
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
    qDebug() << "串口号:" << arg1;
    sernum=arg1;
}

void MainWindow::on_BaudratecomboBox_activated(const QString &arg1)
{
    qDebug() << "波特率:" << arg1;
    bool isok;
    baudrate=arg1.toInt(&isok);
    if (!isok)
    {
        qDebug() << "转换失败";
    }
}

void MainWindow::on_CheckAddr_clicked()
{
    QString buttonText = ui->CheckAddr->text();
    if(buttonText=="查询")
    {
        QTimer::singleShot(GAP_TIME, this, &MainWindow::onTimerTimeout);
        // 更新按钮状态
        ui->CheckAddr->setEnabled(true);
        ui->CheckAddr->setText("停止");
    }
    else
    {
        // 更新按钮状态
        ui->CheckAddr->setEnabled(true);
        ui->CheckAddr->setText("查询");
    }
}

// 定时器超时槽函数 - 每秒执行一次
void MainWindow::onTimerTimeout()
{
    if(addr<=10)
    {
        QString buttonText = ui->CheckAddr->text();
        if(buttonText == "停止")
        {
            SendString(QString::number(addr));
            ui->AddrlineEdit->setText(QString::number(addr));
            addr++;
            QTimer::singleShot(GAP_TIME, this, &MainWindow::onTimerTimeout);
        }
    }
    else
    {
        // 信息提示
        ShowWarningDialog("没有扫描到地址");
        addr=0;
        // 更新按钮状态
        ui->CheckAddr->setEnabled(false);
        ui->CheckAddr->setEnabled(true);
        ui->CheckAddr->setText("查询");
    }
}

void MainWindow::on_AddrlineEdit_textEdited(const QString &arg1)
{
    targetaddr=arg1.toInt();
    qDebug()<<targetaddr;
}
