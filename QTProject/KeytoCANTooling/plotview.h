#ifndef PLOTVIEW_H
#define PLOTVIEW_H

#include "qcustomplot.h"

#define MAX_POINTS  10000

class PlotView : public QObject
{
    Q_OBJECT
public:
    PlotView();
    ~PlotView();

public:
    void SetupPlot(QWidget *widget,QGridLayout *gridLayout);
    void AxisPositionConfig(QLabel *LabelX,QLabel *LabelY);
    void AppendData(int idex ,double x ,double y);
    void SetGraphColor(int idex,QColor col);
    void SetGraphName(int idex,QString namestr);
    void GraphLegendEnable(bool enable);
    void SetAxisName(QString name_x,QString name_y);
    void GrapClear(int idex);
    void ReSetAxisRange();
public:
    QCustomPlot *customPlot;
    QVector<double> data_x,data_y;
    int axis_x_width;//X轴宽度
    double axis_y_max;//Y轴最大值
    double axis_y_min;//Y轴最小值

};

#endif // PLOTVIEW_H
