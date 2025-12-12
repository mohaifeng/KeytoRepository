#include "mainwindow.h"
#include <QDebug>
#include <QMessageBox>
#include <QString>
#include "ui_mainwindow.h"
#include <QMap>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    RefreshSerialPorts(); // 刷新串口号
    ConfigInit();
    json->Get_Json_Filepath(1);
    jsonobj = json->ReadJson(json->filepath);
    qDebug() << json->filepath;
    if (jsonobj.isEmpty())
    {
        qDebug() << "jsonobj.isEmpty()";
        return;
    }
    pser->RegisterFrameIntegrityCheckCallback(MainWindow::DataIntegrityCheckCallback);
    pser->RegisterDataReceiveFinishCheckCallback(MainWindow::DataReceiveFinishCheckCallback);
}

MainWindow::~MainWindow()
{
    delete ui;
}
void MainWindow::MADPRegInit()
{
    RegRowAppend(ui->MADPRegtableWidget,0,"RS232串口波特率","RW","/");
    RegRowAppend(ui->MADPRegtableWidget,1,"RS485串口波特率","RW","/");
    RegRowAppend(ui->MADPRegtableWidget,2,"CAN波特率","RW","/");
    RegRowAppend(ui->MADPRegtableWidget,3,"软件版本","R","/");
    RegRowAppend(ui->MADPRegtableWidget,4,"启用动作完成帧","RW","/");
    RegRowAppend(ui->MADPRegtableWidget,5,"MADP地址","RW","/");
}

