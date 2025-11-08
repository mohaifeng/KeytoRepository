#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QDebug>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    // 加载DLL
    if (!canWrapper->loadLibrary())
    {
        qDebug() << "Failed to load ControlCAN.dll";
        return;
    }
}

MainWindow::~MainWindow()
{
    delete ui;
}


void MainWindow::on_OpenPortpushButton_clicked()
{
    // 打开设备
//    canWrapper->openDevice();
}
