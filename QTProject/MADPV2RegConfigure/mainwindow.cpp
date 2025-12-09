#include "mainwindow.h"
#include <QDebug>
#include <QMessageBox>
#include <QString>
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    RefreshSerialPorts(); // 刷新串口号
    ConfigInit();
    SerialRefreshTimer->start(500);
    json->Get_Json_Filepath(1);
    jsonobj = json->ReadJson(json->filepath);
    qDebug() << json->filepath;
    if (jsonobj.isEmpty())
    {
        qDebug() << "jsonobj.isEmpty()";
        return;
    }
    pser->RegisterFrameIntegrityCheckCallback(MainWindow::DataIntegrityCheckCallback);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::ConfigInit()
{
    // 串口配置
    serialconfig.dataBits = QSerialPort::Data8;                                // 数据位
    serialconfig.stopBits = QSerialPort::OneStop;                              // 停止位
    serialconfig.parity = QSerialPort::NoParity;                               // 校验位
    serialconfig.flowControl = QSerialPort::NoFlowControl;                     // 流控制
    serialconfig.writeTimeout=2000;                                            //写入超时
    serialconfig.responseTimeout=2000;                                         //应答超时
    on_SerialcomboBox_activated(ui->SerialcomboBox->currentIndex());           // 串口号
    on_serBaudRatecomboBox_activated(ui->serBaudRatecomboBox->currentIndex()); // 波特率
    // 连接信号与槽函数
    connect(SerialRefreshTimer,&QTimer::timeout,this,&MainWindow::RefreshSerialPorts); // 连接定时器信号到刷新槽
    connect(pser,&SerialPortWrapper::errorOccurred,this,&MainWindow::ErrorMsgPrint); // 连接定时器信号到刷新槽
}
// 日志输出函数
void MainWindow::LogMsgPrint(const QString &text,Log::LogLevel type)
{
    log->LogPrint(ui->LogplainTextEdit, text, type);
}

void MainWindow::ErrorMsgPrint(int errorcode, const QString &errorString)
{
    QString msg = QString("错误代码:%1，信息:%2").arg(QString::number(errorcode), errorString);
    log->LogPrint(ui->LogplainTextEdit, errorString, Log::ERROR);
}

// 警告框
void MainWindow::ShowWarningDialog(const QString &arg1)
{
    QMessageBox::warning(nullptr, "警告", arg1);
}

void MainWindow::RefreshSerialPorts()
{
    // 获取当前选中的串口（用于保持选择）
    currentsername = ui->SerialcomboBox->currentText();
    QStringList portname = pser->GetAvailablePortNames();
    if (portname.isEmpty())
    {
        ui->SerialcomboBox->clear();
        return;
    }
    quint8 sersize = portname.size();
    if (sersize != currentsernum)
    {
        currentsernum = sersize;
        // 清空ComboBox
        ui->SerialcomboBox->clear();
        // 添加可用串口到ComboBox
        foreach (const QString &portInfo, portname)
        {
            ui->SerialcomboBox->addItem(portInfo);
        }
        // 设置选择
        int idex = ui->SerialcomboBox->findText(currentsername);
        if (idex != -1)
        {
            ui->SerialcomboBox->setCurrentIndex(idex);
        } else {
            // 如果之前的选择不存在，选择第一个
            ui->SerialcomboBox->setCurrentIndex(0);
        }
    }
}

void MainWindow::on_serOpenpushButton_clicked()
{
    if (ui->serOpenpushButton->text() == "打开串口")
    {
        if (pser->OpenPort(serialconfig))
        {
            ui->serOpenpushButton->setText("关闭串口");
            LogMsgPrint(QString("%1打开成功，波特率:%2").arg(serialconfig.portName, QString::number(serialconfig.baudRate)),Log::INFO);
        }
        else
        {
            ShowWarningDialog(pser->GetLastErrorString());
        }
    }
    else
    {
        pser->ClosePort();
        LogMsgPrint(QString("%1关闭成功").arg(serialconfig.portName),Log::INFO);
        ui->serOpenpushButton->setText("打开串口");
    }
}

quint8 lastseridex;
quint8 lastbaudrateidex;
void MainWindow::on_SerialcomboBox_activated(int index)
{
    if (pser->IsOpen() && lastseridex != index)
    {
        ui->SerialcomboBox->setCurrentIndex(lastseridex);
        ShowWarningDialog("请先关闭串口！");
    }
    else
    {
        serialconfig.portName = ui->SerialcomboBox->itemText(index);
        lastseridex = index;
    }
}

void MainWindow::on_IplineEdit_textEdited(const QString &arg1)
{
    qDebug() << arg1;
}

void MainWindow::on_serBaudRatecomboBox_activated(int index)
{
    bool ok;
    if (pser->IsOpen() && lastseridex != index)
    {
        ui->serBaudRatecomboBox->setCurrentIndex(lastbaudrateidex);
        ShowWarningDialog("请先关闭串口！");
    }
    else
    {
        serialconfig.baudRate = ui->serBaudRatecomboBox->itemText(index).toInt(&ok, 10);
        lastbaudrateidex = index;
    }
}

