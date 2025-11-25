#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QTimer>
#include <QObject>
#include "log.h"
#include <QSerialPort>
#include <QSerialPortInfo>
#include <QList>
#include <QPushButton>
#include "protocol.h"
//#include "file_json.h"
#include "valve.h"
#include <QTableWidget>

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

#define GAP_TIME 50
#define RX_TIMEOUT 100

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
    void SendHexString(const QString &text);
    void ScanfAddrLoop();
    void on_CheckAddr_clicked();
    QByteArray WaitResponse(quint16 timeoutM);
    void on_AddrlineEdit_textEdited(const QString &arg1);
    void onReadyRead();
    void LogPrint(const QString &text,Log::LogLevel level);
    bool isResponseComplete(const QByteArray &data);
    void on_ClearLogpushButton_clicked();
    void PushButtonConfigSetEnabled(bool isok);
    void RegisterPushButton();
    void on_CmdSendpushButton_clicked();
    void on_StartReadpushButton_clicked();
    void GetRegList();
    void RegListConfig();
    void AddMultipleRows(QTableWidget* tableWidget, quint32 col,const QList<quint16>& data,int mode);
    void on_AddRowpushButton_clicked();
    void SendRegListAck(QList<QList<quint16>> &lst);
    void InitRow(QTableWidget* tableWidget,int row);
    void on_GapTimelineEdit_textEdited(const QString &arg1);
    void on_ClearLinespushButton_clicked();
    void ReadParaShow(quint16 reg,quint16 data);
    void on_ReadPara_pushButton_clicked();
    QByteArray Modbus_SendWaitAck(quint16 timeoutMs);
    QByteArray Modbus_ReadRegWaitAck(quint8 cmd,quint16 reg , quint16 num,quint16 timeoutMs);
    QByteArray Modbus_WriteSingleRegWaitAck(quint8 cmd,quint16 reg , quint16 data,quint16 timeoutMs);
    QByteArray Modbus_WriteMultiRegWaitAck(quint8 cmd,quint16 reg , QByteArray &data,quint16 timeoutMs);
    void on_FastSwitchpushButton_clicked();
    quint16 ScanfReadPara(quint16 reg);
    void on_TargetChlineEdit_textEdited(const QString &arg1);
    void AppendMultiDataToDataBuff(QByteArray &databuff,QString &datatext,int type);
    void on_CounterclockwisepushButton_clicked();
    void on_ClockwisepushButton_clicked();
    quint16 QstringToUint16(QString &data,int type);
    QByteArray QstringToQbytearray(QString &data,int type);
    void on_InitpushButton_clicked();
    void Versiong_Config(QString ver);
    void on_StoppushButton_clicked();

    void on_ClearErrorpushButton_clicked();

    void on_BaudratecomboBox_activated(int index);

    void on_SetParapushButton_clicked();

    void on_SavepushButton_clicked();

    void on_ResetpushButton_clicked();

    void on_ChangeAddr_clicked();

    // 通用的重试执行函数
    void ExecuteWhenIdle(std::function<void()> function);
private:
    Ui::MainWindow *ui;
    QString sernum;//串口号
    QTimer *SerialRefreshTimer = new QTimer(this);       // 串口任务定时器
    // 创建串口对象
    QSerialPort *SerialPort = new QSerialPort(this);
    QByteArray m_receivedData;//接收到的数据串
    bool m_responseReceived = false;//接收到数据标志
    Modbus *modbus = new Modbus();
    Log *log = new Log();
//    JsonReader *json_read = new JsonReader();
    Valve *valve = new Valve();
    bool firstscanf=true;
    QList<QPushButton*> ControledButtons;
    quint16 startaddr;
    quint16 readnum;
    QList<quint16> reg_lst;
    QList<quint16> para_reg_lst;
    QList<QList<quint16>> send_reg_lst;
    QList<quint16> resp_reg_data;
    quint32 delaytime;
    bool is_seridle;
    QString version;
signals:
    void responseReady();  // 信号声明
};



#endif // MAINWINDOW_H
