#include "plotview.h"
#include <QDebug>
PlotView::PlotView()
{
    // data_x.resize(MAX_POINTS);
    // data_y.resize(MAX_POINTS);
    data_x.clear();
    data_y.clear();
    axis_x_width=500;
    axis_y_max=1000;
    axis_y_min=-10;
}
PlotView::~PlotView()
{

}

void PlotView::SetupPlot(QWidget *widget,QGridLayout *gridLayout)
{
    // 创建 QCustomPlot 实例
    customPlot = new QCustomPlot(widget);
    // 启用选择/拖动
    customPlot->setInteractions(QCP::iRangeDrag | QCP::iRangeZoom | QCP::iSelectPlottables|QCP::iSelectAxes|QCP::iSelectLegend);
    // 设置布局，让 QCustomPlot 填充整个 widget
    gridLayout->addWidget(customPlot);
    gridLayout->setContentsMargins(0, 0, 0, 0);
    customPlot->addGraph();
    customPlot->graph(0)->setData(data_x, data_y);
    // 设置坐标轴范围
    customPlot->xAxis->setRange(0,axis_x_width);
    customPlot->yAxis->setRange(axis_y_min, axis_y_max);
    // 重绘
    customPlot->replot();
}

void PlotView::ReSetAxisRange()
{
    // 设置坐标轴范围
    customPlot->xAxis->setRange(customPlot->xAxis->range().lower,customPlot->xAxis->range().lower+axis_x_width);
    customPlot->yAxis->setRange(axis_y_min, axis_y_max);\
        // 重绘
    customPlot->replot();
}

void PlotView::SetAxisName(QString name_x,QString name_y)
{
    // 设置坐标轴标签
    customPlot->xAxis->setLabel(name_x);
    customPlot->yAxis->setLabel(name_y);
    customPlot->replot();
}

void PlotView::GraphLegendEnable(bool enable)
{
    customPlot->legend->setVisible(enable);
    customPlot->replot();
}

void PlotView::AxisPositionConfig(QLabel *LabelX,QLabel *LabelY)
{
    LabelX->setAlignment(Qt::AlignLeft);
    LabelX->setText("X: -");
    LabelY->setAlignment(Qt::AlignLeft);
    LabelY->setText("Y: -");
    connect(customPlot, &QCustomPlot::mouseMove, this, [this, LabelX](QMouseEvent *event) {
        double x = customPlot->xAxis->pixelToCoord(event->pos().x());
        // 更新覆盖标签
        LabelX->setText(QString("X:%1").arg(x, 0, 'f', 2));
    });
    connect(customPlot, &QCustomPlot::mouseMove, this, [this, LabelY](QMouseEvent *event) {
        double y = customPlot->yAxis->pixelToCoord(event->pos().y());
        // 更新覆盖标签
        LabelY->setText(QString("Y:%1").arg(y, 0, 'f', 2));
    });
}

void PlotView::SetGraphColor(int idex,QColor col)
{
    customPlot->graph(idex)->setPen(QPen(col));
    // 重绘
    customPlot->replot();
}

void PlotView::AppendData(int idex ,double x ,double y)
{
    data_x.append(x);
    data_y.append(y);
    if (data_x.size() > MAX_POINTS)
    {
        data_x.removeFirst();
        data_y.removeFirst();
    }
    // 更新图表
    customPlot->graph(idex)->setData(data_x, data_y);
    // 自动调整X轴范围
    if (x > customPlot->xAxis->range().upper)
    {
        customPlot->xAxis->setRange(x-customPlot->xAxis->range().size(), x);
    }
    if (y > customPlot->yAxis->range().upper)
    {
        customPlot->yAxis->setRange(axis_y_min, y);
    }
    else if (y < customPlot->yAxis->range().lower)
    {
        customPlot->yAxis->setRange(y, axis_y_max);
    }
    // 重绘
    customPlot->replot();
}

void PlotView::SetGraphName(int idex,QString namestr)
{
    customPlot->graph(idex)->setName(namestr);
    // 重绘
    customPlot->replot();
}

void PlotView::GrapClear(int idex)
{
    data_x.clear();
    data_y.clear();
    // 更新图表
    customPlot->graph(idex)->setData(data_x, data_y);
    // 重绘
    customPlot->replot();
}