void MainWindow::ArmRegInit()
{
    RegRowAppend(ui->ArmUserRegtableWidget,1,"当前设备状态","RW","/");
    RegRowAppend(ui->ArmUserRegtableWidget,2,"当前位置","R","um");
    RegRowAppend(ui->ArmUserRegtableWidget,3,"当前速度","R","um/s");
    RegRowAppend(ui->ArmUserRegtableWidget,4,"当前编码器计数值","R","/");
    RegRowAppend(ui->ArmUserRegtableWidget,5,"近端光耦状态","R","/");
    RegRowAppend(ui->ArmUserRegtableWidget,6,"远端光耦状态","R","/");
    RegRowAppend(ui->ArmUserRegtableWidget,7,"最大行程","R","/");
    RegRowAppend(ui->ArmUserRegtableWidget,8,"拨码ID","R","/");
    RegRowAppend(ui->ArmUserRegtableWidget,9,"偏移ID","RW","/");
    RegRowAppend(ui->ArmUserRegtableWidget,45,"移动次数","RW","/");
    RegRowAppend(ui->ArmUserRegtableWidget,46,"移动全行程倍数","RW","/");
    RegRowAppend(ui->ArmUserRegtableWidget,47,"取TIP次数","RW","/");
    RegRowAppend(ui->ArmUserRegtableWidget,48,"置零次数","RW","/");

    RegRowAppend(ui->ArmFacRegtableWidget,1,"新旧协议切换","RW","/");
    RegRowAppend(ui->ArmFacRegtableWidget,2,"运行电流","RW","mA");
    RegRowAppend(ui->ArmFacRegtableWidget,3,"保持电流","RW","mA");
    RegRowAppend(ui->ArmFacRegtableWidget,4,"加速度","RW","ustep/s2");
    RegRowAppend(ui->ArmFacRegtableWidget,5,"减速度","RW","ustep/s2");
    RegRowAppend(ui->ArmFacRegtableWidget,6,"最大速度","RW","um/s");
    RegRowAppend(ui->ArmFacRegtableWidget,7,"电机运动方向","RW","/");
    RegRowAppend(ui->ArmFacRegtableWidget,8,"启动速度","RW","ustep/s");
    RegRowAppend(ui->ArmFacRegtableWidget,9,"截止速度","RW","ustep/s");
    RegRowAppend(ui->ArmFacRegtableWidget,10,"电机细分","RW","/");
    RegRowAppend(ui->ArmFacRegtableWidget,11,"最大行程","RW","um");
    RegRowAppend(ui->ArmFacRegtableWidget,12,"最小行程","RW","um");
    RegRowAppend(ui->ArmFacRegtableWidget,13,"初始化速度","RW","um/s");
    RegRowAppend(ui->ArmFacRegtableWidget,14,"编码器一圈计数值","RW","/");
    RegRowAppend(ui->ArmFacRegtableWidget,15,"导程","RW","um");
    RegRowAppend(ui->ArmFacRegtableWidget,16,"零位补偿值","RW","um");
    RegRowAppend(ui->ArmFacRegtableWidget,17,"背隙值","RW","um");
    RegRowAppend(ui->ArmFacRegtableWidget,18,"堵转阈值","RW","um");
    RegRowAppend(ui->ArmFacRegtableWidget,19,"取TIP堵转阈值","RW","um");
    RegRowAppend(ui->ArmFacRegtableWidget,20,"编码器计数方向","RW","/");
    RegRowAppend(ui->ArmFacRegtableWidget,21,"近端光耦触发电平","RW","/");
    RegRowAppend(ui->ArmFacRegtableWidget,22,"置零时离开零位的距离","RW","um");
    RegRowAppend(ui->ArmFacRegtableWidget,23,"置零时光耦触发偏差","RW","um");
    RegRowAppend(ui->ArmFacRegtableWidget,24,"运动中位置修正阈值","RW","um");
    RegRowAppend(ui->ArmFacRegtableWidget,25,"首次置零的速度","RW","um/s");
    RegRowAppend(ui->ArmFacRegtableWidget,26,"取到TIP后的等待时间","RW","ms");
    RegRowAppend(ui->ArmFacRegtableWidget,27,"编码器计数值","R","ms");

    RegRowAppend(ui->ArmCommonRegtableWidget,0,"设备型号","R","/");
    RegRowAppend(ui->ArmCommonRegtableWidget,1,"紧急停止","W","/");
    RegRowAppend(ui->ArmCommonRegtableWidget,2,"心跳间隔时间","RW","ms");
    RegRowAppend(ui->ArmCommonRegtableWidget,3,"设备重启","W","/");
    RegRowAppend(ui->ArmCommonRegtableWidget,4,"软件版本","R","/");
    RegRowAppend(ui->ArmCommonRegtableWidget,5,"CAN执行完成主动上报","RW","/");
    RegRowAppend(ui->ArmCommonRegtableWidget,6,"串口波特率","RW","/");
    RegRowAppend(ui->ArmCommonRegtableWidget,7,"CAN波特率","RW","/");
    //RegRowAppend(ui->ArmCommonRegtableWidget,8,"Debug串口波特率","RW","/");
    RegRowAppend(ui->ArmCommonRegtableWidget,9,"设备序列号","R","/");
    RegRowAppend(ui->ArmCommonRegtableWidget,10,"串口执行完成主动上报","RW","/");
    RegRowAppend(ui->ArmCommonRegtableWidget,11,"串口应答参数的长度","RW","/");

    // 调整列宽
    ui->ArmUserRegtableWidget->resizeColumnsToContents();
    ui->ArmFacRegtableWidget->resizeColumnsToContents();
    ui->ArmCommonRegtableWidget->resizeColumnsToContents();
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
    SerialRefreshTimer->start(500);

    connect(pser,&SerialPortWrapper::errorOccurred,this,&MainWindow::ErrorMsgPrint); // 连接定时器信号到刷新槽
    connect(pser,&SerialPortWrapper::dataSent,this,&MainWindow::MADPTxMsgPrint); // 连接定时器信号到刷新槽
    connect(pser,&SerialPortWrapper::frameReceived,this,&MainWindow::MADPRxMsgPrint); // 连接定时器信号到刷新槽
    connect(pser,&SerialPortWrapper::debugMessage,this,&MainWindow::DebugMsgPrint); // 连接定时器信号到刷新槽
    m_stopOperations=false;
    MADPRegInit();
    ArmRegInit();
}

void MainWindow::DebugMsgPrint(const QString &text)
{
    qDebug()<<"Info:"<<text;
}

// 日志输出函数
void MainWindow::LogMsgPrint(const QString &text,Log::LogLevel type)
{
    log->LogPrint(ui->LogplainTextEdit, text, type);
}

