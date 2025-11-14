#include "chartwidget.h"

ChartWidget::ChartWidget(QWidget *parent)
    : QWidget(parent)
{
    series = new QLineSeries();
    chart = new QChart();
    chartView = new QChartView(chart);
    // 创建坐标轴
    axisX = new QValueAxis();
    axisY = new QValueAxis();
}

void ChartWidget::AppendChartView(QChartView *chartwidget)
{
    // 创建系列
    series->setName("数据曲线");

    // 创建图表
    chart->addSeries(series);
    chart->setTitle("数据曲线显示");
    chart->setAnimationOptions(QChart::SeriesAnimations);

    // 设置坐标轴标题
    axisX->setTitleText("X轴");
    axisY->setTitleText("Y轴");
    // 设置坐标轴范围
    axisX->setRange(0, 1000);
    axisY->setRange(-1000, 1000);
    axisX->setTickCount(11); // -10000, -8000, ..., 10000
    axisY->setTickCount(11); // -10000, -8000, ..., 10000
    // 设置标签格式
    axisX->setLabelFormat("%.1f");
    axisY->setLabelFormat("%.2f");
    // 添加坐标轴到图表
    chart->addAxis(axisX, Qt::AlignBottom);
    chart->addAxis(axisY, Qt::AlignLeft);
    // 将序列附加到坐标轴
    series->attachAxis(axisX);
    series->attachAxis(axisY);
    // 创建图表视图
    chartView->setRenderHint(QPainter::Antialiasing);
    // 启用图表视图的缩放功能
    chartwidget->setRubberBand(QChartView::RectangleRubberBand);
    chartwidget->setRenderHint(QPainter::Antialiasing);
    // 设置图表到 chartWidget（现在已经是 QChartView）
    chartwidget->setChart(chart);
}

void ChartWidget::addDataPoint(double x, double y)
{
    series->append(x, y);

    // 自动调整坐标轴范围
    chart->createDefaultAxes();
}

void ChartWidget::setChartTitle(const QString &title)
{
    chart->setTitle(title);
}

void ChartWidget::setAxisTitles(const QString &xTitle, const QString &yTitle)
{
    if (chart->axes(Qt::Horizontal).size() > 0) {
        QValueAxis *axisX = qobject_cast<QValueAxis*>(chart->axes(Qt::Horizontal).first());
        if (axisX) axisX->setTitleText(xTitle);
    }
    if (chart->axes(Qt::Vertical).size() > 0) {
        QValueAxis *axisY = qobject_cast<QValueAxis*>(chart->axes(Qt::Vertical).first());
        if (axisY) axisY->setTitleText(yTitle);
    }
}

void ChartWidget::clearData()
{
    series->clear();
}

void ChartWidget::setYRange(double min, double max)
{
    if (chart->axes(Qt::Vertical).size() > 0) {
        QValueAxis *axisY = qobject_cast<QValueAxis*>(chart->axes(Qt::Vertical).first());
        if (axisY) axisY->setRange(min, max);
    }
}
