#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "file_json.h"
#include "log.h"
#include <QTimer>
#include "serport_wrapper.h"
#include "protocol.h"
#include <QTableWidget>

QT_BEGIN_NAMESPACE
namespace Ui {
class MainWindow;
}
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();
    void ConfigInit();
    void MADPRegInit();
    void ArmRegInit();
    void onResponseComplete(const QByteArray &data);
    void RefreshSerialPorts();
    void DebugMsgPrint(const QString &text);
    void LogMsgPrint(const QString &text,Log::LogLevel type);
    void MADPTxMsgPrint();
    void MADPRxMsgPrint(const QByteArray &frame);
    void ErrorMsgPrint(int errorcode ,const QString &errorString);
    void ShowWarningDialog(const QString &arg1 );
    void RefreshRegtableWidget();
    void RegColAppend(QTableWidget *tableWidget,const QString &headerName);
    void RegRowAppend(QTableWidget *tableWidget,int addr ,const QString &func,const QString &rw,const QString &unit);
    void UserNodeAppend(int nodeaddr,const QString &nodename);
    void NodeMapAppend(int type ,int addr,QString &name);
    void ScanfNodeLoop();
    QString NodeNameConfig(int type);
    static bool DataReceiveFinishCheckCallback(const QByteArray &data);
    static bool DataIntegrityCheckCallback(const QByteArray &data);
    void RegtableWidgetClear();
    QList<int> GetColumnRegAddr_R(QTableWidget* tableWidget);
    void AppendColRegData(QTableWidget* tableWidget,const QString &node,const QStringList &items);
    int FindColumnByHeader(QTableWidget* tableWidget, const QString& headerText);
private slots:
    void on_serOpenpushButton_clicked();

    void on_SerialcomboBox_activated(int index);

    void on_IplineEdit_textEdited(const QString &arg1);

    void on_serBaudRatecomboBox_activated(int index);

    void on_ClearLogpushButton_clicked();


    void on_ReadNodepushButton_clicked();

    void on_ReadRegpushButton_clicked();

private:
    bool m_stopOperations;
    QTimer *SerialRefreshTimer = new QTimer(this);      // 串口号刷新定时器
    QTimer *completetimer = new QTimer(this); //数据接收完整定时器
    //串口配置相关
    SerialPortWrapper *pser = new SerialPortWrapper();//串口实例对象
    SerialPortWrapper::SerialConfig serialconfig;//串口配置对象
    QString currentsername;//当前串口号
    quint8 currentsernum = 0;//当前串口数目
    Log *log = new Log();
    Ui::MainWindow *ui;
    QJsonObject jsonobj;
    JsonReader * json= new JsonReader();
    MOEM *moem=new MOEM();
    quint8 checknodeaddr = 0;
};
#endif // MAINWINDOW_H
