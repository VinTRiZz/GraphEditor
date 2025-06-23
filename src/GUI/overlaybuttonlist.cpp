#include "overlaybuttonlist.h"

#include <QPropertyAnimation>
#include <QProperty>

#include "logging.h"

OverlayButtonList::OverlayButtonList(QWidget* parent)
    : QPushButton{parent}
{
    setupWidget();
    setupSignals();
}

void OverlayButtonList::setOpenDirection(ButtonOpenDirection direction_)
{
    m_openDirections = direction_;
}

void OverlayButtonList::setMaxButtonCount(ButtonOpenDirection direction_, uint maxButtonCount)
{
    if (direction_ & ButtonOpenDirection::Left) {
        m_maxButtonCounts[ButtonOpenDirection::Left] = maxButtonCount;
    }

    if (direction_ & ButtonOpenDirection::Up) {
        m_maxButtonCounts[ButtonOpenDirection::Up] = maxButtonCount;
    }

    if (direction_ & ButtonOpenDirection::Right) {
        m_maxButtonCounts[ButtonOpenDirection::Right] = maxButtonCount;
    }

    if (direction_ & ButtonOpenDirection::Down) {
        m_maxButtonCounts[ButtonOpenDirection::Down] = maxButtonCount;
    }
}

void OverlayButtonList::setMovable(bool isMovable)
{
    m_isMovable = isMovable;
}

void OverlayButtonList::setWidgetPadding(ButtonOpenDirection direction_, int offsetValue)
{
    if (direction_ & ButtonOpenDirection::Left) {
        m_padding[ButtonOpenDirection::Left] = offsetValue;
    }

    if (direction_ & ButtonOpenDirection::Up) {
        m_padding[ButtonOpenDirection::Up] = offsetValue;
    }

    if (direction_ & ButtonOpenDirection::Right) {
        m_padding[ButtonOpenDirection::Right] = offsetValue;
    }

    if (direction_ & ButtonOpenDirection::Down) {
        m_padding[ButtonOpenDirection::Down] = offsetValue;
    }

    update();
}

void OverlayButtonList::setButtonSize(const QSize &bSize_)
{
    m_fixedSize = bSize_;

    if (!m_isButtonsHidden) {
        fixButtonsPositions();
    }
}

void OverlayButtonList::setAnimationSpeed(double increaser_)
{
    if (increaser_ < 0) {
        throw std::invalid_argument("OverlayButtonList: increaser less than 0");
    }
    m_animationMultiplier = increaser_;
}

void OverlayButtonList::setHideOnClick(double hideOnClick_)
{
    m_hideOnClick = hideOnClick_;
}

void OverlayButtonList::setOpenedIcon(const QIcon &ic)
{
    m_openIcon = ic;
    if (!m_isButtonsHidden) {
        setIcon(m_openIcon);
    }
}

void OverlayButtonList::setClosedIcon(const QIcon &ic)
{
    m_closeIcon = ic;
    if (m_isButtonsHidden) {
        setIcon(m_closeIcon);
    }
}

uint OverlayButtonList::addButton(const ButtonInfo &button_)
{
    m_buttonsInfo.push_back(button_);
    m_buttons.push_back(new QPushButton(parentWidget()));
    setupButton(m_buttons.back(), button_);

    if (!m_isButtonsHidden) {
        fixButtonsPositions();
    }

    return m_buttons.size() - 1;
}

void OverlayButtonList::updateButton(const ButtonInfo &button_, uint index)
{
    if (index >= m_buttonsInfo.size()) {
        return;
    }

    m_buttonsInfo[index] = button_;
    setupButton(m_buttons[index], m_buttonsInfo[index]);
}

uint OverlayButtonList::buttonCount() const
{
    return m_buttonsInfo.size();
}

uint OverlayButtonList::getDirectionButtonCount(ButtonOpenDirection direction_)
{
    uint cnt {0};
    uint leastCount = m_buttons.size();

    if (direction_ & ButtonOpenDirection::Left & m_openDirections) {
        cnt = std::min(m_maxButtonCounts[ButtonOpenDirection::Left], static_cast<uint>(leastCount));
        leastCount -= cnt;
    }

    if (direction_ & ButtonOpenDirection::Right & m_openDirections) {
        cnt = std::min(m_maxButtonCounts[ButtonOpenDirection::Right], static_cast<uint>(leastCount));
        leastCount -= cnt;
    }

    if (direction_ & ButtonOpenDirection::Up & m_openDirections) {
        cnt = std::min(m_maxButtonCounts[ButtonOpenDirection::Up], static_cast<uint>(leastCount));
        leastCount -= cnt;
    }

    if (direction_ & ButtonOpenDirection::Down & m_openDirections) {
        cnt = std::min(m_maxButtonCounts[ButtonOpenDirection::Down], static_cast<uint>(leastCount));
    }

    return cnt;
}

