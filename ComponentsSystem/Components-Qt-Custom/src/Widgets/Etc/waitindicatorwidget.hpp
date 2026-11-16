#pragma once

#include <QWidget>

#include <memory>

#include "waitindicatorconfigurations.hpp"

namespace QtCustom::Widgets {

class WaitIndicatorDrawHelper;

/**
 * @brief The WaitIndicatorWidget class Used to display wait indicators
 */
class WaitIndicatorWidget : public QWidget
{
    Q_OBJECT
public:
    explicit WaitIndicatorWidget(QWidget *parent = nullptr);
    ~WaitIndicatorWidget();

    /**
     * @brief setTarget Set widget to work with. Must be child of this widget's pWidget widget
     * @param pWidget
     */
    void setTarget(QWidget* pWidget);
    QWidget* getTarget() const;

    /**
     * @brief setConfiguration Set main configuration of a indicator
     * @param conf
     * @note If you did not changed type of configuration, got by getConfiguration() method, use updateConfiguration() to save changes
     */
    void setConfiguration(IndicatorConfigurationBasePtr conf);
    IndicatorConfigurationBasePtr getConfiguration() const;

    // Basic interaction
    void start();
    void pauseIndicator();
    void continueIndicator();
    void stop();

    /**
     * @brief The Status enum Describes status of an indicator
     */
    enum Status : uint8_t {
        InProcess   = 0b00001, // Used to determine whether status is not final

        Ready       = 0b00010,              // Indicator is hidden and ready to work
        Working     = 0b00100,              // Displaying wait status (for example, rotating circle)
        Starting    = InProcess | Working,  // Playing indicator show animation
        Paused      = 0b01000,              // Indication is paused, causing cycle animation with no percent displaying
        Pausing     = InProcess | Paused,   // Playing pause animation
        Stopping    = InProcess | Ready,    // Playing indicator hide animation
    };
    Status getStatus() const;

    /**
     * @brief pollAnimation Used to wait while current status is not final
     */
    void pollAnimation() const;

    /**
     * @brief setPercent Set indicator percent
     * @param perc
     */
    void setPercent(double perc);
    double getCurrentPercent() const;

public slots:
    /**
     * @brief updateVisualState Do the same as QWidget::update, according to this widget logic
     */
    void updateVisualState();

private:
    QWidget* m_pTargetWidget {nullptr};

    // Dynamic processing
    Status      m_status            {Status::Ready};
    uint64_t    m_currentPercent    {0}; // For example, 99.123 is 99123. Used to workaround double store errors

    QPainter*   m_pPainter {nullptr};
    WaitIndicatorDrawHelper* m_pHelper {nullptr};

    std::unique_ptr<WaitIndicatorDrawHelper> m_currentDrawHelper;

    // QWidget interface
protected:
    void paintEvent(QPaintEvent *event) override;
    void showEvent(QShowEvent *event) override;
    void hideEvent(QHideEvent *event) override;
    void changeEvent(QEvent *event) override;
    bool eventFilter(QObject *watched, QEvent *event);
};

}