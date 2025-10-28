#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QDebug>  // 添加这行
#include <QMessageBox>
#include <QThread>
#include <QDateTime>
#include <QString>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    // 初始扫描串口
    RefreshSerialPorts();//串口号开始刷新
    PushButtonConfigSetEnabled(false);//按钮全部无法点击，等待串口开启后解锁
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


void MainWindow::PushButtonConfigSetEnabled(bool isok)
{
    // 遍历处理
    for (QPushButton *button : ControledButtons)
    {
        button->setEnabled(isok);
    }
}

quint8 last_sernum=0;//上一次串口数目
void MainWindow::RefreshSerialPorts()
{
    if(!SerialPort->isOpen())
    {
        PushButtonConfigSetEnabled(false);
    }
    // 获取当前选中的串口（用于保持选择）
    sernum = ui->SerialcomboBox->currentText();
    baudrate = ui->BaudratecomboBox->currentText().toInt();
    QList<QSerialPortInfo> portList = QSerialPortInfo::availablePorts();
    if (portList.isEmpty())
    {
        ui->SerialcomboBox->clear();
        return;
    }
    quint8 currentsersize=portList.size();
    if(currentsersize!=last_sernum)
    {
        last_sernum=currentsersize;
        // 清空ComboBox
        ui->SerialcomboBox->clear();
        // 获取当前可用串口
        int selectIndex = -1;
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
        // 设置选择
        if (selectIndex >= 0)
        {
            ui->SerialcomboBox->setCurrentIndex(selectIndex);
        }
        else
        {
            // 如果之前的选择不存在，选择第一个
            ui->SerialcomboBox->setCurrentIndex(0);
        }
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
            QString logEntry = QString("[%1]: %2").arg("Tx").arg(hexString);
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
    if (data.size() >= 6)
    {
        if(modbus->TwoBytesToQuint16(data.right(2), false) == modbus->calculateCRC16_Modbus(data.chopped(2)))
        {
            if (static_cast<quint8>(data[0]) == modbus->addr&&static_cast<quint8>(data[1])==modbus->cmd)
            {
                QString hexString = data.toHex(' ').toUpper();
                QString logEntry = QString("[%1]: %2").arg("Rx").arg(hexString);
                LogPrint(logEntry);
                return true;
            }
        }
    }
    return false;
}
//有串口数据时触发该槽函数
void MainWindow::onReadyRead()
{
    m_receivedData.append(SerialPort->readAll());
    // 这里可以添加应答数据完整性检查
    if (isResponseComplete(m_receivedData))
    {
        m_responseReceived = true;
        emit responseReady();  // 在这里发出信号！
    }
}

QByteArray MainWindow::WaitResponse(quint16 timeoutMs=100)
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
    timeoutTimer.stop();
    if (!m_responseReceived)
    {
        return QByteArray();
    }
    return m_receivedData;
}

//日志输出函数
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
        modbus->cmd=0x03;
        modbus->regaddr=0xF2;
        modbus->regnum=1;
        QTimer::singleShot(GAP_TIME, this, &MainWindow::ScanfAddrLoop);
        ui->VersionlineEdit->clear();
        ui->CheckAddr->setText("停止");
    }
    else
    {
        firstscanf=true;
        ui->CheckAddr->setText("查询");

    }
}

quint8 tmpaddr=0;
// 定时器超时槽函数 -执行后若没有扫描到地址间隔50ms再次执行
void MainWindow::ScanfAddrLoop()
{
    QString buttonText = ui->CheckAddr->text();
    if(buttonText == "停止")
    {
        if(firstscanf)
        {
            modbus->addr=255;
            tmpaddr=0;
            firstscanf=false;
        }
        else if(tmpaddr<255)
        {
            modbus->addr=tmpaddr;
            tmpaddr++;
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
        QByteArray sendmsg = modbus->ModbusSenddataConfig();
        ui->AddrlineEdit->setText(QString::number(modbus->addr));
        SendData(sendmsg);
        QByteArray resp= WaitResponse();
        if(!resp.isEmpty())
        {
            LogPrint("扫描到地址");
            ui->VersionlineEdit->setText(resp.mid(3,2).toHex());
            ui->CheckAddr->setText("查询");
            firstscanf=true;
        }
        else
        {
            QTimer::singleShot(GAP_TIME, this, &MainWindow::ScanfAddrLoop);
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
/*单步执行命令相关函数*/
/****************************************************************/

void MainWindow::on_CmdSendpushButton_clicked()
{
    QString RegText = ui->RegAddrlineEdit->text();//获取寄存器地址
    QString NumText = ui->WriteNumlineEdit->text();//获取读写个数
    QString DataText = ui->DatalineEdit->text();//获取数据
    if(RegText.isEmpty())
    {
        ShowWarningDialog("请填写寄存器地址！");
        return;
    }
    if(NumText.isEmpty())
    {
        modbus->regnum=1;
    }
    if(DataText.isEmpty())
    {
        modbus->databuff=0;
        modbus->single_data=0;
    }
    /*获取选择的寄存器命令*/
    int cmd_idex=ui->ReadWritecomboBox->currentIndex();
    switch (cmd_idex)
    {
    case 0:modbus->cmd=0x03;
        break;
    case 1:modbus->cmd=0x06;
        break;
    case 2:modbus->cmd=0x10;
        break;
    default:
        break;
    }
    /******************/
    /*处理寄存器地址*/
    bool ok;
    modbus->regaddr = RegText.toUShort(&ok, 16);  // 十六进制
    if(!ok)
    {
        ShowWarningDialog("寄存器地址错误");
        return;
    }
    /******************/
    /*处理读写寄存器个数*/
    modbus->regnum = NumText.toUShort(&ok);  // 默认就是10进制
    if(!ok)
    {
        ShowWarningDialog("寄存器个数错误");
        return;
    }
    /******************/
    /*处理数据*/
    if(modbus->cmd==0x10)
    {
        qint16 decimalValue;
        modbus->databuff.clear();
        QStringList numbers = DataText.split(',',QString::SkipEmptyParts,Qt::CaseInsensitive);
        switch (ui->HexcheckBox->checkState())
        {
        case 0:
            if(DataText.contains(','))
            {
                for (const QString &numStr : numbers)
                {
                    decimalValue = numStr.toInt(&ok);
                    modbus->Appendint16BigEndian(modbus->databuff,decimalValue);
                }
            }
            else
            {
                decimalValue=DataText.toInt(&ok);
                modbus->Appendint16BigEndian(modbus->databuff,decimalValue);
            }
            break;
        case 2:
            if(DataText.contains(','))
            {
                for (const QString &numStr : numbers)
                {
                    modbus->databuff.append(QByteArray::fromHex(numStr.toLatin1()));
                }
            }
            else
            {
                modbus->databuff.append(QByteArray::fromHex(DataText.toLatin1()));
            }
            break;
        default:
            break;
        }
    }
    else
    {
        switch (ui->HexcheckBox->checkState())
        {
        case 0:
            modbus->single_data=DataText.toUShort(&ok);
            break;
        case 2:
            modbus->single_data=DataText.toUShort(&ok,16);
            break;
        default:
            break;
        }
    }
    /******************/
    modbus->ModbusSenddataConfig();
    SendData(modbus->send_buff);
    QByteArray resp= WaitResponse();
    if(resp.isEmpty())
    {
        LogPrint("应答超时");
    }
}
/**********************************************/
void MainWindow::on_StartReadpushButton_clicked()
{
    QTableWidgetItem *tmp=ui->ReadMultableWidget->currentItem();
}



/**********************************************/


