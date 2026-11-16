#include "waitindicatordialog.hpp"

#include <QEventLoop>

#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QPushButton>

namespace QtCustom::Dialogs {

WaitIndicatorDialog::WaitIndicatorDialog(QWidget* parent) :
    QDialog(parent)
{
    connect(&m_deadTimer, &QTimer::timeout, this, [this]() {
        m_deadTimer.stop();
        m_hideTimer.stop();
        hide();
    });

    connect(&m_hideTimer, &QTimer::timeout, this, [this]() {
        m_deadTimer.stop();
        m_hideTimer.stop();
        hide();
    });
    resize(380, 220);

    m_pWaitIndicator = new QtCustom::Widgets::WaitIndicatorWidget(this);

    auto pSpacerWidget = new QWidget(this);
    m_pWaitIndicator->setTarget(pSpacerWidget);

    auto pLayout = new QVBoxLayout(this);
    pLayout->addWidget(pSpacerWidget);

    auto pButtonsLayout = new QHBoxLayout;
    pLayout->addLayout(pButtonsLayout);

    m_pPauseButton = new QPushButton(this);
    pButtonsLayout->addWidget(m_pPauseButton);
    configurePause(true);
    connect(m_pPauseButton, &QPushButton::clicked,
            this, &WaitIndicatorDialog::sig_pauseClicked);

    m_pCancelButton = new QPushButton(this);
    pButtonsLayout->addWidget(m_pCancelButton);
    configureCancel(true);
    connect(m_pCancelButton, &QPushButton::clicked,
            this, [this](){
        emit sig_pauseClicked();
        QDialog::reject();
    });

    auto pConfig = new QtCustom::Widgets::IndicatorCircleLinedConfiguration;
    pConfig->setIsDisablingParent(false);
    m_pWaitIndicator->setConfiguration(pConfig);
}

WaitIndicatorDialog &WaitIndicatorDialog::getInstance() {
    static WaitIndicatorDialog inst;
    return inst;
}

QtCustom::Widgets::WaitIndicatorWidget* WaitIndicatorDialog::getIndicatorWidget() const {
    return m_pWaitIndicator;
}

void WaitIndicatorDialog::configurePause(bool isEn, bool isVisible, const QString &text)
{
    m_pPauseButton->setEnabled(isEn);
    m_pPauseButton->setVisible(isVisible);
    m_pPauseButton->setText(text);
}

QPushButton *WaitIndicatorDialog::getPauseButton() const
{
    return m_pPauseButton;
}

void WaitIndicatorDialog::configureCancel(bool isEn, bool isVisible, const QString &text)
{
    m_pCancelButton->setEnabled(isEn);
    m_pCancelButton->setVisible(isVisible);
    m_pCancelButton->setText(text);
}

QPushButton *WaitIndicatorDialog::getCancelButton() const
{
    return m_pCancelButton;
}

void WaitIndicatorDialog::showEvent(QShowEvent *event)
{
    m_deadTimer.start(20000);
    QDialog::showEvent(event);
    m_pWaitIndicator->start();
}

void WaitIndicatorDialog::hideEvent(QHideEvent *event)
{
    m_pWaitIndicator->stop();
    QDialog::hideEvent(event);
}

void WaitIndicatorDialog::requestShow() {
    show();
}

void WaitIndicatorDialog::requestHide(int timeoutMs) {
    if (m_hideTimer.isActive()) {
        m_hideTimer.stop();
    }
    m_hideTimer.start(timeoutMs);
}

void WaitIndicatorDialog::pollHide() {
    if (isHidden()) { return; }

    QEventLoop poller;
    connect(&m_hideTimer, &QTimer::timeout,
            &poller, &QEventLoop::quit);
    poller.exec();
}

void WaitIndicatorDialog::setTitle(const QString &text)
{
    auto pConfig = m_pWaitIndicator->getConfiguration();
    pConfig->setTitle(text);
}

void WaitIndicatorDialog::setDescription(const QString &text)
{
    auto pConfig = m_pWaitIndicator->getConfiguration();
    pConfig->setDescription(text);
}


}