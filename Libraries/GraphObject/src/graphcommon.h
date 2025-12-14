#pragma once

#include <QColor>
#include <QJsonDocument>
#include <QVariant>
#include <QVector>

#include <optional>

namespace Graph {

using graphId_t = std::optional<long long>;

/**
 * @brief The GraphProperty enum Обычные для всех свойства графа
 */
enum GraphProperty {
    GRAPHPROPERTY_NAME = 0,     //! Название графа
    GRAPHPROPERTY_DESCRIPTION,  //! Описание графа
    GRAPHPROPERTY_CREATETIME,   //! Время создания
    GRAPHPROPERTY_EDITTIME,     //! Время последнего изменения
};

const QString DATE_CONVERSION_FORMAT =
    "HH:mm:ss_dd.MM.yyyy";  //! Формат конвертирования даты в строку
const QString DATE_DISPLAY_CONVERSION_FORMAT =
    "HH:mm dd.MM.yyyy";  //! Формат конвертирования даты в строку на показ

namespace GObjectValueName {

// Основная информация
const QString COMMON_PLUGIN_NAME         {"pluginName"};
const QString COMMON_PLUGIN_OBJECTNAME   {"pluginObjectName"};

// Основная для графического объекта информация
const QString COMMON_TOOLTIP     {"tooltip"};
const QString COMMON_DESCRIPTION {"description"};
const QString COMMON_TITLEPOS    {"titlePosition"};

// Графическая информация
const QString COMMON_LINEPEN     {"linePen"};
const QString COMMON_BGRBRUSH    {"backgroundBrush"};

}

}  // namespace Graph
