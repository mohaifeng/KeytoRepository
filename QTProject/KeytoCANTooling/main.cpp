#include "mainwindow.h"
QT_CHARTS_USE_NAMESPACE
#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

//    // 创建数据序列
//    QLineSeries *series = new QLineSeries();
//    series->append(0, 6);
//    series->append(2, 4);
//    series->append(3, 8);
//    series->append(7, 4);
//    series->append(10, 5);
//    series->setName("示例曲线");

//    // 创建图表
//    QChart *chart = new QChart();
//    chart->addSeries(series);
//    chart->setTitle("简单曲线图");
//    chart->createDefaultAxes();
//    chart->legend()->setVisible(true);

//    // 新方法：获取坐标轴并设置范围
//    auto xAxis = static_cast<QValueAxis*>(chart->axes(Qt::Horizontal).first());
//    auto yAxis = static_cast<QValueAxis*>(chart->axes(Qt::Vertical).first());

//    xAxis->setTitleText("X 轴");
//    yAxis->setTitleText("Y 轴");

//    // 创建图表视图
//    QChartView *chartView = new QChartView(chart);
//    chartView->setRenderHint(QPainter::Antialiasing);

    MainWindow w;
    w.show();
    return a.exec();
}
