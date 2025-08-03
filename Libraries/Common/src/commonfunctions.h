#ifndef COMMONFUNCTIONS_H
#define COMMONFUNCTIONS_H

#include <QLabel>
#include <QPushButton>
#include <QWidget>

namespace CommonFunctions {

void showAnimatedVertical(QWidget* pTarget, int maxHeight, int timeMs = 150);
void hideAnimatedVertical(QWidget* pTarget, int maxHeight, int timeMs = 150);

void showAnimatedHorizontal(QWidget* pTarget, int maxWidth, int timeMs = 150);
void hideAnimatedHorizontal(QWidget* pTarget, int maxWidth, int timeMs = 150);

void connectColorDialog(QPushButton* pButton, QLabel* pTargetLabel);
void setColor(QLabel* pLabel, const QColor& color);
QColor getColor(QLabel* pLabel);

QPixmap pixmapFromPath(const QString& targetPath, const QSize& scaleSize);

}  // namespace CommonFunctions

#endif  // COMMONFUNCTIONS_H
