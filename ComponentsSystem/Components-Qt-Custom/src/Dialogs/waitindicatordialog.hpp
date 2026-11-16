#pragma once

#include <QDialog>
#include <QTimer>

#include <Components/CustomQt/Widgets/Etc/WaitIndicatorWidget.h>

namespace QtCustom::Dialogs {

class WaitIndicatorDialog : public QDialog
{
    Q_OBJECT
public:
    explicit WaitIndicatorDialog(QWidget* parent = nullptr);

    static WaitIndicatorDialog& getInstance();

    void requestShow();
    void requestHide(int timeoutMs = 2000);
    void pollHide();

    void setTitle(const QString& text);
    void setDescription(const QString& text);

    QtCustom::Widgets::WaitIndicatorWidget* getIndicatorWidget() const;

    void configurePause(bool isEn, bool isVisible = true, const QString& text = {"Pause"});
    QPushButton* getPauseButton() const;

    void configureCancel(bool isEn, bool isVisible = true, const QString& text = {"Cancel"});
    QPushButton* getCancelButton() const;

signals:
    void sig_pauseClicked();
    void sig_cancelClicked();

private:
    QtCustom::Widgets::WaitIndicatorWidget* m_pWaitIndicator {nullptr};

    QTimer m_deadTimer;
    QTimer m_hideTimer;

    QPushButton* m_pPauseButton {nullptr};
    QPushButton* m_pCancelButton {nullptr};

    // QWidget interface
protected:
    void showEvent(QShowEvent *event) override;
    void hideEvent(QHideEvent *event) override;
};

}