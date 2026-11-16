#pragma once

#include <QWidget>
#include <QPushButton>

class QVariantAnimation;

namespace QtCustom::Widgets {

/**
 * @brief The SidebarWidget class Widget to show / hide other widgets as a side panel
 * @note Set parent of this widget to use. Do not add into layout to avoid artefacts
 */
class SidebarWidget : public QWidget
{
    Q_OBJECT
    using QWidget::setParent; // Can break internal logic if allowed to use
public:
    explicit SidebarWidget(QWidget *parent = nullptr);

    // Overrides parent method because require extra logic
    void setParentWidget(QWidget* pWidget);

    /**
     * @brief setWidget Set widget inside bar
     * @param pWidget
     */
    void setWidget(QWidget* pWidget);
    inline QWidget* getWidget() const { return m_pWidget; }

    /**
     * @brief setWidgetWidth Set width of a widget to be set after showing
     * @param wwidth
     * @note works only when direction is right / left
     */
    void setWidgetWidth(int wwidth);
    inline int getWidgetWidth() const { return m_widgetTargetWidth; }

    /**
     * @brief setWidgetHeight Set height of a widget to be set after showing
     * @param wheight
     * @note works only when direction is top / bottom
     */
    void setWidgetHeight(int wheight);
    inline int getWidgetHeight() const { return m_widgetTargetHeight; }

    /**
     * @brief hideTargetWidget Used to hide widget manually without animation
     */
    void hideTargetWidget();

    /**
     * @brief showTargetWidget Used to show widget manually without animation
     */
    void showTargetWidget();

    /**
     * @brief The Direction enum Used to work with directions in widget
     */
    enum Direction : uint8_t
    {
        Top         = 0b0001,
        Left        = 0b0010,
        Right       = 0b0100,
        Bottom      = 0b1000,
    };

    /**
     * @brief setShowTowardsDirection Set direction for a widget to be shown
     * @param sdir
     * @note Changes logic of using widget's target height and width
     */
    void setShowTowardsDirection(Direction sdir);
    inline Direction getShowTowardsDirection() const { return m_showTowardsDirection; }

    /**
     * @brief setButtonPosition Set position offset of a button referred to a parent widget
     * @param offsetP
     */
    void setButtonPosition(const QPoint &offsetP);
    inline QPoint getButtonPosition() const { return m_buttonOffset; }

    enum HideState : uint8_t
    {
        HS_Invalid          = 0, // For invalid widget set cases
        HS_Hidden           = 0b001,
        HS_Shown            = 0b010,
        HS_InProcess        = 0b100,
        HS_InProcess_hide   = HS_InProcess | HS_Hidden,
        HS_InProcess_show   = HS_InProcess | HS_Shown
    };
    inline HideState getWidgetState() const { return m_widgetHideState; }

    /**
     * @brief The AnimationSpeed enum For simple configuring
     * @note Must not be used with widget that can change size
     */
    enum class AnimationSpeed : uint8_t
    {
        NoAnimation = 0,
        UltraFast,
        Fast,
        Medium,
        Slow,
        UltraSlow,
    };

    inline void setWidgetShowSpeed(AnimationSpeed spd) { m_widgetShowSpeed = spd; }
    inline AnimationSpeed getWidgetShowSpeed() const { return m_widgetShowSpeed; }

    inline void setWidgetHideSpeed(AnimationSpeed spd) { m_widgetHideSpeed = spd; }
    inline AnimationSpeed getWidgetHideSpeed() const { return m_widgetHideSpeed; }

    // Used to customize button
    using toggleCallback_t = std::function<void(QPushButton*, HideState)>; // Button to toggle and state to toggle in
    void setButton(QPushButton* pButton);
    void setToggleCallback(toggleCallback_t&& cbk);
    inline QPushButton* getButton() const { return m_pButton; }

signals:
    void sig_aboutToToggle();
    void sig_toggled();

public slots:
    /**
     * @brief slot_toggle Toggles widget to be hidden / shown. Called by this widget button
     * @note If widget not set, does nothing (even not call button toggle callback)
     */
    void slot_toggle();

    /**
     * @brief slot_updateButtonPosition Updates button position
     * @note    Must be called from resizeEvent() of parentwidget
     *          Important slot for widgets, allowed to change their size
     */
    void slot_updateButtonPosition();

private:
    // Main functionality
    QPushButton*m_pButton   {nullptr};
    QWidget*    m_pWidget   {nullptr};

    // Because of resize events
    QSize m_cachedParentSize;

    // Dynamic state processing
    int                 m_widgetTargetWidth     {300};
    int                 m_widgetTargetHeight    {300};
    QVariantAnimation*  m_pCurrentAnimation     {nullptr};
    HideState           m_widgetHideState {HS_Invalid};
    AnimationSpeed      m_widgetHideSpeed {AnimationSpeed::Medium};
    AnimationSpeed      m_widgetShowSpeed {AnimationSpeed::Medium};
    toggleCallback_t    m_buttonToggleCallback;

    // Viewing configuration
    Direction   m_showTowardsDirection {Direction::Right};
    QPoint      m_buttonOffset  {};

    // Utility
    double calculateAnimationDuration(AnimationSpeed spd) const;
    QPoint calculateCurrentButtonPosition() const;

    // Widget animation methods
    void pollWidgetAnimation();
    void stopWidgetAnimations();
    void updateVisualState();
    void setAnimationStep(int step);
    void startShowAnimation(AnimationSpeed asp);
    void startHideAnimation(AnimationSpeed asp);

protected:
    void changeEvent(QEvent *event) override;
    bool eventFilter(QObject *watched, QEvent *event) override;

};

}