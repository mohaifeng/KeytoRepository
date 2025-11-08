#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QDebug>  // 添加这行
#include <QMessageBox>
#include <QThread>
#include <QDateTime>
#include <QString>
#include <QElapsedTimer>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    // 初始扫描串口
    version="1.0.0";
    Versiong_Config(version);
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
    para_reg_lst={MAX_SPEED,MIN_SPEED,ACCELERATION,DECELERATION,RATED_CURRENT,MAX_CHANNEL,CAN_BAUDRATE,SER_BAUDRATE};
    send_reg_lst.clear();
    reg_lst.clear();
    resp_reg_data.clear();
    delaytime=100;
    ui->GapTimelineEdit->setText(QString::number(delaytime));
    is_seridle=false;
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

void MainWindow::Versiong_Config(QString ver)
{
    QString timestamp = QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss");
    QString fin_str=QString("Valve ModBus %1 [%2]").arg(ver).arg(timestamp);
    setWindowTitle(fin_str);//设置界面抬头
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
    valve->ser_baudrate = ui->BaudratecomboBox->currentIndex();
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
        is_seridle=true;
        return true;

    }
    else
    {
        is_seridle=false;
        return false;
    }
}
//关闭串口
void MainWindow::CloseSerialPort()
{
    if (SerialPort->isOpen())
    {
        SerialPort->close();
        is_seridle=false;
    }
}