void MainWindow::MADPTxMsgPrint()
{
    MOEM::MOEMSend send=moem->GetMOEMSendStu();
    QString text=QString("[TX]:%1(%2):%4").arg(QString::number(send.group_ID),send.frametype,send.scriptbuffer);
    LogMsgPrint(text,Log::TX);
}

void MainWindow::MADPRxMsgPrint(const QByteArray &frame)
{
    MOEM::MOEMResult result = moem->ResponseDataConfig(frame);
    QString text=QString("[RX]:%1(%2 %3):%4").arg(QString::number(result.group_ID),result.frametype,result.madpstatus,result.responsestring);
    LogMsgPrint(text,Log::RX);
}

void MainWindow::ErrorMsgPrint(int errorcode, const QString &errorString)
{
    m_stopOperations = true;
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

void MainWindow::RegRowAppend(QTableWidget *tableWidget,int addr, const QString &func, const QString &rw, const QString &unit)
{
    // 获取当前行数
    int rowCount = tableWidget->rowCount();
    // 插入新行
    tableWidget->insertRow(rowCount);
    // 设置新行的内容
    // 假设表格有5列
    QTableWidgetItem *item1 = new QTableWidgetItem(QString::number(addr));
    QTableWidgetItem *item2 = new QTableWidgetItem(func);
    QTableWidgetItem *item3 = new QTableWidgetItem(rw);
    QTableWidgetItem *item4 = new QTableWidgetItem(unit);

    // 设置项的对齐方式（可选）
    item1->setTextAlignment(Qt::AlignCenter);
    item2->setTextAlignment(Qt::AlignCenter);
    item3->setTextAlignment(Qt::AlignCenter);
    item4->setTextAlignment(Qt::AlignCenter);
    // 设置项是否可编辑
    item1->setFlags(item1->flags() & ~Qt::ItemIsEditable);
    item2->setFlags(item2->flags() & ~Qt::ItemIsEditable);
    item3->setFlags(item3->flags() & ~Qt::ItemIsEditable);
    item4->setFlags(item4->flags() & ~Qt::ItemIsEditable);
    // 将项添加到表格中
    tableWidget->setItem(rowCount, 0, item1);
    tableWidget->setItem(rowCount, 1, item2);
    tableWidget->setItem(rowCount, 2, item3);
    tableWidget->setItem(rowCount, 3, item4);
}

void MainWindow::RegColAppend(QTableWidget *tableWidget,const QString &headerName)
{
    // 保存当前表头
    QStringList headers;
    int columnCount = tableWidget->columnCount();
    for (int i = 0; i < columnCount; ++i)
    {
        QTableWidgetItem* headerItem = tableWidget->horizontalHeaderItem(i);
        if (headerItem)
        {
            headers << headerItem->text();
        }
        else
        {
            headers << QString("Column %1").arg(i + 1);
        }
    }
    // 添加新列的表头
    headers << headerName;
    // 设置列数
    tableWidget->setColumnCount(columnCount + 1);
    // 设置所有表头
    tableWidget->setHorizontalHeaderLabels(headers);
}
QMap<QString, int> ADPNodeMap;
QMap<QString, int> ADPZNodeMap;
QMap<QString, int> ArmNodeMap;
void MainWindow::RefreshRegtableWidget()
{
    RegtableWidgetClear();
    for (auto it = ADPNodeMap.begin(); it != ADPNodeMap.end(); ++it)
    {
        RegColAppend(ui->ADPCommonRegtableWidget,it.key());
        RegColAppend(ui->ADPUserRegtableWidget,it.key());
        RegColAppend(ui->ADPFacRegtableWidget,it.key());
    }
    for (auto it = ADPZNodeMap.begin(); it != ADPZNodeMap.end(); ++it)
    {
        RegColAppend(ui->ZUserRegtableWidget,it.key());
        RegColAppend(ui->ZFacRegtableWidget,it.key());
        RegColAppend(ui->ZCommonRegtableWidge,it.key());
    }
    for (auto it = ArmNodeMap.begin(); it != ArmNodeMap.end(); ++it)
    {
        RegColAppend(ui->ArmCommonRegtableWidget,it.key());
        RegColAppend(ui->ArmFacRegtableWidget,it.key());
        RegColAppend(ui->ArmUserRegtableWidget,it.key());
    }
    ui->ADPCommonRegtableWidget->horizontalHeader()->setSectionResizeMode(QHeaderView::ResizeToContents);
    ui->ADPUserRegtableWidget->horizontalHeader()->setSectionResizeMode(QHeaderView::ResizeToContents);
    ui->ADPFacRegtableWidget->horizontalHeader()->setSectionResizeMode(QHeaderView::ResizeToContents);

    ui->ZUserRegtableWidget->horizontalHeader()->setSectionResizeMode(QHeaderView::ResizeToContents);
    ui->ZFacRegtableWidget->horizontalHeader()->setSectionResizeMode(QHeaderView::ResizeToContents);
    ui->ZCommonRegtableWidge->horizontalHeader()->setSectionResizeMode(QHeaderView::ResizeToContents);

    ui->ArmCommonRegtableWidget->horizontalHeader()->setSectionResizeMode(QHeaderView::ResizeToContents);
    ui->ArmFacRegtableWidget->horizontalHeader()->setSectionResizeMode(QHeaderView::ResizeToContents);
    ui->ArmUserRegtableWidget->horizontalHeader()->setSectionResizeMode(QHeaderView::ResizeToContents);
}

void MainWindow::RegtableWidgetClear()
{
    ui->ADPCommonRegtableWidget->setColumnCount(4);
    ui->ADPUserRegtableWidget->setColumnCount(4);
    ui->ADPFacRegtableWidget->setColumnCount(4);
    ui->ZUserRegtableWidget->setColumnCount(4);
    ui->ZFacRegtableWidget->setColumnCount(4);
    ui->ZCommonRegtableWidge->setColumnCount(4);
    ui->ArmCommonRegtableWidget->setColumnCount(4);
    ui->ArmFacRegtableWidget->setColumnCount(4);
    ui->ArmUserRegtableWidget->setColumnCount(4);
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
    if(pser->IsOpen())
    {
        checknodeaddr=0;
        m_stopOperations=false;
        QTimer::singleShot(50, this, &MainWindow::ScanfNodeLoop);
        ui->NodetableWidget->setRowCount(0);
        ADPNodeMap.clear();
        ADPZNodeMap.clear();
        ArmNodeMap.clear();
    }
    else
    {
        ShowWarningDialog("请打开串口再扫描节点");
    }

}
//检查数据完整性
bool MainWindow::DataReceiveFinishCheckCallback(const QByteArray &data)
{
    if (data.size() > 7 )
    {
        if(static_cast<quint8>(data[data.size()-3]) ==0xBC)
        {
            return true;
        }
    }
    return false;
}
bool MainWindow::DataIntegrityCheckCallback(const QByteArray &data)
{
    QByteArray framedata;
    int start_idex = -1;
    int end_idex = -1;
    if (data.size() < 8)
    {
        qDebug()<<"帧长度不够";
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
                qDebug()<<"帧尾长度不够";
                return false;
            }
            break;
        }
    }
    if (start_idex < 0 || end_idex < 0||start_idex >= end_idex) // 判断是否找到帧头祯尾
    {
        qDebug()<<"未找到帧头帧尾";
        return false;
    }
    framedata = data.mid(start_idex, end_idex - start_idex + 1);
    if ((static_cast<quint8>(data[end_idex + 1]) << 8 | static_cast<quint8>(data[end_idex + 2])) != calculateCRC16(framedata))
    {
        qDebug()<<"校验错误";
        return false;
    }
    return true;
}

