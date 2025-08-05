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

/**
 * @brief encodeColor   Конвертировать цвет во внутренний способ хранения
 * @param iCol          Входной цвет
 * @return              Массив байт с кодом цвета
 */
QByteArray encodeColor(const QColor& iCol);
QByteArray encodeColorGSE(const QColor& iCol);

/**
 * @brief decodeColor   Получить цвет из массива байт внутреннего способа
 * хранения
 * @param iName         Входной массив байт
 * @return              Декодированный цвет
 */
QColor decodeColor(const QString& iName);
QColor decodeColorGSE(const QString& iName);

}  // namespace CommonFunctions

#endif  // COMMONFUNCTIONS_H