bool MainWindow::SendData(const QByteArray &data)
{
    if (SerialPort->isOpen() && SerialPort->isWritable())
    {
        qint64 bytesWritten = SerialPort->write(data);
        if (bytesWritten == -1||bytesWritten != data.size())
        {
            LogPrint("发送失败",Log::WARNING);
            return false;
        }
        // 确保数据被发送
        if (SerialPort->waitForBytesWritten(1000))
        {
            QString hexString = data.toHex(' ').toUpper();
            QString logEntry = QString("[%1]: %2").arg("Tx").arg(hexString);
            LogPrint(logEntry,Log::TX);
            return true;
        }
        else
        {
            LogPrint("等待写入超时",Log::WARNING);
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

void MainWindow::SendHexString(const QString &text)
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
                LogPrint(logEntry,Log::RX);
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
    connect(&timeoutTimer, &QTimer::timeout, &loop, &QEventLoop::quit);//超时时间定时器
    connect(this, &MainWindow::responseReady, &loop, &QEventLoop::quit);//事件触发定时器
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
void MainWindow::LogPrint(const QString &text,Log::LogLevel level)
{
    log->LogPrint(ui->LogplainTextEdit,text,level);
}

//打开串口槽函数
void MainWindow::on_pushButton_clicked()
{
    QString buttonText = ui->pushButton->text();
    if (buttonText == "打开串口")
    {
        if(OpenSerialPort(sernum,valve->SerBaudRateSwitch(valve->ser_baudrate)))
        {
            ui->pushButton->setText("关闭串口");
            LogPrint("串口打开成功",Log::INFO);
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
        LogPrint("串口关闭成功",Log::INFO);
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
void MainWindow::on_BaudratecomboBox_activated(int index)
{
    valve->ser_baudrate = index;
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
            if(tmpaddr==2)
            {
                tmpaddr++;
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
            return;
        }
        QByteArray sendmsg = modbus->ModbusSenddataConfig();
        ui->AddrlineEdit->setText(QString::number(modbus->addr));
        SendData(sendmsg);
        QByteArray resp= WaitResponse();
        if(!resp.isEmpty())
        {
            LogPrint("扫描到地址",Log::INFO);
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
    bool ok;
    valve->target_addr=arg1.toInt(&ok,10);
    if(!ok)
    {
        ShowWarningDialog("地址超出范围:0-255");
    }
}

void MainWindow::on_ClearLogpushButton_clicked()
{
    ui->LogplainTextEdit->clear();
}
/*单步执行命令相关函数*/
/****************************************************************/

void MainWindow::on_CmdSendpushButton_clicked()
{
    ExecuteWhenIdle([this](){
        quint16 regaddr,regnum;
        quint8 cmd;
        QString RegText = ui->RegAddrlineEdit->text();//获取寄存器地址
        QString NumText = ui->WriteNumlineEdit->text();//获取读写个数
        QString DataText = ui->DatalineEdit->text();//获取数据
        if(RegText.isEmpty())
        {
            ShowWarningDialog("寄存器地址为空！");
            return;
        }
        if(NumText.isEmpty())
        {
            ShowWarningDialog("寄存器个数为空！");
            return;
        }
        if(DataText.isEmpty())
        {
            modbus->databuff=0;
            modbus->single_data=0;
        }

        /******************/
        /*处理寄存器地址*/
        bool ok;
        regaddr = RegText.toUShort(&ok, 16);  // 十六进制
        if(!ok)
        {
            ShowWarningDialog("寄存器地址错误");
            return;
        }
        /******************/
        /*处理读写寄存器个数*/
        regnum = NumText.toUShort(&ok,10);  // 默认就是10进制
        if(!ok)
        {
            ShowWarningDialog("寄存器个数错误");
            return;
        }
        /******************/
        /*处理数据*/
        /*获取选择的寄存器命令*/
        quint16 single_data;
        QByteArray data_buff;
        switch (ui->ReadWritecomboBox->currentIndex())
        {
        case 0:
            cmd=0x03;
            Modbus_ReadRegWaitAck(cmd,regaddr,regnum,RX_TIMEOUT);
            break;
        case 1:
            cmd=0x06;
            single_data=QstringToUint16(DataText,ui->HexcheckBox->checkState());
            Modbus_WriteSingleRegWaitAck(cmd,regaddr,single_data,RX_TIMEOUT);
            break;
        case 2:
            cmd=0x10;
            data_buff=QstringToQbytearray(DataText,ui->HexcheckBox->checkState());
            Modbus_WriteMultiRegWaitAck(cmd,regaddr,data_buff,RX_TIMEOUT);
            break;
        }
    });
}

quint16 MainWindow::QstringToUint16(QString &data,int type)
{
    quint16 single_data=0;
    bool ok;
    switch (type)
    {
    case 0:
        single_data=data.toUShort(&ok,type);
        break;
    case 2:
        single_data=data.toUShort(&ok,type);
        break;
    default:
        single_data=0;
        break;
    }
    return single_data;
}

QByteArray MainWindow::QstringToQbytearray(QString &data,int type)
{
    QByteArray data_buff=0;
    switch (type)
    {
    case 0://10进制
        AppendMultiDataToDataBuff(data_buff,data,10);
        break;
    case 2://16进制
        AppendMultiDataToDataBuff(data_buff,data,16);
    default:
        break;
    }
    return data_buff;
}

void MainWindow::AppendMultiDataToDataBuff(QByteArray &databuff, QString &datatext,int type)
{
    qint16 decimalValue;
    bool ok;
    databuff.clear();
    QStringList numbers;
    if(datatext.contains(','))
    {
        numbers= datatext.split(',',QString::SkipEmptyParts,Qt::CaseInsensitive);
    }
    switch (type)
    {
    case 10:
        if(!numbers.isEmpty())
        {
            numbers= datatext.split(',',QString::SkipEmptyParts,Qt::CaseInsensitive);
            for (const QString &numStr : numbers)
            {
                decimalValue = numStr.toInt(&ok);
                modbus->Appendint16BigEndian(databuff,decimalValue);
            }
        }
        else
        {
            decimalValue=datatext.toInt(&ok);
            modbus->Appendint16BigEndian(databuff,decimalValue);
        }
        break;
    case 16:
        if(!numbers.isEmpty())
        {
            for (const QString &numStr : numbers)
            {
                databuff.append(QByteArray::fromHex(numStr.toLatin1()));
            }
        }
        else
        {
            databuff.append(QByteArray::fromHex(datatext.toLatin1()));
        }
        break;

    }
}
/**********************************************/
/**********************************************/

void MainWindow::GetRegList()
{
    bool ok;
    int rowCount = ui->ReadMultableWidget->rowCount();
    for (int row = 0; row < rowCount; ++row)
    {
        QTableWidgetItem* item = ui->ReadMultableWidget->item(row, 0); // 第0列是第一列
        if (item)
        {
            quint16 data=item->text().toInt(&ok,16);
            if(ok)
            {
                reg_lst.append(data);
            }
        }
    }
}

void MainWindow::RegListConfig()
{
    bool ok;
    startaddr = ui->RegStartlineEdit->text().toInt(&ok,16);
    readnum = ui->ReadNumlineEdit->text().toInt();
    for (quint32 i=0;i<readnum;i++)
    {
        reg_lst.append(startaddr++);
    }
}

//开始读取多个寄存器值
void MainWindow::on_StartReadpushButton_clicked()
{
    if(ui->StartReadpushButton->text()=="开始读取")
    {
        if(!ui->ReadMultableWidget->rowCount())
        {
            ShowWarningDialog("寄存器列表为空！");
            return;
        }
        switch(ui->LoopReadcheckBox->checkState())
        {
        case 0:
            SendRegListAck(send_reg_lst);
            break;
        case 2:
            QTimer::singleShot(delaytime, this, [this](){SendRegListAck(send_reg_lst);});
            ui->StartReadpushButton->setText("停止读取");
            break;
        default:
            break;
        }
    }
    else
    {
        ui->StartReadpushButton->setText("开始读取");
    }
}

void MainWindow::SendRegListAck(QList<QList<quint16>> &lst)
{
    if(is_seridle)
    {
        quint8 cmd=0x03;
        resp_reg_data.clear();
        for (const QList<quint16>& tmp_send : lst)
        {
            QByteArray resp=Modbus_ReadRegWaitAck(cmd,tmp_send[0],tmp_send[1],RX_TIMEOUT);
            if(resp.isEmpty())
            {
                LogPrint("应答超时",Log::WARNING);
                return;
            }
            QByteArray respdata=QByteArray::fromHex(resp.mid(3,resp[2]).toHex());
            for (int i = 0; i + 1 < respdata.size(); i += 2)
            {
                quint16 value = static_cast<quint8>(respdata[i]) << 8 | static_cast<quint8>(respdata[i + 1]);
                resp_reg_data.append(value);
            }
        }
        AddMultipleRows(ui->ReadMultableWidget,1,resp_reg_data,10);
    }
    if(ui->StartReadpushButton->text()=="停止读取")
    {
        QTimer::singleShot(delaytime, this, [this](){SendRegListAck(send_reg_lst);});
    }
}

void MainWindow::on_AddRowpushButton_clicked()
{
    bool ok;
    QList<quint16> tmp_data;
    tmp_data.clear();
    startaddr = ui->RegStartlineEdit->text().toInt(&ok,16);
    readnum = ui->ReadNumlineEdit->text().toInt();
    tmp_data.append(startaddr);
    tmp_data.append(readnum);
    send_reg_lst.append(tmp_data);
    for (quint16 i=0;i<readnum;i++)
    {
        reg_lst.append(startaddr++);
    }
    AddMultipleRows(ui->ReadMultableWidget,0,reg_lst,16);
}

//初始化一行所有的单元格
void MainWindow::InitRow(QTableWidget* tableWidget,int row)
{
    QString text = "";
    for (int col = 0; col < tableWidget->columnCount(); ++col)
    {
        QTableWidgetItem* item = new QTableWidgetItem(text);
        item->setTextAlignment(Qt::AlignCenter);
        tableWidget->setItem(row, col, item);
    }
}
// 在第col列添加多行字符串数据
void MainWindow::AddMultipleRows(QTableWidget* tableWidget, quint32 col,const QList<quint16>& data,int mode)
{
    for (int row = 0; row < data.size(); ++row)
    {
        QTableWidgetItem* item = tableWidget->item(row, col);
        if(item)
        {
            switch (mode)
            {
            case 10:
                item ->setText(QString::number(data.at(row)));
                break;
            case 16:
                item ->setText(QString("%1").arg(data.at(row), 4, 16, QChar('0')).toUpper());
                break;
            default:
                item ->setText(QString::number(data.at(row)));
                break;
            }
        }
        else
        {
            tableWidget->insertRow(row);
            // 初始化所有列
            InitRow(tableWidget,row);
            switch (mode)
            {
            case 10:
                item = new QTableWidgetItem(QString::number(data.at(row)));
                break;
            case 16:
                item = new QTableWidgetItem(QString("%1").arg(data.at(row), 4, 16, QChar('0')).toUpper());
                break;
            default:
                item = new QTableWidgetItem(QString("%1").arg(data.at(row), 4, 16, QChar('0')).toUpper());
                break;
            }
            item->setTextAlignment(Qt::AlignCenter);
            tableWidget->setItem(row, col, item);
        }
    }
}

void MainWindow::on_GapTimelineEdit_textEdited(const QString &arg1)
{
    bool ok;
    delaytime=arg1.toInt(&ok,10);
}

void MainWindow::on_ClearLinespushButton_clicked()
{
    if(ui->StartReadpushButton->text()=="停止读取")
    {
        ui->StartReadpushButton->setText("开始读取");
    }
    ui->ReadMultableWidget->clearContents();
    ui->ReadMultableWidget->setRowCount(0);
    reg_lst.clear();
    send_reg_lst.clear();
    resp_reg_data.clear();
}

/**********************************************/

void MainWindow::on_ReadPara_pushButton_clicked()
{
    ExecuteWhenIdle([this]() {
        for (quint8 i=0;i<para_reg_lst.size();i++)
        {
            QByteArray resp = Modbus_ReadRegWaitAck(0x03,para_reg_lst[i],1,RX_TIMEOUT);
            if (resp.isEmpty())
            {
                return;
            }
            ReadParaShow(para_reg_lst[i],modbus->ModbusSingleRegByteToInt(resp));
        }
    });
}


void MainWindow::ExecuteWhenIdle(std::function<void()> function)
{
    if (!is_seridle)
    {
        QTimer::singleShot(50, this, [this, function]() {ExecuteWhenIdle(function);});
        return;
    }

    // 串口空闲，执行传入的函数
    function();
}

void MainWindow::ReadParaShow(quint16 reg,quint16 data)
{
    int idex;
    switch (reg)
    {
    case MAX_SPEED:
        ui->MaxSpeedlineEdit->setText(QString::number(data));
        break;
    case MIN_SPEED:
        ui->MinSpeedlineEdit->setText(QString::number(data));
        break;
    case ACCELERATION:
        ui->AcclineEdit->setText(QString::number(data));
        break;
    case DECELERATION:
        ui->DeclineEdit->setText(QString::number(data));
        break;
    case RATED_CURRENT:
        ui->CurrentlineEdit->setText(QString::number(data));
        break;
    case MAX_CHANNEL:
        ui->MaxChlineEdit->setText(QString::number(data));
        break;
    case CAN_BAUDRATE:
        idex=ui->CANBaudRatecomboBox->findText(QString::number(data));
        if(idex !=-1)
        {
            ui->CANBaudRatecomboBox->setCurrentIndex(idex);
        }
        else
        {
            LogPrint("CAN波特率错误",Log::ERROR);
        }
        break;
    case SER_BAUDRATE:
        if(data < ui->SerBaudRatecomboBox->count())
        {
            ui->SerBaudRatecomboBox->setCurrentIndex(data);
        }
        else
        {
            LogPrint("串口波特率错误",Log::ERROR);
        }
        break;
    default:
        break;
    }
}

QByteArray MainWindow::Modbus_SendWaitAck(quint16 timeoutMs)
{
    modbus->ModbusSenddataConfig();
    is_seridle=false;
    SendData(modbus->send_buff);
    QByteArray resp= WaitResponse(timeoutMs);
    is_seridle=true;
    if(resp.isEmpty())
    {
        LogPrint("应答超时",Log::WARNING);
        return QByteArray();
    }
    return resp;
}

QByteArray MainWindow::Modbus_ReadRegWaitAck(quint8 cmd,quint16 reg , quint16 num,quint16 timeoutMs)
{
    modbus->cmd=cmd;
    modbus->regaddr = reg;
    modbus->regnum=num;
    return Modbus_SendWaitAck(timeoutMs);
}

QByteArray MainWindow::Modbus_WriteSingleRegWaitAck(quint8 cmd,quint16 reg , quint16 data,quint16 timeoutMs)
{
    modbus->cmd=cmd;
    modbus->regaddr = reg;
    modbus->single_data=data;
    return Modbus_SendWaitAck(timeoutMs);
}

QByteArray MainWindow::Modbus_WriteMultiRegWaitAck(quint8 cmd,quint16 reg , QByteArray &data,quint16 timeoutMs)
{
    modbus->cmd=cmd;
    modbus->regaddr = reg;
    modbus->databuff=data;
    return Modbus_SendWaitAck(timeoutMs);
}

void MainWindow::on_FastSwitchpushButton_clicked()
{
    ExecuteWhenIdle([this](){
        Modbus_WriteSingleRegWaitAck(0x06,OPTIMAL_SWITCH,valve->target_channel,RX_TIMEOUT);
    });
}

void MainWindow::on_TargetChlineEdit_textEdited(const QString &arg1)
{
    valve->target_channel=arg1.toInt();
}

void MainWindow::on_CounterclockwisepushButton_clicked()
{
    ExecuteWhenIdle([this](){
        Modbus_WriteSingleRegWaitAck(0x06,COUNTERCLOCKWISE_SWITCH,valve->target_channel,RX_TIMEOUT);
    });
}

void MainWindow::on_ClockwisepushButton_clicked()
{
    ExecuteWhenIdle([this](){
        Modbus_WriteSingleRegWaitAck(0x06,CLOCKWISE_SWITCH,valve->target_channel,RX_TIMEOUT);
    });
}

void MainWindow::on_InitpushButton_clicked()
{
    ExecuteWhenIdle([this](){
        Modbus_WriteSingleRegWaitAck(0x06,INITIALIZATION,0,RX_TIMEOUT);
    });
}

void MainWindow::on_StoppushButton_clicked()
{
    ExecuteWhenIdle([this](){
        Modbus_WriteSingleRegWaitAck(0x06,STOP,0,RX_TIMEOUT);
    });
}

void MainWindow::on_ClearErrorpushButton_clicked()
{
    ExecuteWhenIdle([this](){
        Modbus_WriteSingleRegWaitAck(0x06,CLEAR_ERROR,0,RX_TIMEOUT);
    });
}

quint16 MainWindow::ScanfReadPara(quint16 reg)
{
    quint16 resp=0;
    bool ok;
    switch (reg)
    {
    case MAX_SPEED:
        resp=ui->MaxSpeedlineEdit->text().toInt(&ok,10);
        break;
    case MIN_SPEED:
        resp=ui->MinSpeedlineEdit->text().toInt(&ok,10);
        break;
    case ACCELERATION:
        resp=ui->AcclineEdit->text().toInt(&ok,10);
        break;
    case DECELERATION:
        resp=ui->DeclineEdit->text().toInt(&ok,10);
        break;
    case RATED_CURRENT:
        resp=ui->CurrentlineEdit->text().toInt(&ok,10);
        break;
    case MAX_CHANNEL:
        resp=ui->MaxChlineEdit->text().toInt(&ok,10);
        break;
    case CAN_BAUDRATE:
        resp=ui->CANBaudRatecomboBox->currentText().toInt(&ok,10);
        break;
    case SER_BAUDRATE:
        resp=ui->SerBaudRatecomboBox->currentIndex();
        break;
    default:
        break;
    }
    return resp;
}

void MainWindow::on_SetParapushButton_clicked()
{
    ExecuteWhenIdle([this]() {
    foreach(const quint16 &reg, para_reg_lst)
    {
        QByteArray resp = Modbus_WriteSingleRegWaitAck(0x06,reg,ScanfReadPara(reg),RX_TIMEOUT);
        if (resp.isEmpty())
        {
            return;
        }
        if(modbus->send_buff !=resp)
        {
            LogPrint("写入失败",Log::WARNING);
            return;
        }
    }
    });
}

void MainWindow::on_SavepushButton_clicked()
{
    ExecuteWhenIdle([this](){Modbus_WriteSingleRegWaitAck(0x06,SAVE,0x1234,RX_TIMEOUT);});
}

void MainWindow::on_ResetpushButton_clicked()
{
    ExecuteWhenIdle([this](){Modbus_WriteSingleRegWaitAck(0x06,RESET,0x1234,RX_TIMEOUT);});
}

void MainWindow::on_ChangeAddr_clicked()
{
    ExecuteWhenIdle([this](){
        Modbus_WriteSingleRegWaitAck(0x06,ADDRESS,valve->target_addr,RX_TIMEOUT);
        modbus->addr = valve->target_addr;
    });
}
