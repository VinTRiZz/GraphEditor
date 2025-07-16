#include "buttonmatrix.h"

#include <QPropertyAnimation>

#include <math.h>

namespace ButtonMatrix
{


HeadButton::HeadButton(QWidget *parent) :
    QPushButton(parent)
{
    setupSignals();
}

HeadButton::~HeadButton()
{

}

bool HeadButton::addButton(const ButtonConfig &bConfig)
{
    if (!isValid(bConfig.positionX, bConfig.positionY)) {
        return false;
    }
    auto mappedPos = mapToMatrix(bConfig.positionX, bConfig.positionY);
    auto& buttonConfig = m_buttonMatrix[mappedPos.first][mappedPos.second];
    buttonConfig.conf = bConfig;
    buttonConfig.pButton = new QPushButton(parentWidget());
    setupButton(buttonConfig.pButton, bConfig);
    return true;
}

std::optional<ButtonConfig> HeadButton::getButtonConfig(int buttonX, int buttonY)
{
    if (!isValid(buttonX, buttonY)) {
        return std::nullopt;
    }
    auto mappedPos = mapToMatrix(buttonX, buttonY);
    return m_buttonMatrix[mappedPos.first][mappedPos.second].conf;
}

bool HeadButton::updateButton(const ButtonConfig &bConfig)
{
    if (!isValid(bConfig.positionX, bConfig.positionY)) {
        return false;
    }
    auto mappedPos = mapToMatrix(bConfig.positionX, bConfig.positionY);
    m_buttonMatrix[mappedPos.first][mappedPos.second].conf = bConfig;
    setupButton(m_buttonMatrix[mappedPos.first][mappedPos.second].pButton, bConfig);
    return false;
}

void HeadButton::removeButton(int buttonX, int buttonY)
{
    if (!isValid(buttonX, buttonY)) {
        return;
    }
    auto mappedPos = mapToMatrix(buttonX, buttonY);
    auto& buttonConf = m_buttonMatrix[mappedPos.first][mappedPos.second];
    delete buttonConf.pButton;
    buttonConf = {};
}

void HeadButton::setButtonMatrix(int xMin, int yMin, int xMax, int yMax)
{
    if (xMax - xMin == 0 || yMax - yMin == 0) {
        throw std::invalid_argument("HeadButton::setButtonMatrix Invalid matrix size (0 rows or 0 columns)");
    }

    if (xMax < xMin || yMax < yMin) {
        throw std::invalid_argument("HeadButton::setButtonMatrix Invalid matrix size (max coordinate less than min)");
    }

    for (const auto& buttonRow : m_buttonMatrix) {
        for (auto& button : buttonRow) {
            delete button.pButton;
        }
    }

    m_buttonMatrix.resize(boost::extents[std::abs(xMax - xMin) + 1][std::abs(yMax - yMin) + 1]);

    m_buttonMatrixMinimum.first     = xMin;
    m_buttonMatrixMinimum.second    = yMin;
}

void HeadButton::setButtonPadding(unsigned int left, unsigned int right, unsigned int top, unsigned int bottom)
{
    m_paddingLeft   = left;
    m_paddingRight  = right;
    m_paddingTop    = top;
    m_paddingBottom = bottom;
    moveSelf();
}

void HeadButton::setIcons(const QIcon &collapsedIcon, const QIcon &expandedIcon)
{
    m_expandedIcon  = expandedIcon;
    m_collapsedIcon = collapsedIcon;

    setIcon(m_isButtonsExpanded ? expandedIcon : collapsedIcon);
}

void HeadButton::setAnimationSpeed(double animationMultiplier)
{
    m_animationMultiplier = animationMultiplier;
}

void HeadButton::setButtonMargin(double buttonMargins)
{
    m_buttonMargins = buttonMargins;
    if (m_isButtonsExpanded) {
        expand(false);
    }
}

void HeadButton::setButtonsSize(const QSize &iSize)
{
    m_fixedSize = iSize;
    resize(iSize);
    if (m_isButtonsExpanded) {
        expand(false);
    }
}

QPushButton *HeadButton::getButton(int x, int y)
{
    if (!isValid(x, y)) {
        return nullptr;
    }

    auto targetPos = mapToMatrix(x, y);
    return m_buttonMatrix[targetPos.first][targetPos.second].pButton;
}

std::list<ButtonConfig> HeadButton::getAllButtons() const
{
    std::list<ButtonConfig> res;
    for (const auto& row : m_buttonMatrix) {
        for (auto& rButton : row) {
            res.push_back(rButton.conf);
        }
    }
    return res;
}

void HeadButton::expand(bool isAnimated)
{
    emit stopAnimations();
    auto matrixShape = m_buttonMatrix.shape();

    setIcon(m_expandedIcon);
    for (unsigned posX = 0; posX < matrixShape[0]; ++posX) {
        for (unsigned posY = 0; posY < matrixShape[1]; ++posY) {
            moveButton(posX, posY, isAnimated, false);
        }
    }
    m_isButtonsExpanded = true;
}

void HeadButton::collapse(bool isAnimated)
{
    emit stopAnimations();
    auto matrixShape = m_buttonMatrix.shape();

    setIcon(m_collapsedIcon);
    for (unsigned posX = 0; posX < matrixShape[0]; ++posX) {
        for (unsigned posY = 0; posY < matrixShape[1]; ++posY) {
            moveButton(posX, posY, isAnimated, true);
        }
    }
    m_isButtonsExpanded = false;
}

void HeadButton::setupSignals()
{
    connect(this, &QPushButton::clicked,
            this, [this]() {
        if (m_isButtonsExpanded) {
            collapse(true);
        } else {
            expand(true);
        }
    });
}

bool HeadButton::isValid(int xpos, int ypos)
{
    if (xpos == 0 && ypos == 0) [[unlikely]] {
        return false;
    }

    auto xMatrixPos = mapToMatrixX(xpos);
    auto yMatrixPos = mapToMatrixY(ypos);
    auto matrixShape = m_buttonMatrix.shape();

    return  (xMatrixPos > -1) && (static_cast<unsigned>(xMatrixPos) < matrixShape[0]) &&
            (yMatrixPos > -1) && (static_cast<unsigned>(yMatrixPos) < matrixShape[1]);
}

int HeadButton::mapToMatrixX(int xpos)
{
    return xpos - m_buttonMatrixMinimum.first;
}

int HeadButton::mapToMatrixY(int ypos)
{
    return ypos - m_buttonMatrixMinimum.second;
}

std::pair<int, int> HeadButton::mapToMatrix(int xpos, int ypos)
{
    return {mapToMatrixX(xpos), mapToMatrixY(ypos)};
}

std::pair<int, int> HeadButton::mapFromMatrix(int xpos, int ypos)
{
    std::pair<int, int> res {(xpos + m_buttonMatrixMinimum.first), (ypos + m_buttonMatrixMinimum.second)};

    res.first   = x() + res.first * (width() + m_buttonMargins);
    res.second  = y() - res.second * (height() + m_buttonMargins);

    return res;
}

void HeadButton::moveSelf()
{
    int x = m_paddingRight == 0 ? m_paddingLeft : std::max(parentWidget()->width() - width() - m_paddingRight, m_paddingLeft);
    int y = m_paddingBottom == 0 ? m_paddingTop : std::max(parentWidget()->height() - height() - m_paddingBottom, m_paddingTop);
    move(x, y);
}

void HeadButton::moveButton(int xpos, int ypos, bool isAnimated, bool hideOnFinish)
{
    auto pButton = m_buttonMatrix[xpos][ypos].pButton;
    if (nullptr == pButton) {
        return;
    }

    auto startRect = rect();
    auto startPosition = mapFromMatrix(xpos, ypos);
    startRect.moveTo(startPosition.first, startPosition.second);

    auto targetRect = geometry();

    if (!hideOnFinish) {
        std::swap(startRect, targetRect);
    }

    if (!hideOnFinish) {
        pButton->show();
    }

    pButton->setFixedSize(m_fixedSize);
    pButton->setIconSize(m_fixedSize.scaled(m_fixedSize.width() * 0.6, m_fixedSize.height() * 0.6, Qt::AspectRatioMode::KeepAspectRatio));
    if (!isAnimated) {
        pButton->move(targetRect.topLeft());
        return;
    }

    auto anim = new QPropertyAnimation(pButton, "geometry");
    anim->setStartValue(startRect);
    anim->setEndValue(targetRect);

    auto animationTime = (startPosition.first + startPosition.second) / 2.5 * m_animationMultiplier;
    anim->setDuration(animationTime);

    connect(this, &HeadButton::stopAnimations,
            anim, &QPropertyAnimation::stop);

    if (hideOnFinish) {
        connect(anim, &QPropertyAnimation::finished,
                this, [pButton]() {
            pButton->hide();
        });
    }

    anim->start(QPropertyAnimation::DeleteWhenStopped);
}

void HeadButton::setupButton(QPushButton *pButton, const ButtonConfig &buttonInfo)
{
    pButton->setText(buttonInfo.name);
    pButton->setStyleSheet(buttonInfo.styleSheet);
    pButton->setToolTip(buttonInfo.tooltip);
    pButton->setIcon(buttonInfo.icon);
    pButton->setFixedSize(m_fixedSize);
    pButton->setHidden(!m_isButtonsExpanded);
    pButton->setEnabled(buttonInfo.isEnabled);

    if (!buttonInfo.action) {
        return;
    }

    connect(pButton, &QPushButton::clicked,
            pButton, [this, buttonInfo, pButton]() {
        buttonInfo.action(pButton);
        emit stopAnimations();
        if (m_isButtonsExpanded) {
            expand(false);
        } else {
            collapse(false);
        }
    });
}

void HeadButton::paintEvent(QPaintEvent *e)
{
    moveSelf();
    QPushButton::paintEvent(e);
}

void HeadButton::resizeEvent(QResizeEvent *e)
{
    if (size() != m_fixedSize) {
        setFixedSize(m_fixedSize);
        setIconSize(m_fixedSize.scaled(m_fixedSize.width() * 0.85, m_fixedSize.height() * 0.85, Qt::AspectRatioMode::KeepAspectRatio));
    }
    QWidget::resizeEvent(e);
}

void HeadButton::moveEvent(QMoveEvent *event)
{
    emit stopAnimations();
    if (m_isButtonsExpanded) {
        expand(false);
    } else {
        collapse(false);
    }
    QPushButton::moveEvent(event);
}


}
