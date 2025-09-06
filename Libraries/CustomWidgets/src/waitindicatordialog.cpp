#include "waitindicatordialog.h"
#include "ui_waitindicatordialog.h"

WaitIndicatorDialog::WaitIndicatorDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::WaitIndicatorDialog)
{
    ui->setupUi(this);
    ui->progressBar->setValue(0);

    connect(&m_deadTimer, &QTimer::timeout,
            this, [this](){
        m_deadTimer.stop();
        m_hideTimer.stop();
        hide();
    });

    connect(&m_hideTimer, &QTimer::timeout,
            this, [this](){
        m_deadTimer.stop();
        m_hideTimer.stop();
        hide();
    });

    setFixedSize(380, 190);
}

WaitIndicatorDialog::~WaitIndicatorDialog()
{
    delete ui;
}

WaitIndicatorDialog &WaitIndicatorDialog::getInstance()
{
    static WaitIndicatorDialog inst;
    return inst;
}

void WaitIndicatorDialog::requestShow()
{
    show();
    while (isHidden()) {
        qApp->processEvents();
    }
    m_deadTimer.start(20000);
}

void WaitIndicatorDialog::requestHide(int timeoutMs)
{
    if (m_hideTimer.isActive()) {
        m_hideTimer.stop();
    }
    m_hideTimer.start(timeoutMs);
}

void WaitIndicatorDialog::pollHide()
{
    while (m_hideTimer.isActive()) {
        qApp->processEvents(QEventLoop::ExcludeUserInputEvents);
    }
}

int WaitIndicatorDialog::getCurrentPercent() const
{
    return ui->progressBar->value();
}

void WaitIndicatorDialog::setTitle(const QString &txt)
{
    auto displayText = QString("<html><head/><body><p><span style=\" font-size:16pt;\">%0</span></p></body></html>").arg(txt);
    ui->title_label->setText(displayText);
    qApp->processEvents();
}

QString WaitIndicatorDialog::getTitle() const
{
    return ui->title_label->text();
}

void WaitIndicatorDialog::setDescription(const QString &txt)
{
    auto displayText = QString("<html><head/><body><p><span style=\" font-size:12pt;\">%0</span></p></body></html>").arg(txt);
    ui->description_label->setText(displayText);
    qApp->processEvents();
}

QString WaitIndicatorDialog::getDescription() const
{
    return ui->description_label->text();
}

void WaitIndicatorDialog::setDescriptionHidden(bool isHiddn)
{
    ui->description_label->setHidden(isHiddn);
    qApp->processEvents();
}

void WaitIndicatorDialog::setButtonsEnabled(bool isButtnsEnabled)
{
    ui->buttons_widget->setHidden(!isButtnsEnabled);
    qApp->processEvents();
}

bool WaitIndicatorDialog::getIsButtonsEnabled() const
{
    return !ui->buttons_widget->isHidden();
}

void WaitIndicatorDialog::setProgressBarEnabled(bool isBarEnabled)
{
    ui->progressBar->setHidden(!isBarEnabled);
    qApp->processEvents();
}

bool WaitIndicatorDialog::getIsProgressBarEnabled() const
{
    return !ui->progressBar->isHidden();
}

void WaitIndicatorDialog::setPercent(int perc)
{
    ui->progressBar->setValue(perc);
    qApp->processEvents();
}
