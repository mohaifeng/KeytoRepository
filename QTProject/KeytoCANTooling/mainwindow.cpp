#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QDebug>
#include <QTimer>
#include <QRandomGenerator>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    CANdllPaths.clear();
    CANdllPaths = QCoreApplication::applicationDirPath()+ "/ControlCAN.dll";    //应用程序所在目录
    connect(canWrapper, &ControlCANWrapper::errorOccurred,this, &MainWindow::onErrorOccurred);
    // 加载DLL
    if (!canWrapper->LoadCANLibrary(CANdllPaths))
    {
        qDebug() << "Failed to load ControlCAN.dll";
        return;
    }
    Plot_Init();
    on_DevTypecomboBox_activated(ui->DevTypecomboBox->currentIndex());
    sertime->setInterval(50);
    connect(sertime,&QTimer::timeout,this,&MainWindow::DataAppend);
    sertime->start();
}

MainWindow::~MainWindow()
{
    delete ui;
}

// 警告框
void MainWindow::ShowWarningDialog(const QString &arg1 )
{
    QMessageBox::warning(nullptr, "警告", arg1);
}

void MainWindow::Plot_Init()
{
    plot->SetupPlot(ui->chartwidget,ui->gridLayout);
    plot->AxisPositionConfig(ui->AxisXlabel,ui->AxisYlabel);
    ui->MaxYlineEdit->setText(QString::number(plot->axis_y_max));
    ui->MinYlineEdit->setText(QString::number(plot->axis_y_min));
    ui->WidthXlineEdit->setText(QString::number(plot->axis_x_width));
}

double xidex=0;
void MainWindow::DataAppend()
{
    // 生成 1 到 100 之间的随机整数
    double randomInRange = double(QRandomGenerator::global()->bounded(1, 100));
    plot->AppendData(0,xidex++,randomInRange);
    xidex++;
}

void MainWindow::Log_Print(const QString &text,Log::LogLevel level)
{
    log->LogPrint(ui->LogplainTextEdit,text,level);
}

void MainWindow::onErrorOccurred(const QString &errorMessage)
{
    QString error_str="CAN Error:"+errorMessage;
    Log_Print(error_str,Log::LOG_ERROR);
}

void MainWindow::ControlCANParaConfig()
{
    //CAN参数初始化
    hcontrolcan.DevType=VCI_USBCAN_2E_U;
    hcontrolcan.DevIdex=ui->DevIdexcomboBox->currentIndex();
    hcontrolcan.CANIdex=ui->CANIdexcomboBox->currentIndex();
    hcontrolcan.CANBaudRate=ui->CANBaudratecomboBox->currentText().toInt();
}

bool MainWindow::CANOpen()
{
    QList<int> tim;
    switch(CANType)
    {
    case 0:
        ControlCANParaConfig();
        if(!canWrapper->OpenDevice(hcontrolcan.DevType,hcontrolcan.DevIdex))
        {
            return false;
        }
        hcontrolcan.InitStu.Mode=MODE_NORMAL;
        hcontrolcan.InitStu.Filter=RECEIVE_ALL;
        hcontrolcan.InitStu.AccCode=0;
        hcontrolcan.InitStu.AccMask=0xFFFFFFFF;
        tim = canWrapper->CAN_Baudrate_Config(hcontrolcan.CANBaudRate);
        hcontrolcan.InitStu.Timing0=tim[0];
        hcontrolcan.InitStu.Timing1=tim[1];
        if(!canWrapper->InitCAN(hcontrolcan.DevType, hcontrolcan.DevIdex, hcontrolcan.CANIdex, &hcontrolcan.InitStu))
        {
            return false;
        }
        if(!canWrapper->StartCAN(hcontrolcan.DevType, hcontrolcan.DevIdex, hcontrolcan.CANIdex))
        {
            return false;
        }

        break;
    case 1:break;
    }
    return true;
}

bool MainWindow::CANClose()
{
    switch(CANType)
    {
    case 0:
        return canWrapper->CloseDevice(hcontrolcan.DevType, hcontrolcan.DevIdex);
    case 1:
        break;
    }
    return false;
}

void MainWindow::on_OpenCANpushButton_clicked()
{
    // 打开设备
    if(ui->OpenCANpushButton->text()=="打开")
    {
        if(CANOpen())
        {
            ui->OpenCANpushButton->setText("关闭");
            Log_Print("打开成功",Log::LOG_INFO);
        }
        else
        {
            Log_Print("打开失败",Log::LOG_ERROR);
        }
    }
    else
    {
        CANClose();
        ui->OpenCANpushButton->setText("打开");
    }
}


void MainWindow::on_SetPlotpushButton_clicked()
{
    bool ok;
    plot->axis_y_max=ui->MaxYlineEdit->text().toDouble(&ok);
    plot->axis_y_min=ui->MinYlineEdit->text().toDouble(&ok);
    plot->axis_x_width=ui->WidthXlineEdit->text().toDouble(&ok);
    plot->ReSetAxisRange();
}

void MainWindow::on_ClearGraphpushButton_clicked()
{
    plot->GrapClear(0);
    xidex=0;
}

void MainWindow::on_ClearLogpushButton_clicked()
{
    ui->LogplainTextEdit->clear();
}

bool MainWindow::TxCANDataConfig()
{
    QString tmp=ui->CANDatalineEdit->text().remove(" ");
    if(tmp.size() !=(hcontrolcan.Tx_CANData.DataLen*2))
    {
        ShowWarningDialog("数据长度错误，请重新输入！");
        return false;
    }
    for(int i = 0; i < hcontrolcan.Tx_CANData.DataLen; i++)
    {
        bool ok;
        quint8 byte = static_cast<char>(QStringView(tmp).mid(i*2, 2).toInt(&ok, 16));
        if (!ok) {
            return false;
        }
        hcontrolcan.Tx_CANData.Data[i]=byte;
    }
    return true;
}

void MainWindow::on_CANSendpushButton_clicked()
{
    bool ok;
    hcontrolcan.Tx_CANData.ExternFlag=ui->TxFrameTypecomboBox->currentIndex();
    hcontrolcan.Tx_CANData.ID=ui->TxFrameIDlineEdit->text().toUInt(&ok,16);
    hcontrolcan.Tx_CANData.DataLen=ui->DLCcomboBox->currentText().toInt(&ok,10);
    hcontrolcan.Tx_CANData.RemoteFlag=0;
    if(!TxCANDataConfig())
    {
        return;
    }
    switch (CANType)
    {
    case 0:
        canWrapper->Transmit(hcontrolcan.DevType, hcontrolcan.DevIdex, hcontrolcan.CANIdex,&hcontrolcan.Tx_CANData, 1);
        break;
    case 1:
        break;

    }

}

void MainWindow::on_SetRxFramepushButton_clicked()
{

}

void MainWindow::on_DevTypecomboBox_activated(int index)
{
    CANType=index;
}


void MainWindow::on_GraphStoppushButton_clicked()
{

}
