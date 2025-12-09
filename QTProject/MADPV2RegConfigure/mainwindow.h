#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "file_json.h"
#include "log.h"
#include <QTimer>
#include "serport_wrapper.h"
#include "protocol.h"

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
    void onResponseComplete(const QByteArray &data);
    void RefreshSerialPorts();
    void LogMsgPrint(const QString &text,Log::LogLevel type);
    void ErrorMsgPrint(int errorcode ,const QString &errorString);
    void ShowWarningDialog(const QString &arg1 );
    void UserRegListAppend(int addr ,const QString &func,const QString &rw,const QString &unit,int value);
    void UserNodeAppend(int nodeaddr,const QString &nodename);
    void ScanfNodeLoop();
    static bool DataIntegrityCheckCallback(QByteArray &data);
private slots:
    void on_serOpenpushButton_clicked();

    void on_SerialcomboBox_activated(int index);

    void on_IplineEdit_textEdited(const QString &arg1);

    void on_serBaudRatecomboBox_activated(int index);

    void on_ClearLogpushButton_clicked();


    void on_ReadNodepushButton_clicked();

private:
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
};
#endif // MAINWINDOW_H
