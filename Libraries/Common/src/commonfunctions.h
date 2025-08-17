#ifndef COMMONFUNCTIONS_H
#define COMMONFUNCTIONS_H

#include <QLabel>
#include <QPushButton>
#include <QWidget>

namespace CommonFunctions {

/**
 * @brief showAnimatedVertical  Анимированно показывает виджет по вертикали (сверху вниз)
 * @param pTarget               Целевой виджет
 * @param maxHeight             Высота после анимации
 * @param timeMs                Время анимации, мс
 */
void showAnimatedVertical(QWidget* pTarget, int maxHeight, int timeMs = 150);

/**
 * @brief hideAnimatedVertical  Анимированно скрывает виджет по вертикали (снизу вверх)
 * @param pTarget               Целевой виджет
 * @param maxHeight             Высота до анимации
 * @param timeMs                Время анимации, мс
 */
void hideAnimatedVertical(QWidget* pTarget, int maxHeight, int timeMs = 150);

/**
 * @brief showAnimatedHorizontal Анимированно показывает виджет по горизонтали (справа налево)
 * @param pTarget               Целевой виджет
 * @param maxWidth              Ширина до анимации
 * @param timeMs                Время анимации, мс
 */
void showAnimatedHorizontal(QWidget* pTarget, int maxWidth, int timeMs = 150);

/**
 * @brief hideAnimatedHorizontal Анимированно скрывает виджет по горизонтали (слева направо)
 * @param pTarget               Целевой виджет
 * @param maxWidth              Ширина до анимации
 * @param timeMs                Время анимации, мс
 */
void hideAnimatedHorizontal(QWidget* pTarget, int maxWidth, int timeMs = 150);

/**
 * @brief connectColorDialog    Соединить кнопку с открытием файлового диалога
 * @param pButton               Указатель на кнопку
 * @param pTargetLabel          Указатель на QLabel, которая после задания будет иметь выбранный цвет
 */
void connectColorDialog(QPushButton* pButton, QLabel* pTargetLabel);

/**
 * @brief setColor  Задать цвет в QLabel
 * @param pLabel    Целевая QLabel
 * @param color     Цвет для задания
 */
void setColor(QLabel* pLabel, const QColor& color);

/**
 * @brief getColor  Получить цвет QLabel
 * @param pLabel    Целевая QLabel
 * @return          Цвет, который был задан функцией setColor() (см. выше)
 */
QColor getColor(QLabel* pLabel);

/**
 * @brief pixmapFromPath    Считать изображение
 * @param targetPath        Путь до файла
 * @param scaleSize         Размер, до которого сжать/расширить изображение
 * @return                  NULL QPixmap если не удалось считать
 */
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