std::optional<OverlayButtonList::ButtonInfo> OverlayButtonList::buttonInfo(uint index) const
{
    if (index >= m_buttonsInfo.size()) {
        return {};
    }
    return m_buttonsInfo[index];
}

void OverlayButtonList::removeButton(uint buttonIndex)
{
    if (buttonIndex >= m_buttonsInfo.size()) {
        return;
    }

    m_buttonsInfo.erase(m_buttonsInfo.begin() + buttonIndex);
    m_buttons[buttonIndex]->deleteLater();
    m_buttons.erase(m_buttons.begin() + buttonIndex);
}

QPushButton *OverlayButtonList::getButton(uint buttonIndex)
{
    if (buttonIndex >= m_buttonsInfo.size()) {
        return nullptr;
    }
    return m_buttons[buttonIndex];
}

void OverlayButtonList::showButtons()
{
    for (auto pButton : m_buttons) {
        pButton->show();
        pButton->setFixedSize(m_fixedSize);
        pButton->setIconSize(m_fixedSize.scaled(m_fixedSize.width() * 0.6, m_fixedSize.height() * 0.6, Qt::AspectRatioMode::KeepAspectRatio));
    }

    moveButtons(!m_isButtonsHidden, true);
    m_isButtonsHidden = false;
    setIcon(m_openIcon);
}

void OverlayButtonList::hideButtons()
{
    moveButtons(!m_isButtonsHidden, true);
    m_isButtonsHidden = true;
    setIcon(m_closeIcon);
}

void OverlayButtonList::moveButtons(bool isInverted, bool isAnimated)
{
    uint directionButtonCount {0};

    auto moveDirected = [&](ButtonOpenDirection moveDir) {
        uint currentOffset {1};
        while ((directionButtonCount < m_buttons.size()) && (m_maxButtonCounts[moveDir] >= currentOffset)) {
            auto pButton = m_buttons[directionButtonCount];

            QRect positionRect;
            positionRect.setX(this->x());
            positionRect.setY(this->y());
            positionRect.setWidth(width());
            positionRect.setHeight(height());
            QRect prevPositionRect = positionRect;

            double animationTime = 0;

            // "дома"
            auto separatorLength = m_buttonMargins + positionRect.width();
            bool isHorizontalMove = static_cast<bool>(moveDir & ButtonOpenDirection::Left) || static_cast<bool>(moveDir & ButtonOpenDirection::Right);
            auto deltaX = separatorLength * isHorizontalMove * currentOffset;
            auto targetX = positionRect.x() + deltaX * -1 * static_cast<bool>(moveDir & ButtonOpenDirection::Left) + deltaX * static_cast<bool>(moveDir & ButtonOpenDirection::Right);

            separatorLength = m_buttonMargins + positionRect.height();
            bool isVerticalMove = static_cast<bool>(moveDir & ButtonOpenDirection::Up) || static_cast<bool>(moveDir & ButtonOpenDirection::Down);
            auto deltaY = separatorLength * isVerticalMove * currentOffset;
            auto targetY = positionRect.y() + deltaY * -1 * static_cast<bool>(moveDir & ButtonOpenDirection::Up) + deltaY * static_cast<bool>(moveDir & ButtonOpenDirection::Down);

            positionRect.moveTo(targetX, targetY);

            animationTime = std::max(fabs(deltaX) / separatorLength, fabs(deltaY) / separatorLength) * 300 / m_animationMultiplier;

            if (isInverted) {
                std::swap(positionRect, prevPositionRect);
            }

            if (isAnimated) {
                auto anim = new QPropertyAnimation(pButton, "geometry");

                anim->setStartValue(prevPositionRect);
                anim->setEndValue(positionRect);

                anim->setDuration(animationTime);

                connect(this, &OverlayButtonList::stopAnimations,
                        anim, &QPropertyAnimation::stop);

                connect(anim, &QPropertyAnimation::finished,
                        this, [pButton, isInverted]() {
                    pButton->setHidden(isInverted);
                });

                anim->start(QPropertyAnimation::DeleteWhenStopped);
            } else {
                pButton->setGeometry(positionRect);
                pButton->setHidden(isInverted);
            }

            currentOffset++;
            directionButtonCount ++;
        }
    };
    if (m_openDirections & ButtonOpenDirection::Left) {
        moveDirected(ButtonOpenDirection::Left);
    }
    if (m_openDirections & ButtonOpenDirection::Right) {
        moveDirected(ButtonOpenDirection::Right);
    }
    if (m_openDirections & ButtonOpenDirection::Up) {
        moveDirected(ButtonOpenDirection::Up);
    }
    if (m_openDirections & ButtonOpenDirection::Down) {
        moveDirected(ButtonOpenDirection::Down);
    }
}

