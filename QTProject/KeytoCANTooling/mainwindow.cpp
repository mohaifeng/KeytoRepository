#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QDebug>
#include <QTimer>
#include <QValueAxis>
#include <QRandomGenerator>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QWidget>

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
    chartwidget->AppendChartView(ui->chartwidget);
    sertime->setInterval(1000);
    connect(sertime,&QTimer::timeout,this,&MainWindow::SerAppend);
    sertime->start();
}

MainWindow::~MainWindow()
{
    delete ui;
}
int xidex=0;
void MainWindow::SerAppend()
{
    chartwidget->addDataPoint(xidex,(qrand() % 100 + 1));
    xidex++;
}
//void Log_Print(const QString &text,Log::LogLevel level)
//{
//    log->LogPrint(ui->LogplainTextEdit,text,level);
//}

void MainWindow::onErrorOccurred(const QString &errorMessage)
{
    qDebug() << "CAN Error:" << errorMessage;
}

void MainWindow::on_OpenCANpushButton_clicked()
{
    // 打开设备
    if(ui->OpenCANpushButton->text()=="打开")
    {
        if(canWrapper->OpenDevice(21,0))
        {
            ui->OpenCANpushButton->setText("关闭");
        }
        else
        {
            qDebug()<<"打开失败";
        }

    }
    else
    {
        canWrapper->CloseDevice(21,0);
        ui->OpenCANpushButton->setText("打开");
    }
}