void MainWindow::on_ClearLogpushButton_clicked()
{
    ui->LogplainTextEdit->clear();
}

void MainWindow::UserRegListAppend(
    int addr, const QString &func, const QString &rw, const QString &unit, int value)
{
    // 获取当前行数
    int rowCount = ui->UserRegtableWidget->rowCount();
    // 插入新行
    ui->UserRegtableWidget->insertRow(rowCount);
    // 设置新行的内容
    // 假设表格有5列
    QTableWidgetItem *item1 = new QTableWidgetItem(QString::number(addr));
    QTableWidgetItem *item2 = new QTableWidgetItem(func);
    QTableWidgetItem *item3 = new QTableWidgetItem(rw);
    QTableWidgetItem *item4 = new QTableWidgetItem(unit);
    QTableWidgetItem *item5 = new QTableWidgetItem(QString::number(value));

    // 设置项的对齐方式（可选）
    item1->setTextAlignment(Qt::AlignCenter);
    item2->setTextAlignment(Qt::AlignCenter);
    item3->setTextAlignment(Qt::AlignCenter);
    item4->setTextAlignment(Qt::AlignCenter);
    item5->setTextAlignment(Qt::AlignCenter);
    // 设置项是否可编辑
    item1->setFlags(item1->flags() & ~Qt::ItemIsEditable);
    item2->setFlags(item2->flags() & ~Qt::ItemIsEditable);
    item3->setFlags(item3->flags() & ~Qt::ItemIsEditable);
    item4->setFlags(item4->flags() & ~Qt::ItemIsEditable);
    // 将项添加到表格中
    ui->UserRegtableWidget->setItem(rowCount, 0, item1);
    ui->UserRegtableWidget->setItem(rowCount, 1, item2);
    ui->UserRegtableWidget->setItem(rowCount, 2, item3);
    ui->UserRegtableWidget->setItem(rowCount, 3, item4);
    ui->UserRegtableWidget->setItem(rowCount, 4, item5);
}

void MainWindow::UserNodeAppend(int nodeaddr, const QString &nodename)
{
    // 获取当前行数
    int rowCount = ui->NodetableWidget->rowCount();
    // 插入新行
    ui->NodetableWidget->insertRow(rowCount);
    QTableWidgetItem *item1 = new QTableWidgetItem(QString::number(nodeaddr));
    QTableWidgetItem *item2 = new QTableWidgetItem(nodename);
    // 设置项是否可编辑
    item1->setFlags(item1->flags() & ~Qt::ItemIsEditable);
    item2->setFlags(item2->flags() & ~Qt::ItemIsEditable);
    // 设置项的对齐方式（可选）
    item1->setTextAlignment(Qt::AlignCenter);
    item2->setTextAlignment(Qt::AlignCenter);
    // 将项添加到表格中
    ui->NodetableWidget->setItem(rowCount, 0, item1);
    ui->NodetableWidget->setItem(rowCount, 1, item2);
}

void MainWindow::on_ReadNodepushButton_clicked()
{
    QTimer::singleShot(50, this, &MainWindow::ScanfNodeLoop);
    ui->NodetableWidget->clear();
}
//检查数据完整性
bool MainWindow::DataIntegrityCheckCallback(QByteArray &data)
{
    QByteArray framedata;
    int start_idex = -1;
    int end_idex = -1;
    if (data.size() < 8)
    {
        return false;
    }
    for (int i = 0; i < data.size(); ++i)
    {
        if (static_cast<quint8>(data[i]) == 0xBB && start_idex == -1) // 找到帧头
        {
            start_idex = i;
        }
        if (static_cast<quint8>(data[i]) == 0xBC && start_idex != -1 && end_idex == -1) // 找到帧尾
        {
            end_idex = i;
            if (data.size() - 1 - i < 2) // 判断是否还剩下至少2个字节数据
            {
                return false;
            }
            break;
        }
    }
    if (start_idex < 0 || end_idex < 0||start_idex >= end_idex) // 判断是否找到帧头祯尾
    {
        return false;
    }
    framedata = data.mid(start_idex, end_idex - start_idex + 1);
    if ((static_cast<quint8>(data[end_idex + 1]) << 8 | static_cast<quint8>(data[end_idex + 2])) != calculateCRC16(framedata))
    {
        return false;
    }
    return true;
}

quint8 nodeaddr = 0;
void MainWindow::ScanfNodeLoop()
{
    if (++nodeaddr > 48)
    {
        ShowWarningDialog("未扫描到节点");
        nodeaddr = 0;
        return;
    }
    QByteArray sendmsg = moem->MoemSenddataConfig(0, 0, 'k', QString("%1Rp0").arg(nodeaddr).toUtf8());
    LogMsgPrint(sendmsg.toHex().toUpper(),Log::TX);
    QByteArray respmsg = pser->ByteArrayTransmitWaitAck(sendmsg, 500);
    // MOEM::MOEMResult result=moem->ResponseDataConfig(respmsg);

    if (!respmsg.isEmpty())
    {
        LogMsgPrint(respmsg.toHex().toUpper(),Log::RX);
    }
    QTimer::singleShot(50, this, &MainWindow::ScanfNodeLoop);
}
