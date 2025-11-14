#ifndef CHARTWIDGET_H
#define CHARTWIDGET_H

#include <QWidget>
#include <QtCharts>

class ChartWidget : public QWidget
{
    Q_OBJECT

public:
    explicit ChartWidget(QWidget *parent = nullptr);
    void AppendChartView(QChartView *chartwidget);
    void addDataPoint(double x, double y);
    void setChartTitle(const QString &title);
    void setAxisTitles(const QString &xTitle, const QString &yTitle);
    void clearData();
    void setYRange(double min, double max);
private:
    QChart *chart;
    QChartView *chartView;
    QLineSeries *series;
    QValueAxis *axisX;
    QValueAxis *axisY;
};


#endif // CHARTWIDGET_H
