#ifndef COMMONFUNCTIONS_H
#define COMMONFUNCTIONS_H

#include <QWidget>

namespace CommonFunctions
{

void showAnimatedVertical(QWidget* pTarget, int maxHeight, int timeMs = 150);
void hideAnimatedVertical(QWidget* pTarget, int maxHeight, int timeMs = 150);

void showAnimatedHorizontal(QWidget* pTarget, int maxWidth, int timeMs = 150);
void hideAnimatedHorizontal(QWidget* pTarget, int maxWidth, int timeMs = 150);

}

#endif // COMMONFUNCTIONS_H
