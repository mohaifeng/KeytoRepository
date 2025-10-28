#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QTimer>
#include <QObject>
#include <QSerialPort>
#include <QSerialPortInfo>
#include <QList>
#include <QPushButton>
#include "protocol.h"



QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

#define GAP_TIME 50

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void on_pushButton_clicked();
    bool OpenSerialPort(const QString &portName,const int &baudrate);
    void CloseSerialPort();
    void RefreshSerialPorts();  // 刷新串口列表
    QByteArray hexStringToByteArray(const QString &hexString);
    void ShowWarningDialog(const QString &arg1 );
    void on_SerialcomboBox_activated(const QString &arg1);
    bool SendData(const QByteArray &data);
    void SendString(const QString &text);
    void SendHex(const QString &text);
    void on_BaudratecomboBox_activated(const QString &arg1);
    void ScanfAddrLoop();
    void on_CheckAddr_clicked();
    QByteArray WaitResponse(quint16 timeoutM);
    void on_AddrlineEdit_textEdited(const QString &arg1);
    void onReadyRead();
    void LogPrint(const QString &text);
    bool isResponseComplete(const QByteArray &data);
    void on_ClearLogpushButton_clicked();
    void PushButtonConfigSetEnabled(bool isok);
    void RegisterPushButton();
    void on_CmdSendpushButton_clicked();

    void on_StartReadpushButton_clicked();

private:
    Ui::MainWindow *ui;
    QString sernum;
    int baudrate;
    quint16 targetaddr=0;
    QTimer *SerialRefreshTimer = new QTimer(this);       // 串口任务定时器
    // 创建串口对象
    QSerialPort *SerialPort = new QSerialPort(this);
    QByteArray m_receivedData;
    bool m_responseReceived = false;
    Modbus *modbus=new Modbus();
    bool firstscanf=true;
    QList<QPushButton*> ControledButtons;

signals:
    void responseReady();  // 信号声明
};

#endif // MAINWINDOW_H