void OverlayButtonList::fixButtonsPositions()
{
    emit stopAnimations();
    moveButtons(m_isButtonsHidden, false);
}

void OverlayButtonList::setupButton(QPushButton *pButton, const ButtonInfo &buttonInfo)
{
    pButton->setText(buttonInfo.name);
    pButton->setStyleSheet(buttonInfo.styleSheet);
    pButton->setToolTip(buttonInfo.tooltip);
    pButton->setIcon(buttonInfo.icon);
    pButton->setFixedSize(size());
    pButton->setHidden(m_isButtonsHidden);

    if (buttonInfo.action) {
        connect(pButton, &QPushButton::clicked,
                pButton, [this, buttonInfo, pButton]() {
            buttonInfo.action(pButton);
            fixButtonsPositions();
            if (m_hideOnClick) {
                hideButtons();
            }
        });
    }
}

void OverlayButtonList::setupSignals()
{
    connect(this, &QPushButton::clicked,
            this, [this](){
        if (m_buttons.empty()) {
            return;
        }

        if (m_isButtonsHidden) {
            showButtons();
            return;
        }

        hideButtons();
    });
}

void OverlayButtonList::setupWidget()
{
    setIcon(m_closeIcon);
    setButtonSize(QSize(50, 50));
}

void OverlayButtonList::fixPosition()
{
    auto positionX = this->x();
    auto positionY = this->y();

    auto pParent = parentWidget();
    auto parentMaxX = pParent->width() - width();
    auto parentMaxY = pParent->height() - height();

    if (m_buttons.size()) {
        if (!m_isButtonsHidden) {
            auto rightButtonCount = getDirectionButtonCount(ButtonOpenDirection::Right);
            parentMaxX -= rightButtonCount * width() + rightButtonCount * m_buttonMargins;

            auto downButtonCount = getDirectionButtonCount(ButtonOpenDirection::Down);
            parentMaxY -= downButtonCount * height() + downButtonCount * m_buttonMargins;
        }
    }

    const int currentRightPadding     = (parentMaxX - positionX);
    const int currentLeftPadding      = (positionX);
    const int currentUpPadding        = positionY;
    const int currentDownPadding      = (parentMaxY - positionY);

    bool isRightPosition =
            (currentRightPadding    == m_padding[ButtonOpenDirection::Right]) &&
            (currentLeftPadding     == m_padding[ButtonOpenDirection::Left]) &&
            (currentUpPadding       == m_padding[ButtonOpenDirection::Up]) &&
            (currentDownPadding     == m_padding[ButtonOpenDirection::Down]) &&
            (positionX < parentMaxX) &&
            (positionY < parentMaxY)
    ;
    if (isRightPosition) {
        return;
    }

    positionX = m_padding[ButtonOpenDirection::Right] > -1 && (m_padding[ButtonOpenDirection::Right] != parentMaxX - positionX) ? parentMaxX - m_padding[ButtonOpenDirection::Right] : positionX;
    positionX = m_padding[ButtonOpenDirection::Left] > -1 && (m_padding[ButtonOpenDirection::Left] > positionX) ? m_padding[ButtonOpenDirection::Left] : positionX;
    positionX = positionX < 0 ? 0 : positionX;

    positionY = m_padding[ButtonOpenDirection::Down] > -1 && (m_padding[ButtonOpenDirection::Down] != parentMaxY - positionY) ? parentMaxY - m_padding[ButtonOpenDirection::Down] : positionY;
    positionY = m_padding[ButtonOpenDirection::Up] > -1 && (m_padding[ButtonOpenDirection::Up] > positionY) ? m_padding[ButtonOpenDirection::Up] : positionY;
    positionY = positionY < 0 ? 0 : positionY;

    if (!m_isButtonsHidden) {
        auto deltaX = positionX - x();
        auto deltaY = positionY - y();

        for (auto pButton : m_buttons) {
            pButton->move(pButton->x() + deltaX, pButton->y() + deltaY);
        }
    }

    move(positionX, positionY);
}

void OverlayButtonList::paintEvent(QPaintEvent *e)
{
    if (parentWidget() == nullptr) {
        QPushButton::paintEvent(e);
        return;
    }

    fixPosition();
    QPushButton::paintEvent(e);
}

void OverlayButtonList::resizeEvent(QResizeEvent *e)
{
    if (size() != m_fixedSize) {
        setFixedSize(m_fixedSize);
        setIconSize(m_fixedSize.scaled(m_fixedSize.width() * 0.6, m_fixedSize.height() * 0.6, Qt::AspectRatioMode::KeepAspectRatio));
    }
    QWidget::resizeEvent(e);
}
