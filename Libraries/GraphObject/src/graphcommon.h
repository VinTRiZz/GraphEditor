#ifndef GRAPHCOMMON_H
#define GRAPHCOMMON_H

#include <QColor>
#include <QJsonDocument>
#include <QVariant>
#include <QVector>

namespace GraphCommon {

using graphId_t = long long;

/**
 * @brief The GraphProperty enum Обычные для всех свойства графа
 */
enum GraphProperty {
    GRAPHPROPERTY_NAME,         //! Название графа
    GRAPHPROPERTY_DESCRIPTION,  //! Описание графа
    GRAPHPROPERTY_CREATETIME,   //! Время создания
    GRAPHPROPERTY_EDITTIME,     //! Время последнего изменения
};

const QString DATE_CONVERSION_FORMAT =
    "HH:mm:ss_dd.MM.yyyy";  //! Формат конвертирования даты в строку
const QString DATE_DISPLAY_CONVERSION_FORMAT =
    "HH:mm:ss dd.MM.yyyy";  //! Формат конвертирования даты в строку на показ

}  // namespace GraphCommon

#endif  // GRAPHCOMMON_H
