#include "chartwidget.hpp"
#include "ui_chartwidget.h"

#include <QtCharts>
#include "callout.hpp"

#include <QVBoxLayout>

ChartWidget::ChartWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::ChartWidget)
{
    ui->setupUi(this);

    m_chart = new QChart;
    m_series = new QLineSeries;

    QChartView *chartView = new QChartView(this);

    chartView->setChart(m_chart);
    m_chart->addSeries(m_series);

    QValueAxis *axisX = new QValueAxis(m_chart);
    axisX->setRange(0, 100);
    axisX->setLabelFormat("%g");
    axisX->setTitleText("X axis");

    m_chart->addAxis(axisX, Qt::AlignBottom);
    m_series->attachAxis(axisX);

    QValueAxis *axisY = new QValueAxis(m_chart);
    axisY->setRange(0, 100);
    axisY->setTitleText("Y axis");

    m_chart->addAxis(axisY, Qt::AlignLeft);
    m_series->attachAxis(axisY);

    m_chart->legend()->hide();

    connect(m_series, &QLineSeries::hovered, this, &ChartWidget::tooltip);

    QVBoxLayout *mainLayout = new QVBoxLayout(this);
    mainLayout->addWidget(chartView);
}

ChartWidget::~ChartWidget()
{
    if (m_chart)
    {
        delete m_series;
        m_series = nullptr;

        delete m_chart;
        m_chart = nullptr;
    }

    delete ui;
}

void ChartWidget::setupChart(const QString &axeXname, const QString &axeYname, const std::pair<float, float> initRangeX, const std::pair<float, float> initRangeY)
{
    if (
        (rangeX.first > rangeX.second) ||
        (rangeY.first > rangeY.second)
        )
        return;

    rangeX = initRangeX;
    rangeY = initRangeY;

    m_series->clear();
    for (int i = rangeX.first; i < rangeX.second; i++)
        m_series->append(i, rangeY.first);

    QValueAxis * pXaxis = dynamic_cast<QValueAxis *>(m_chart->axes(Qt::Horizontal)[0]);
    if (pXaxis)
    {
        pXaxis->setRange(rangeX.first, rangeX.second);
        pXaxis->setLabelFormat("%g");
        pXaxis->setTitleText(axeXname);
    }

    QValueAxis * pYaxis = dynamic_cast<QValueAxis *>(m_chart->axes(Qt::Vertical)[0]);
    if (pYaxis)
    {
        pYaxis->setRange(rangeY.first, rangeY.second);
        pYaxis->setTitleText(axeYname);
    }
}

void ChartWidget::drawChart(std::vector<float> &valueVector)
{
    for (int i = 0; i < m_series->count(); i++)
    {
        if ((valueVector.size() > i) && (valueVector[i] <= rangeY.second) && (valueVector[i] >= rangeY.first))
            m_series->replace(i, QPoint(i, valueVector[i]));
        else
            m_series->replace(i, QPoint(i, rangeY.first));
    }
}

void ChartWidget::resetChart()
{
    for (int i = 0; i < m_series->count(); i++)
    {
        m_series->replace(i, QPoint(i, rangeY.first));
    }
}

void ChartWidget::tooltip(QPointF point, bool state)
{
    if (!m_tooltip)
        m_tooltip = new Callout(m_chart);

    if (state) {
        int xPos = point.x();
        if ((m_series->count() <= xPos) )// || (m_packets.size() <= xPos))
        {
            qDebug() << "Found bad pos:" << xPos;
            return;
        }

        m_tooltip->setText(QString("Дата: %1\nВремя: %2\nЗначение: %3").arg(
//                m_packets[xPos].convertTime.c_str(),
//                m_packets[xPos].convertDate.c_str(),
                QString::number(m_series->at(xPos).y())
            )
        );

        m_tooltip->setAnchor(point);
        m_tooltip->setZValue(11);
        m_tooltip->updateGeometry();
        m_tooltip->show();
    } else {
        m_tooltip->hide();
    }
}
