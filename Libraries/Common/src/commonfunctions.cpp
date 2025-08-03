#include "commonfunctions.h"

#include <QPropertyAnimation>

namespace CommonFunctions
{

void showAnimatedVertical(QWidget *pTarget, int maxHeight, int timeMs)
{
    pTarget->show();

    QPropertyAnimation* animation = new QPropertyAnimation(pTarget, "minimumHeight");
    animation->setDuration(150);

    pTarget->setMaximumHeight(maxHeight);
    pTarget->setMinimumHeight(0);
    pTarget->setFixedHeight(0);
    animation->setStartValue(0);
    animation->setEndValue(maxHeight);
    animation->start(QPropertyAnimation::DeleteWhenStopped);
}

void hideAnimatedVertical(QWidget *pTarget, int maxHeight, int timeMs)
{
    QPropertyAnimation* animation = new QPropertyAnimation(pTarget, "maximumHeight");
    animation->setDuration(150);

    pTarget->setMaximumHeight(maxHeight);
    pTarget->setMinimumHeight(0);
    pTarget->setFixedHeight(0);
    animation->setStartValue(maxHeight);
    animation->setEndValue(0);
    animation->start(QPropertyAnimation::DeleteWhenStopped);

    pTarget->connect(animation, &QPropertyAnimation::finished,
            pTarget, &QWidget::hide);
}

void showAnimatedHorizontal(QWidget *pTarget, int maxWidth, int timeMs)
{
    pTarget->show();

    QPropertyAnimation* animation = new QPropertyAnimation(pTarget, "minimumWidth");
    animation->setDuration(150);

    pTarget->setMaximumWidth(maxWidth);
    pTarget->setMinimumWidth(0);
    pTarget->setFixedWidth(0);
    animation->setStartValue(0);
    animation->setEndValue(maxWidth);
    animation->start(QPropertyAnimation::DeleteWhenStopped);
}

void hideAnimatedHorizontal(QWidget *pTarget, int maxWidth, int timeMs)
{
    QPropertyAnimation* animation = new QPropertyAnimation(pTarget, "maximumWidth");
    animation->setDuration(150);

    pTarget->setMaximumWidth(maxWidth);
    pTarget->setMinimumWidth(0);
    pTarget->setFixedWidth(0);
    animation->setStartValue(maxWidth);
    animation->setEndValue(0);
    animation->start(QPropertyAnimation::DeleteWhenStopped);

    pTarget->connect(animation, &QPropertyAnimation::finished,
            pTarget, &QWidget::hide);
}

}
