#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "controlcan_wrapper.h"
#include "log.h"
#include "plotview.h"

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private:
    Ui::MainWindow *ui;
private slots:
    void onErrorOccurred(const QString &errorMessage);
    void on_OpenCANpushButton_clicked();    
    void on_SetPlotpushButton_clicked();

    void on_ClearGraphpushButton_clicked();

    void on_ClearLogpushButton_clicked();

    void on_CANSendpushButton_clicked();

    void on_SetRxFramepushButton_clicked();

    void on_DevTypecomboBox_activated(int index);

    void on_GraphStoppushButton_clicked();

public:
    void ControlCANParaConfig();
    bool CANOpen();
    bool CANClose();
    void ShowWarningDialog(const QString &arg1 );
    void Log_Print(const QString &text,Log::LogLevel level);
    bool TxCANDataConfig();
    void Plot_Init();
    void DataAppend();
public:
     ControlCANWrapper *canWrapper = new ControlCANWrapper(this);
     Log *log = new Log();
     QString CANdllPaths;
     QTimer *sertime=new QTimer(this);
     PlotView *plot=new PlotView();
     quint8 CANType;
     ControlCANWrapper::ControlCAN hcontrolcan;
};



#endif // MAINWINDOW_H