void MainWindow::ScanfNodeLoop()
{
    if (++checknodeaddr > 48)
    {
        if(ui->NodetableWidget->rowCount()==0)
        {
            ShowWarningDialog("未扫描到节点");
        }
        RefreshRegtableWidget();
        checknodeaddr = 0;
        return;
    }
    QByteArray sendmsg = moem->MoemSenddataConfig(0, 0, 'k', QString("%1Rp0").arg(checknodeaddr).toUtf8());
    // LogMsgPrint(sendmsg.toHex().toUpper(),Log::TX);
    QByteArray respmsg = pser->ByteArrayTransmitWaitAck(sendmsg, 500);
    if (!respmsg.isEmpty())
    {
        MOEM::MOEMResult result = moem->ResponseDataConfig(respmsg);
        if(result.madpstatus == '0')
        {
            QString name=QString("%1 %2").arg(NodeNameConfig(result.responsestring.toInt()),QString::number(checknodeaddr));
            UserNodeAppend(checknodeaddr,name);
            NodeMapAppend(result.responsestring.toInt(),checknodeaddr,name);
        }
        // LogMsgPrint(respmsg.toHex().toUpper(),Log::RX);
    }
    if(!m_stopOperations)
    {
        QTimer::singleShot(50, this, &MainWindow::ScanfNodeLoop);
    }
    else
    {
        checknodeaddr=0;
    }
}

