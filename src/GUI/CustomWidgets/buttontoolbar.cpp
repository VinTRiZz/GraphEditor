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

void HeadWidget::setButtonEnabled(unsigned int buttonPos, bool isButtonEnabled)
{
    if (buttonPos >= m_buttons.size()) {
        return;
    }
    auto targetIt = m_buttons.begin();
    std::advance(targetIt, buttonPos);
    if (targetIt == m_buttons.end()) {
        return;
    }
    targetIt->pButton->setEnabled(isButtonEnabled);
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

    auto pItem = layout()->takeAt(0);
    while (pItem != nullptr) {
        delete pItem;
        pItem = layout()->takeAt(0);
    }

    for (auto& buttonConf : m_buttons) {
        if (nullptr == buttonConf.pButton) {
            buttonConf.pButton = new QPushButton(this);
            setupButton(buttonConf.pButton, buttonConf.config);
        }
        buttonConf.pButton->setEnabled(buttonConf.config.isEnabled);
        layout()->addWidget(buttonConf.pButton);
    }
    auto horPolicy = !m_isVertical ? QSizePolicy::Expanding : QSizePolicy::Preferred;
    auto vertPolicy = m_isVertical ? QSizePolicy::Expanding : QSizePolicy::Preferred;
    layout()->addItem(new QSpacerItem(m_buttonsSize.width(), m_buttonsSize.height(), horPolicy, vertPolicy));
}

void HeadWidget::setupButton(QPushButton *pButton, const ButtonConfig &buttonInfo)
{
    pButton->setText(buttonInfo.name);
    pButton->setStyleSheet(buttonInfo.styleSheet);
    pButton->setToolTip(buttonInfo.tooltip);
    pButton->setIcon(buttonInfo.icon);
    pButton->setFixedSize(m_buttonsSize);
    pButton->setEnabled(buttonInfo.isEnabled);

    if (!buttonInfo.action) {
        return;
    }

    connect(pButton, &QPushButton::clicked,
            pButton, [buttonInfo, pButton]() {
        buttonInfo.action(pButton);
    });
}

void HeadWidget::resizeEvent(QResizeEvent *e)
{
    if (size() != m_buttonsSize) {
        for (auto& buttonInfo : m_buttons) {
            buttonInfo.pButton->setFixedSize(m_buttonsSize);
            buttonInfo.pButton->setIconSize(m_buttonsSize.scaled(m_buttonsSize.width() * 0.6, m_buttonsSize.height() * 0.6, Qt::AspectRatioMode::KeepAspectRatio));
        }
    }
    QWidget::resizeEvent(e);
}

}
