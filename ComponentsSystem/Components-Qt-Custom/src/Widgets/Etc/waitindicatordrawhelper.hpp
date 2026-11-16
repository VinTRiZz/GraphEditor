#pragma once

#include <QObject>

#include "waitindicatorconfigurations.hpp"

class QPainter;
class QVariantAnimation;

namespace QtCustom::Widgets {

class WaitIndicatorWidget;

/**
 * @brief The WaitIndicatorDrawHelper class Base for widget draw helpers
 */
class WaitIndicatorDrawHelper : public QObject
{
public:
    using QObject::QObject;

    static WaitIndicatorDrawHelper* create(const IndicatorConfigurationBasePtr& cfg, WaitIndicatorWidget* parent);

    void setConfiguration(const IndicatorConfigurationBasePtr& pConfig);

    template <typename ConfigT = IndicatorConfigurationBase>
    ConfigT* getConfig() const {
        return static_cast<ConfigT*>(m_pConfig);
    }

    virtual void init() = 0;
    virtual void paint(QPainter* pPainter,
                       const QRect& targetWidgetRect,
                       double currentPercent,
                       uint8_t indicatorStatus) = 0;

    virtual void processPercentSet(double previousValue, double nextValue) = 0;

    virtual void startAnimation() = 0;
    virtual void pauseAnimation() = 0;
    virtual void continueAnimation() = 0;
    virtual void stopAnimation() = 0;
    virtual void pollAnimation() = 0;

private:
    IndicatorConfigurationBasePtr m_pConfig;
};

template <>
inline IndicatorConfigurationBasePtr WaitIndicatorDrawHelper::getConfig() const {
    return m_pConfig;
}



/**
 * @brief The CircleDrawHelper class Standard loading circle
 */
class CircleDrawHelper : public WaitIndicatorDrawHelper
{
    Q_OBJECT
public:
    using WaitIndicatorDrawHelper::WaitIndicatorDrawHelper;

    // WaitIndicatorDrawHelper interface
    void init() override;
    virtual void paint(QPainter* pPainter,
                       const QRect &targetWidgetRect,
                       double currentPercent,
                       uint8_t indicatorStatus) override;

    virtual void processPercentSet(double previousValue, double nextValue) override;

    void startAnimation() override;
    void pauseAnimation() override;
    void continueAnimation() override;
    void stopAnimation() override;
    void pollAnimation() override;

private slots:
    void slot_processRollingAnimation(const QVariant& animationValue);

private:
    static constexpr auto CIRCLE_RECT_OFFSET {5};
    QRect createCircleRect() const;

    bool   m_isCircleHasSpaces {false};
    double m_animationOffsetPercent {0};

    QVariantAnimation* m_pRollingAnimation {nullptr};

    QRect getCircleArea(QRect targetWidgetRect) const;

    void paintTitle(QPainter *pPainter, const QRect& targetWidgetRect) const;
    void paintDescription(QPainter* pPainter, const QRect& targetWidgetRect) const;
    void paintPercent(QPainter* pPainter, const QRect& targetWidgetRect, uint8_t indicatorStatus, double currentPercent) const;

    void paintIndicatorCircle(QPainter* pPainter, double currentPercent) const;
};


/**
 * @brief The CircleLinedDrawHelper class Loading circle, made of lines from center
 */
class CircleLinedDrawHelper : public WaitIndicatorDrawHelper
{
    Q_OBJECT
public:
    using WaitIndicatorDrawHelper::WaitIndicatorDrawHelper;

    // WaitIndicatorDrawHelper interface
    void init() override;
    virtual void paint(QPainter* pPainter,
                       const QRect &targetWidgetRect,
                       double currentPercent,
                       uint8_t indicatorStatus) override;

    virtual void processPercentSet(double previousValue, double nextValue) override;

    void startAnimation() override;
    void pauseAnimation() override;
    void continueAnimation() override;
    void stopAnimation() override;
    void pollAnimation() override;

private slots:
    void slot_processRollingAnimation(const QVariant& animationValue);

private:
    static constexpr auto CIRCLE_RECT_OFFSET {5};
    QRect createCircleRect() const;

    uint32_t            m_currentLineStep {0};
    QLinearGradient     m_linePenGradient;
    QVariantAnimation*  m_pRollingAnimation {nullptr};

    QRect getCircleArea(QRect targetWidgetRect) const;

    void paintTitle(QPainter *pPainter, const QRect& targetWidgetRect) const;
    void paintDescription(QPainter* pPainter, const QRect& targetWidgetRect) const;
    void paintPercent(QPainter* pPainter, const QRect& targetWidgetRect, uint8_t indicatorStatus, double currentPercent) const;

    void paintIndicatorCircle(QPainter* pPainter, double currentPercent);
};


}