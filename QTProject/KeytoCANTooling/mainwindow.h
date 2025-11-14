#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "controlcan_wrapper.h"
#include "log.h"
#include <QWidget>
#include <QtCharts>
#include "chartwidget.h"
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
    void SerAppend();
public:
     ControlCANWrapper *canWrapper = new ControlCANWrapper(this);
     Log *log = new Log();
     QString CANdllPaths;
     QTimer *sertime=new QTimer(this);
     PlotView *plot;
};





#endif // MAINWINDOW_H
