#include "waitindicatorwidget.hpp"

#include <QPainter>
#include <QEvent>
#include <QResizeEvent>

#include <Components/Logger/Logger.h>

#include "waitindicatordrawhelper.hpp"

namespace QtCustom::Widgets {

WaitIndicatorWidget::WaitIndicatorWidget(QWidget *parent) :
    QWidget{parent}
{
    // Work with source transparency for better displaying
    setWindowFlags(Qt::FramelessWindowHint | Qt::WindowStaysOnTopHint);
    setAttribute(Qt::WA_TranslucentBackground);
    setAttribute(Qt::WA_TransparentForMouseEvents);

    m_pHelper = WaitIndicatorDrawHelper::create(new IndicatorCircleConfiguration(this), this);

    if (parent) {
        parent->installEventFilter(this);
        this->setGeometry(parent->rect()); // Сразу подгоняем размер
    }
}

WaitIndicatorWidget::~WaitIndicatorWidget()
{
    // TODO: If there's errors with exiting during working status, think how to stop properly
}

void WaitIndicatorWidget::setTarget(QWidget *pWidget)
{
    m_pTargetWidget = pWidget;
    if (m_pHelper) {
        m_pHelper->startAnimation();
        m_pHelper->pauseAnimation();
    }
    updateVisualState();
}

QWidget *WaitIndicatorWidget::getTarget() const
{
    return m_pTargetWidget;
}

void WaitIndicatorWidget::setConfiguration(IndicatorConfigurationBasePtr conf)
{
    if (m_pHelper) { m_pHelper->deleteLater(); }
    m_pHelper = WaitIndicatorDrawHelper::create(conf, this);
    if (m_pHelper) {
        m_pHelper->startAnimation();
        m_pHelper->pauseAnimation();
    }
    updateVisualState();
}

IndicatorConfigurationBasePtr WaitIndicatorWidget::getConfiguration() const
{
    if (!m_pHelper) { return {}; }
    return m_pHelper->getConfig();
}

void WaitIndicatorWidget::start()
{
    if (m_status != Status::Ready) {
        return;
    }
    m_status = Status::Working;

    if (m_pHelper && m_pHelper->getConfig()->getIsDisablingParent()) {
        m_pTargetWidget->setEnabled(false);
    }

    show();
    pollAnimation();
}

void WaitIndicatorWidget::pauseIndicator()
{
    if (m_status & (Status::Paused | Status::Ready)) {
        return;
    }
    m_status = Status::Pausing;
    updateVisualState();
    if (m_pHelper) {
        m_pHelper->pauseAnimation();
    }

    m_status = Status::Paused;
    updateVisualState();
}

void WaitIndicatorWidget::continueIndicator()
{
    if (m_status != Status::Paused) {
        return;
    }
    m_status = Status::Working;
    updateVisualState();
    if (m_pHelper) {
        m_pHelper->continueAnimation();
    }
}

void WaitIndicatorWidget::stop()
{
    if (m_status & Status::Ready) {
        return;
    }
    m_status = Status::Stopping;

    pollAnimation();
    if (m_pHelper && m_pHelper->getConfig()->getIsDisablingParent()) {
        m_pTargetWidget->setEnabled(true);
    }
    m_status = Status::Ready;
    updateVisualState();
}

void WaitIndicatorWidget::pollAnimation() const
{
    // No animation pending
    if (m_status ^ ~Status::InProcess) {
        return;
    }
    if (!m_pHelper) {
        return;
    }
    m_pHelper->pollAnimation();
}

WaitIndicatorWidget::Status WaitIndicatorWidget::getStatus() const
{
    return m_status;
}

void WaitIndicatorWidget::setPercent(double perc)
{
    auto prevValue = m_currentPercent;
    m_currentPercent = perc * 1000;
    if (m_pHelper) {
        m_pHelper->processPercentSet(prevValue, m_currentPercent);
    }
}

double WaitIndicatorWidget::getCurrentPercent() const
{
    double res {};
    res += m_currentPercent / 1000;
    res += 0.001f * (m_currentPercent % 1000);
    return res;
}

void WaitIndicatorWidget::updateVisualState()
{
    if (m_status == Status::Ready) {
        hide();
    } else {
        update();
    }
}

void WaitIndicatorWidget::paintEvent(QPaintEvent *event)
{
    QWidget::paintEvent(event); // Draw base
    if (!m_pTargetWidget || !m_pHelper) { return; }

    if (!m_pPainter) {
        m_pPainter = new QPainter(this);
    }
    if (!m_pPainter->isActive()) {
        m_pPainter->begin(this);
    }
    m_pHelper->paint(m_pPainter, m_pTargetWidget->rect(), getCurrentPercent(), m_status);
}

void WaitIndicatorWidget::showEvent(QShowEvent *event)
{
    QWidget::showEvent(event);
    if (m_pTargetWidget) {
        setGeometry(m_pTargetWidget->geometry());
    }
    if (m_pHelper) {
        m_pHelper->continueAnimation();
    }
}

void WaitIndicatorWidget::hideEvent(QHideEvent *event)
{
    if (m_pHelper) {
        m_pHelper->pauseAnimation();
    }
    QWidget::hideEvent(event);
}

void WaitIndicatorWidget::changeEvent(QEvent *event) {
    QWidget::changeEvent(event);
    if (event->type() == QEvent::ParentChange) {
        if (auto *p = parentWidget()) {
            p->installEventFilter(this);
            this->setGeometry(p->rect());
        }
    }
}

bool WaitIndicatorWidget::eventFilter(QObject *watched, QEvent *event) {
    if (watched == parent() && event->type() == QEvent::Resize) {
        auto *resizeEvent = static_cast<QResizeEvent*>(event);
        this->resize(resizeEvent->size());
    }
    return QWidget::eventFilter(watched, event);
}

}