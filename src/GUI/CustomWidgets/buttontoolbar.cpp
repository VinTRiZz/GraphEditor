#include "buttontoolbar.h"

#include <QVBoxLayout>
#include <QHBoxLayout>

#include <QDebug>

namespace ButtonToolbar
{

HeadWidget::HeadWidget(QWidget *parent) :
    QWidget(parent)
{

}

HeadWidget::~HeadWidget()
{

}

void HeadWidget::setVertical()
{
    m_isVertical = true;
    updateLayout();
}

void HeadWidget::setHorizontal()
{
    m_isVertical = false;
    updateLayout();
}

void HeadWidget::addButton(const ButtonConfig &conf)
{
    auto buttonPos = conf.buttonPos;
    if (buttonPos > m_buttons.size()) {
        return;
    }

    auto targetIt = m_buttons.begin();
    std::advance(targetIt, buttonPos);
    if (targetIt == m_buttons.end()) {
        m_buttons.push_back({conf, nullptr});
    } else {
        m_buttons.insert(targetIt, {conf, nullptr});
    }
    updateLayout();
}

void HeadWidget::updateButton(const ButtonConfig &conf)
{
    auto buttonPos = conf.buttonPos;
    if (buttonPos >= m_buttons.size()) {
        return;
    }
    auto targetIt = m_buttons.begin();
    std::advance(targetIt, buttonPos);
    if (targetIt == m_buttons.end()) {
        return;
    }
    targetIt->config = conf;
    updateLayout();
}

void HeadWidget::removeButton(unsigned int buttonPos)
{
    if (buttonPos >= m_buttons.size()) {
        return;
    }
    auto targetIt = m_buttons.begin();
    std::advance(targetIt, buttonPos);
    delete targetIt->pButton;
    m_buttons.erase(targetIt);
}

void HeadWidget::setButtonSize(const QSize &iSize)
{
    m_buttonsSize = iSize;
}

void HeadWidget::updateLayout()
{
    if (m_isVertical && (nullptr == dynamic_cast<QVBoxLayout*>(layout()))) {
        delete layout();
        setLayout(new QVBoxLayout);
    }

    if (!m_isVertical && (nullptr == dynamic_cast<QHBoxLayout*>(layout()))) {
        delete layout();
        setLayout(new QHBoxLayout);
    }

    for (auto& buttonConf : m_buttons) {
        if (nullptr == buttonConf.pButton) {
            buttonConf.pButton = new QPushButton(this);
        }
        setupButton(buttonConf.pButton, buttonConf.config);
        layout()->addWidget(buttonConf.pButton);
    }
}

void HeadWidget::setupButton(QPushButton *pButton, const ButtonConfig &buttonInfo)
{
    pButton->setText(buttonInfo.name);
    pButton->setStyleSheet(buttonInfo.styleSheet);
    pButton->setToolTip(buttonInfo.tooltip);
    pButton->setIcon(buttonInfo.icon);
    pButton->setFixedSize(m_buttonsSize);

    if (!buttonInfo.action) {
        return;
    }

    connect(pButton, &QPushButton::clicked,
            pButton, [buttonInfo, pButton]() {
        buttonInfo.action(pButton);
    });
}

}