void MainWindow::NodeMapAppend(int type ,int addr,QString &name)
{
    switch (type)
    {
    case 0x00200001:
        ADPNodeMap.insert(name,addr);
        break;
    case 0x00300001:
        ADPZNodeMap.insert(name,addr);
        break;
    case 0x00300011:
        ADPZNodeMap.insert(name,addr);
        break;
    case 0x00200008:
        ADPNodeMap.insert(name,addr);
        break;
    case 0x00200010:
        ADPNodeMap.insert(name,addr);
        break;
    case 0x00200011:
        ADPNodeMap.insert(name,addr);
        break;
    case 0x00200003:
        ADPNodeMap.insert(name,addr);
        break;
    case 0x00300004:
        ArmNodeMap.insert(name,addr);
        break;
    case 0x00300012:
        ArmNodeMap.insert(name,addr);
        break;
    default:
        break;
    }
}



QString MainWindow::NodeNameConfig(int type)
{
    QString name;
    switch (type)
    {
    case 0x00200001:
        name="ADP16";
        break;
    case 0x00300001:
        name="ADPZ-64K";
        break;
    case 0x00300011:
        name="ADPZ-128K";
        break;
    case 0x00200008:
        name="ADP13";
        break;
    case 0x00200010:
        name="MAP8";
        break;
    case 0x00200011:
        name="MAP96";
        break;
    case 0x00200003:
        name="ADP18";
        break;
    case 0x00300004:
        name="移液臂-主控";
        break;
    case 0x00300012:
        name="移液臂-节点";
        break;
    default:
        break;
    }
    return name;
}

