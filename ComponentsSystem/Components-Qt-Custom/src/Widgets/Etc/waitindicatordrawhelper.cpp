#include "waitindicatordrawhelper.hpp"

#include <QPainter>
#include <QVariantAnimation>

#include <QEventLoop>

#include <Components/Logger/Logger.h>

#include "waitindicatorwidget.hpp"
#include "waitindicatorutility.hpp"

namespace QtCustom::Widgets {

WaitIndicatorDrawHelper *WaitIndicatorDrawHelper::create(const IndicatorConfigurationBasePtr &cfg, WaitIndicatorWidget *parent) {
    WaitIndicatorDrawHelper* pHelper {nullptr};
    if (dynamic_cast<IndicatorCircleConfiguration*>(cfg)) {
        pHelper = new CircleDrawHelper(parent);
    }
    else if (dynamic_cast<IndicatorCircleLinedConfiguration*>(cfg)) {
        pHelper = new CircleLinedDrawHelper(parent);
    }

    pHelper->setConfiguration(cfg);
    pHelper->init();
    cfg->setParent(pHelper);
    connect(cfg, &IndicatorConfigurationBase::visibleDataChanged,
            parent, &WaitIndicatorWidget::updateVisualState);
    return pHelper;
}

void WaitIndicatorDrawHelper::setConfiguration(const IndicatorConfigurationBasePtr &pConfig) {
    m_pConfig = pConfig;
}




void CircleDrawHelper::init()
{
    m_pRollingAnimation = new QVariantAnimation(this);
    connect(m_pRollingAnimation, &QVariantAnimation::valueChanged,
            this, &CircleDrawHelper::slot_processRollingAnimation);
    connect(m_pRollingAnimation, &QVariantAnimation::finished,
            m_pRollingAnimation, [this](){
                m_pRollingAnimation->start();
            });

    m_pRollingAnimation->setStartValue(double(0));
    m_pRollingAnimation->setEndValue(double(360));
    m_pRollingAnimation->setDuration(720);
}

void CircleDrawHelper::paint(QPainter *pPainter,
                             const QRect& targetWidgetRect,
                             double currentPercent,
                             uint8_t indicatorStatus)
{
    // Move to desired draw location
    pPainter->save();

    m_isCircleHasSpaces = (currentPercent > 0 && currentPercent < 100);

    auto convertedRect = getCircleArea(targetWidgetRect);

    pPainter->setClipRect(convertedRect);
    pPainter->translate(convertedRect.topLeft());

    paintIndicatorCircle(pPainter, currentPercent);
    paintPercent(pPainter, convertedRect, indicatorStatus, currentPercent);

    pPainter->restore();

    paintTitle(pPainter, convertedRect);
    paintDescription(pPainter, convertedRect);

    pPainter->end();
}

void CircleDrawHelper::processPercentSet([[maybe_unused]] double previousValue, [[maybe_unused]] double nextValue)
{
    qobject_cast<QWidget*>(parent())->update();
}

void CircleDrawHelper::startAnimation()
{
    m_pRollingAnimation->start();
}

void CircleDrawHelper::pauseAnimation()
{
    m_pRollingAnimation->pause();
}

void CircleDrawHelper::continueAnimation()
{
    m_pRollingAnimation->resume();
}

void CircleDrawHelper::stopAnimation()
{
    m_pRollingAnimation->stop();
}

void CircleDrawHelper::pollAnimation()
{
    // Wait for animation to complete
    QEventLoop loop;
    connect(m_pRollingAnimation, &QVariantAnimation::finished,
            &loop, &QEventLoop::quit);
    loop.exec();
}

void CircleDrawHelper::slot_processRollingAnimation(const QVariant &animationValue)
{
    m_animationOffsetPercent = animationValue.toDouble();
    if (m_isCircleHasSpaces) {
        qobject_cast<QWidget*>(parent())->update(); // TODO: Add checks?
    }
}

QRect CircleDrawHelper::createCircleRect() const
{
    return QRect(CIRCLE_RECT_OFFSET, CIRCLE_RECT_OFFSET, getConfig()->getSize().width(), getConfig()->getSize().height());
}

QRect CircleDrawHelper::getCircleArea(QRect targetWidgetRect) const
{
    auto rectCenter = targetWidgetRect.center();
    auto workAreaSize = getConfig()->getSize();
    targetWidgetRect.setWidth(workAreaSize.width() + CIRCLE_RECT_OFFSET * 2);
    targetWidgetRect.setHeight(workAreaSize.height() + CIRCLE_RECT_OFFSET * 2);
    targetWidgetRect.moveCenter(rectCenter);
    return targetWidgetRect;
}

void CircleDrawHelper::paintTitle(QPainter* pPainter, const QRect &targetWidgetRect) const
{
    pPainter->save();

    pPainter->setPen(getConfig()->getTextPen());
    auto fnt = pPainter->font();
    fnt.setBold(true);
    fnt.setPixelSize(14);
    pPainter->setFont(fnt);

    auto textRect = getCircleArea(targetWidgetRect);
    textRect.moveTo(textRect.x() - textRect.width() * 1.5, textRect.y() + textRect.height() * 0.8);
    textRect.setWidth(textRect.width() + textRect.width() * 3);
    pPainter->drawText(textRect, Qt::AlignCenter | Qt::TextWordWrap, getConfig()->getTitle());

    pPainter->restore();
}

void CircleDrawHelper::paintDescription(QPainter* pPainter, const QRect &targetWidgetRect) const
{
    pPainter->save();

    pPainter->setPen(getConfig()->getTextPen());
    auto fnt = pPainter->font();
    fnt.setItalic(true);
    fnt.setPixelSize(10);
    pPainter->setFont(fnt);

    auto textRect = getCircleArea(targetWidgetRect);
    textRect.moveTo(textRect.x() - textRect.width() * 1.5, textRect.y() + textRect.height() * 1.5);
    textRect.setWidth(textRect.width() + textRect.width() * 3);
    pPainter->drawText(textRect, Qt::AlignHCenter | Qt::TextWordWrap, getConfig()->getDescription());

    pPainter->restore();
}

void CircleDrawHelper::paintPercent(QPainter* pPainter, const QRect &targetWidgetRect, uint8_t indicatorStatus, double currentPercent) const
{
    if (indicatorStatus & WaitIndicatorWidget::Status::Paused) {
        return;
    }
    pPainter->save();

    auto pConfig = getConfig<IndicatorCircleConfiguration>();
    pPainter->setPen(pConfig->getPrimaryPen());
    pPainter->drawText(createCircleRect(), Qt::AlignCenter, QString("%1%").arg(currentPercent));

    pPainter->restore();
}

void CircleDrawHelper::paintIndicatorCircle(QPainter* pPainter, double currentPercent) const
{
    pPainter->setBrush(Qt::transparent);

    auto pConfig = getConfig<IndicatorCircleConfiguration>();

    pPainter->setPen(pConfig->getSecondaryPen());
    pPainter->drawEllipse(createCircleRect());

    pPainter->setPen(pConfig->getPrimaryPen());
    pPainter->drawArc(createCircleRect(),
                      utilityPieFromDegree(90 - m_animationOffsetPercent),
                      -utilityPieFromDegree(3.6f * currentPercent));
}




void CircleLinedDrawHelper::init()
{
    m_pRollingAnimation = new QVariantAnimation(this);
    connect(m_pRollingAnimation, &QVariantAnimation::valueChanged,
            this, &CircleLinedDrawHelper::slot_processRollingAnimation);
    connect(m_pRollingAnimation, &QVariantAnimation::finished,
            m_pRollingAnimation, [this](){
                m_pRollingAnimation->start();
                m_pRollingAnimation->setEndValue(getConfig<IndicatorCircleLinedConfiguration>()->getLineCount() - 1);
            });

    m_pRollingAnimation->setStartValue(0);
    m_pRollingAnimation->setEndValue(getConfig<IndicatorCircleLinedConfiguration>()->getLineCount() - 1);
    m_pRollingAnimation->setDuration(360);
}

void CircleLinedDrawHelper::paint(QPainter *pPainter, const QRect &targetWidgetRect, double currentPercent, uint8_t indicatorStatus)
{
    // Move to desired draw location
    pPainter->save();

    auto convertedRect = getCircleArea(targetWidgetRect);

    auto pConfig = getConfig<IndicatorCircleLinedConfiguration>();
    m_linePenGradient.setColorAt(0, pConfig->getSecondaryPen().color());
    m_linePenGradient.setColorAt(pConfig->getLineOffsetCoefficient(), pConfig->getPrimaryPen().color());

    pPainter->setClipRect(convertedRect);
    pPainter->translate(convertedRect.topLeft());

    paintIndicatorCircle(pPainter, currentPercent);
    paintPercent(pPainter, convertedRect, indicatorStatus, currentPercent);

    pPainter->restore();

    paintTitle(pPainter, convertedRect);
    paintDescription(pPainter, convertedRect);

    pPainter->end();
}

void CircleLinedDrawHelper::processPercentSet([[maybe_unused]] double previousValue, [[maybe_unused]]double nextValue)
{
    // Nothing to do in this context
}

void CircleLinedDrawHelper::startAnimation()
{
    m_pRollingAnimation->start();
}

void CircleLinedDrawHelper::pauseAnimation()
{
    m_pRollingAnimation->pause();
}

void CircleLinedDrawHelper::continueAnimation()
{
    m_pRollingAnimation->resume();
}

void CircleLinedDrawHelper::stopAnimation()
{
    m_pRollingAnimation->stop();
}

void CircleLinedDrawHelper::pollAnimation()
{
    // Wait for animation to complete
    QEventLoop loop;
    connect(m_pRollingAnimation, &QVariantAnimation::finished,
            &loop, &QEventLoop::quit);
    loop.exec();
}

void CircleLinedDrawHelper::slot_processRollingAnimation(const QVariant &animationValue)
{
    m_currentLineStep = animationValue.toInt();
    qobject_cast<QWidget*>(parent())->update(); // TODO: Add checks?
}

QRect CircleLinedDrawHelper::createCircleRect() const
{
    return QRect(CIRCLE_RECT_OFFSET, CIRCLE_RECT_OFFSET, getConfig()->getSize().width(), getConfig()->getSize().height());
}

QRect CircleLinedDrawHelper::getCircleArea(QRect targetWidgetRect) const
{
    auto rectCenter = targetWidgetRect.center();
    auto workAreaSize = getConfig()->getSize();
    targetWidgetRect.setWidth(workAreaSize.width() + CIRCLE_RECT_OFFSET * 2);
    targetWidgetRect.setHeight(workAreaSize.height() + CIRCLE_RECT_OFFSET * 2);
    targetWidgetRect.moveCenter(rectCenter);
    return targetWidgetRect;
}

void CircleLinedDrawHelper::paintTitle(QPainter *pPainter, const QRect &targetWidgetRect) const
{
    pPainter->save();

    pPainter->setPen(getConfig()->getTextPen());
    auto fnt = pPainter->font();
    fnt.setBold(true);
    fnt.setPixelSize(14);
    pPainter->setFont(fnt);

    auto textRect = getCircleArea(targetWidgetRect);
    textRect.moveTo(textRect.x() - textRect.width() * 1.5, textRect.y() + textRect.height() * 0.8);
    textRect.setWidth(textRect.width() + textRect.width() * 3);
    pPainter->drawText(textRect, Qt::AlignCenter | Qt::TextWordWrap, getConfig()->getTitle());

    pPainter->restore();
}

void CircleLinedDrawHelper::paintDescription(QPainter *pPainter, const QRect &targetWidgetRect) const
{
    pPainter->save();

    pPainter->setPen(getConfig()->getTextPen());
    auto fnt = pPainter->font();
    fnt.setItalic(true);
    fnt.setPixelSize(10);
    pPainter->setFont(fnt);

    auto textRect = getCircleArea(targetWidgetRect);
    textRect.moveTo(textRect.x() - textRect.width() * 1.5, textRect.y() + textRect.height() * 1.4);
    textRect.setWidth(textRect.width() + textRect.width() * 3);
    pPainter->drawText(textRect, Qt::AlignHCenter | Qt::TextWordWrap, getConfig()->getDescription());

    pPainter->restore();
}

void CircleLinedDrawHelper::paintPercent(QPainter *pPainter, const QRect &targetWidgetRect, uint8_t indicatorStatus, double currentPercent) const
{
    if (indicatorStatus & WaitIndicatorWidget::Status::Paused) {
        return;
    }
    pPainter->save();

    auto pConfig = getConfig<IndicatorCircleLinedConfiguration>();
    pPainter->setPen(pConfig->getPrimaryPen());
    pPainter->drawText(createCircleRect(), Qt::AlignCenter, QString("%1%").arg(currentPercent));

    pPainter->restore();
}

void CircleLinedDrawHelper::paintIndicatorCircle(QPainter *pPainter, double currentPercent)
{
    pPainter->setBrush(Qt::transparent);

    auto pConfig = getConfig<IndicatorCircleLinedConfiguration>();

    auto circleRect = createCircleRect();

    auto deltaAngle = 360.0 / double(pConfig->getLineCount());
    QLine deltaLine;
    auto deltaLineLength = pConfig->getSize().width() / 2.0;
    deltaLine.setP1(QPoint(deltaLineLength * pConfig->getLineOffsetCoefficient(), 0));
    deltaLine.setP2(QPoint(deltaLineLength, 0));

    // Get gradient point
    QTransform currentLineTransform;
    currentLineTransform.translate(circleRect.center().x(), circleRect.center().y());
    currentLineTransform.rotate(deltaAngle * m_currentLineStep);
    auto deltaLineCopy = deltaLine;
    deltaLineCopy.setP1({});
    auto mappedLine = currentLineTransform.map(deltaLineCopy);
    m_linePenGradient.setStart(mappedLine.p1());
    m_linePenGradient.setFinalStop(mappedLine.p2());

    auto linePen = pConfig->getSecondaryPen();
    linePen.setBrush(m_linePenGradient);
    pPainter->setPen(linePen);

    for (uint16_t i = 0; i < pConfig->getLineCount(); ++i) {
        currentLineTransform.reset();
        currentLineTransform.translate(circleRect.center().x(), circleRect.center().y());
        currentLineTransform.rotate(deltaAngle * i);
        auto tmpLine = currentLineTransform.map(deltaLine);
        pPainter->drawLine(tmpLine);
    }
}

}