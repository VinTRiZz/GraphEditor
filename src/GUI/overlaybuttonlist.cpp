#include "overlaybuttonlist.h"

#include <QParallelAnimationGroup>
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
    updateConfiguration();
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

    updateConfiguration();
}

void OverlayButtonList::setMovable(bool isMovable)
{
    m_isMovable = isMovable;
}

void OverlayButtonList::setWidgetPosition(int leftWP, int topWP)
{
    m_offsets[ButtonOpenDirection::Left] = leftWP;
    m_offsets[ButtonOpenDirection::Up] = topWP;
    setGeometry(m_offsets[ButtonOpenDirection::Left], m_offsets[ButtonOpenDirection::Up], width(), height());
    updateConfiguration();
}

void OverlayButtonList::setButtonSize(const QSize &bSize_)
{
    setGeometry(x(), y(), bSize_.width(), bSize_.height());
}

void OverlayButtonList::setAnimationSpeed(double increaser_)
{
    if (increaser_ < 0) {
        throw std::invalid_argument("OverlayButtonList: increaser less than 0");
    }
    m_animationMultiplier = increaser_;
}

uint OverlayButtonList::addButton(const ButtonInfo &button_)
{
    m_buttonsInfo.push_back(button_);
    m_buttons.push_back(new QPushButton(parentWidget()));
    setupButton(m_buttons.back(), button_);

    hideButtons();
    showButtons();

    return m_buttons.size();
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

void OverlayButtonList::showButtons()
{
    for (auto pButton : m_buttons) {
        pButton->show();
    }

    uint directionButtonCount {0};

    auto moveDirected = [&](ButtonOpenDirection moveDir, bool inverseMovement = false) {
        uint currentOffset {1};
        while ((directionButtonCount < m_buttons.size()) && (m_maxButtonCounts[moveDir] > directionButtonCount)) {
            auto pButton = m_buttons[directionButtonCount];
            auto anim = new QPropertyAnimation(pButton, "geometry");

            QRect positionRect;
            positionRect.setX(this->x());
            positionRect.setY(this->y());
            positionRect.setWidth(width());
            positionRect.setHeight(height());
            anim->setStartValue(positionRect);

            auto separatorLength = m_buttonMargins + positionRect.width();
            bool isHorizontalMove = static_cast<bool>(moveDir & ButtonOpenDirection::Left) || static_cast<bool>(moveDir & ButtonOpenDirection::Right);
            auto deltaX = separatorLength * isHorizontalMove * currentOffset;
            deltaX = inverseMovement ? deltaX * -1.0 : deltaX;
            auto targetX = positionRect.x() + deltaX * -1 * static_cast<bool>(moveDir & ButtonOpenDirection::Left) + deltaX * static_cast<bool>(moveDir & ButtonOpenDirection::Right);

            separatorLength = m_buttonMargins + positionRect.height();
            bool isVerticalMove = static_cast<bool>(moveDir & ButtonOpenDirection::Up) || static_cast<bool>(moveDir & ButtonOpenDirection::Down);
            auto deltaY = separatorLength * isVerticalMove * currentOffset;
            deltaY = inverseMovement ? deltaY * -1.0 : deltaY;
            auto targetY = positionRect.y() + deltaY * -1 * static_cast<bool>(moveDir & ButtonOpenDirection::Up) + deltaX * static_cast<bool>(moveDir & ButtonOpenDirection::Down);

            positionRect.moveTo(targetX, targetY);
            anim->setEndValue(positionRect);

            anim->setDuration(std::max(deltaX / separatorLength, deltaY / separatorLength) * 300 / m_animationMultiplier);
            connect(anim, &QPropertyAnimation::finished,
                    this, [anim]() {
                delete anim;
            });
            anim->start();

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
    m_isButtonsHidden = false;
}

void OverlayButtonList::hideButtons()
{
    uint directionButtonCount {0};

    auto moveDirected = [&](ButtonOpenDirection moveDir, bool inverseMovement = false) {
        uint currentOffset {1};
        while ((directionButtonCount < m_buttons.size()) && (m_maxButtonCounts[moveDir] > directionButtonCount)) {
            auto pButton = m_buttons[directionButtonCount];
            auto anim = new QPropertyAnimation(pButton, "geometry", this);

            QRect positionRect;
            positionRect.setX(this->x());
            positionRect.setY(this->y());
            positionRect.setWidth(width());
            positionRect.setHeight(height());
            anim->setEndValue(positionRect);

            auto separatorLength = m_buttonMargins + positionRect.width();
            bool isHorizontalMove = static_cast<bool>(moveDir & ButtonOpenDirection::Left) || static_cast<bool>(moveDir & ButtonOpenDirection::Right);
            auto deltaX = separatorLength * isHorizontalMove * currentOffset;
            deltaX = inverseMovement ? deltaX * -1.0 : deltaX;
            auto targetX = positionRect.x() + deltaX * -1 * static_cast<bool>(moveDir & ButtonOpenDirection::Left) + deltaX * static_cast<bool>(moveDir & ButtonOpenDirection::Right);

            separatorLength = m_buttonMargins + positionRect.height();
            bool isVerticalMove = static_cast<bool>(moveDir & ButtonOpenDirection::Up) || static_cast<bool>(moveDir & ButtonOpenDirection::Down);
            auto deltaY = separatorLength * isVerticalMove * currentOffset;
            deltaY = inverseMovement ? deltaY * -1.0 : deltaY;
            auto targetY = positionRect.y() + deltaY * -1 * static_cast<bool>(moveDir & ButtonOpenDirection::Up) + deltaX * static_cast<bool>(moveDir & ButtonOpenDirection::Down);

            positionRect.moveTo(targetX, targetY);
            anim->setStartValue(positionRect);

            anim->setDuration(std::max(deltaX / separatorLength, deltaY / separatorLength) * 300 / m_animationMultiplier);
            connect(anim, &QPropertyAnimation::finished,
                    this, [anim, pButton]() {
                delete anim;
                pButton->hide();
            });
            anim->start();

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

    m_isButtonsHidden = true;
}

void OverlayButtonList::setupButton(QPushButton *pButton, const ButtonInfo &buttonInfo)
{
    pButton->setText(buttonInfo.name);
    pButton->setStyleSheet(buttonInfo.styleSheet);
    pButton->setToolTip(buttonInfo.tooltip);
    pButton->setIcon(buttonInfo.icon);
    pButton->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
    pButton->setHidden(m_isButtonsHidden);

    if (buttonInfo.action) {
        connect(pButton, &QPushButton::clicked, pButton, buttonInfo.action);
    }
}

void OverlayButtonList::setupSignals()
{
    connect(this, &QPushButton::clicked,
            this, [this](){
        if (m_buttons.size()) {
            if (m_isButtonsHidden) {
                showButtons();
            } else {
                hideButtons();
            }
        }
    });
}

void OverlayButtonList::setupWidget()
{
    setIcon(QIcon::fromTheme("settings"));
    setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
    setGeometry(x(), y(), 40, 40);
}

void OverlayButtonList::updateConfiguration()
{
    for (auto pButton : m_buttons) {
        pButton->setGeometry(pButton->x(), pButton->y(), width(), height());
    }

    hideButtons();
    showButtons();
}

void OverlayButtonList::paintEvent(QPaintEvent *e)
{
    if (parentWidget() == nullptr) {
        QPushButton::paintEvent(e);
        return;
    }

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

    bool isRightPosition =
            ((parentMaxX - positionX) == m_offsets[ButtonOpenDirection::Left]) &&
            ((parentMaxY - positionX) == m_offsets[ButtonOpenDirection::Up]) &&
            (positionX < parentMaxX) &&
            (positionY < parentMaxY)
    ;
    if (isRightPosition) {
        QPushButton::paintEvent(e);
        return;
    }

    positionX = m_offsets[ButtonOpenDirection::Left] > -1 ? m_offsets[ButtonOpenDirection::Left] : positionX;
    positionX = positionX < parentMaxX ? positionX : parentMaxX;
    positionX = positionX < 0 ? 0 : positionX;

    positionY = m_offsets[ButtonOpenDirection::Up] > -1 ? m_offsets[ButtonOpenDirection::Up] : positionY;
    positionY = positionY < parentMaxY ? positionY : parentMaxY;
    positionY = positionY < 0 ? 0 : positionY;

    if (!m_isButtonsHidden) {
        auto deltaX = positionX - x();
        auto deltaY = positionY - y();

        for (auto pButton : m_buttons) {
            pButton->move(pButton->x() + deltaX, pButton->y() + deltaY);
        }
    }

    move(positionX, positionY);
    QPushButton::paintEvent(e);
}