void MainWindow::on_ReadRegpushButton_clicked()
{
    QList<int> userregaddr;
    QList<int> facregaddr;
    QList<int> comregaddr;
    QByteArray respmsg;
    switch (ui->RegtabWidget->currentIndex())
    {
    case 0:
    {
        userregaddr=GetColumnRegAddr_R(ui->MADPRegtableWidget);
        QString tmp=QString("m0,%1").arg(userregaddr.size());
        respmsg =pser->ByteArrayTransmitWaitAck(moem->MoemSenddataConfig(0,0,'k',tmp.toUtf8()),500);
        if(!respmsg.isEmpty())
        {
            MOEM::MOEMResult result = moem->ResponseDataConfig(respmsg);
            if(result.madpstatus == '0')
            {
                QStringList items = result.responsestring.split(',');
                AppendColRegData(ui->MADPRegtableWidget,"值",items);
                ui->MADPRegtableWidget->resizeColumnsToContents();
            }
        }
        break;
    }
    case 1:

        break;
    case 2:

        break;
    case 3:

        break;
    case 4:
    {
        userregaddr=GetColumnRegAddr_R(ui->ArmUserRegtableWidget);
        facregaddr=GetColumnRegAddr_R(ui->ArmFacRegtableWidget);
        comregaddr=GetColumnRegAddr_R(ui->ArmCommonRegtableWidget);
        QString cmd;
        foreach (const QString node, ArmNodeMap.keys())
        {
            QString super=QString("%1Wr200,8888|").arg(ArmNodeMap.value(node));
            moem->ResponseDataConfig(pser->ByteArrayTransmitWaitAck(moem->MoemSenddataConfig(0,0,'k',super.toUtf8()),500));
            foreach (int regaddr, userregaddr)
            {
                QString tmp=QString("%1Rr%2|").arg(ArmNodeMap.value(node)).arg(regaddr);
                cmd=cmd.append(tmp);
            }
            respmsg =pser->ByteArrayTransmitWaitAck(moem->MoemSenddataConfig(0,0,'k',cmd.toUtf8()),500);
            if(!respmsg.isEmpty())
            {
                MOEM::MOEMResult result = moem->ResponseDataConfig(respmsg);
                if(result.madpstatus == '0')
                {
                    QStringList items = result.responsestring.split('|');
                    AppendColRegData(ui->ArmUserRegtableWidget,node,items);
                }
            }
            cmd.clear();
            foreach (int regaddr, facregaddr)
            {
                QString tmp=QString("%1Ru%2|").arg(ArmNodeMap.value(node)).arg(regaddr);
                cmd=cmd.append(tmp);
            }
            respmsg =pser->ByteArrayTransmitWaitAck(moem->MoemSenddataConfig(0,0,'k',cmd.toUtf8()),500);
            if(!respmsg.isEmpty())
            {
                MOEM::MOEMResult result = moem->ResponseDataConfig(respmsg);
                if(result.madpstatus == '0')
                {
                    QStringList items = result.responsestring.split('|');
                    AppendColRegData(ui->ArmFacRegtableWidget,node,items);
                }
            }
            cmd.clear();
            foreach (int regaddr, comregaddr)
            {
                QString tmp=QString("%1Rp%2|").arg(ArmNodeMap.value(node)).arg(regaddr);
                cmd=cmd.append(tmp);
            }
            respmsg =pser->ByteArrayTransmitWaitAck(moem->MoemSenddataConfig(0,0,'k',cmd.toUtf8()),500);
            if(!respmsg.isEmpty())
            {
                MOEM::MOEMResult result = moem->ResponseDataConfig(respmsg);
                if(result.madpstatus == '0')
                {
                    QStringList items = result.responsestring.split('|');
                    AppendColRegData(ui->ArmCommonRegtableWidget,node,items);
                }
            }
            cmd.clear();
        }
        break;
    }
    default:
        break;



    }
}

void MainWindow::AppendColRegData(QTableWidget* tableWidget,const QString &node,const QStringList &datalist)
{
    int colidex=FindColumnByHeader(tableWidget,node);
    int dataidex=0;
    for (int row = 0; row < tableWidget->rowCount(); ++row)
    {
        QTableWidgetItem* permission = tableWidget->item(row, 2);
        if (permission && permission->text().contains('R'))
        {
            if (permission->text().contains('R'))
            {       
                QTableWidgetItem* item = new QTableWidgetItem(datalist[dataidex]);
                tableWidget->setItem(row, colidex, item);
                dataidex++;
            }
        }
        else
        {
            // 对于没有读取权限的行，确保该单元格为空
            // 注意：setItem(row, colidex, nullptr) 会删除原有的item
            QTableWidgetItem* existingItem = tableWidget->item(row, colidex);
            if (existingItem)
            {
                tableWidget->takeItem(row, colidex); // 移除但不删除
                delete existingItem; // 显式删除
            }
        }
    }
}

int MainWindow::FindColumnByHeader(QTableWidget* tableWidget, const QString& headerText)
{
    for (int col = 0; col < tableWidget->columnCount(); ++col)
    {
        QTableWidgetItem* headerItem = tableWidget->horizontalHeaderItem(col);
        if (headerItem && headerItem->text() == headerText)
        {
            return col;  // 找到，返回列序号
        }
    }
    return -1;  // 未找到
}
// 获取整数类型数据
QList<int> MainWindow::GetColumnRegAddr_R(QTableWidget* tableWidget)
{
    QList<int> values;
    for (int row = 0; row < tableWidget->rowCount(); ++row)
    {
        QTableWidgetItem* item = tableWidget->item(row, 0);
        QTableWidgetItem* permission = tableWidget->item(row, 2);
        if (item && permission)
        {
            bool ok;
            int value = item->text().toInt(&ok);
            if (ok && permission->text().contains('R'))
            {
                values.append(value);
            }
        }
    }
    return values;
}
