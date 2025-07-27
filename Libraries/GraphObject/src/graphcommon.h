#ifndef GRAPHCOMMON_H
#define GRAPHCOMMON_H

#include <QJsonDocument>
#include <QVariant>
#include <QVector>

#include <QColor>

namespace GraphCommon
{

using graphId_t = long long;

/**
 * @brief The GraphProperty enum Обычные для всех свойства графа
 */
enum GraphProperty
{
    GRAPHPROPERTY_NAME,         //! Название графа
    GRAPHPROPERTY_DESCRIPTION,  //! Описание графа
    GRAPHPROPERTY_CREATETIME,   //! Время создания
    GRAPHPROPERTY_EDITTIME,     //! Время последнего изменения
};

const QColor DEFAULT_VERTEX_COLOR {QColor("#ecd1a6")};          //! Цвет вершин по умолчанию
const QColor DEFAULT_VERTEX_BORDER_COLOR {Qt::black};   //! Цвет границ вершин по умолчанию
const QColor DEFAULT_VERTEX_TEXT_BGR_COLOR {QColor("#f3f5dd")}; //! Цвет фона текста названий вершин по умолчанию

const QString DATE_CONVERSION_FORMAT = "HH:mm:ss_dd.MM.yyyy";          //! Формат конвертирования даты в строку
const QString DATE_DISPLAY_CONVERSION_FORMAT = "HH:mm:ss dd.MM.yyyy";  //! Формат конвертирования даты в строку на показ


/**
 * @brief encodeColor   Конвертировать цвет во внутренний способ хранения
 * @param iCol          Входной цвет
 * @return              Массив байт с кодом цвета
 */
QByteArray encodeColor(const QColor& iCol);

/**
 * @brief decodeColor   Получить цвет из массива байт внутреннего способа хранения
 * @param iBytes        Входной массив байт
 * @return              Декодированный цвет
 */
QColor decodeColor(const QByteArray &iBytes);

}

#endif // GRAPHCOMMON_H
