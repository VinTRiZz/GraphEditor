#pragma once

#include <QWidget>
#include <QChartGlobal>

QT_CHARTS_BEGIN_NAMESPACE
class QLineSeries;
class QChart;
class QChartView;
QT_CHARTS_END_NAMESPACE

QT_CHARTS_USE_NAMESPACE

class Callout;

namespace Ui {
class ChartWidget;
}

class ChartWidget : public QWidget
{
    Q_OBJECT

public:
    explicit ChartWidget(QWidget *parent = nullptr);
    ~ChartWidget();

    void setupChart(const QString & axeXname, const QString & axeYname, const std::pair<float, float> initRangeX, const std::pair<float, float> initRangeY);

    void drawChart(std::vector<float> & valueVector);

    void resetChart();

public slots:
    void tooltip(QPointF point, bool state);

private:
    Ui::ChartWidget *ui;

    Callout *m_tooltip;

    QChart *m_chart {nullptr};
    QLineSeries *m_series {nullptr};

    std::pair<float, float> rangeX;
    std::pair<float, float> rangeY;
};
