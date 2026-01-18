#pragma once

#include <QColor>
#include <QJsonDocument>
#include <QVariant>
#include <QVector>

#include <optional>

#include <Components/CustomQt/ObjectView/ObjectItems.h>

#include "graphsettingsnames.hpp"

namespace Graph {

using graphId_t = ObjectItems::objectId_t;

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

enum GraphObjectTypes : int {
    OBJECTTYPE_VERTEX = ObjectItems::ObjectType::OIT_UserItemType,
    OBJECTTYPE_CONNECTION
};

namespace Layers {
const int VERTEX_LAYER {100};
const int CONNECTION_LAYER {50};
}

namespace Sizes {
const int VERTEX_RADIUS {50};
}

namespace Colors {
const QColor DEFAULT_COLOR_COMMON_BGR{QColor("#fcf0f0")};

const QColor DEFAULT_COLOR_VERTEX_LINE{Qt::black};
const QColor DEFAULT_COLOR_VERTEX_BGR {QColor("#ecd1a6")};
const QColor DEFAULT_COLOR_VERTEX_SEL {QColor("#5cff37")};

const QColor DEFAULT_COLOR_CONNECTION_LINE{QColor("#2a8d7c")};
const QColor DEFAULT_COLOR_CONNECTION_BGR {QColor("#5aadac")};
const QColor DEFAULT_COLOR_CONNECTION_SEL {QColor("#ffbc20")};
}


enum ConnectionEventType : unsigned short
{
    CET_AnyType = 0,
    CET_Feedback,

    CET_UserType = 50,
};

struct ConnectionEvent
{
    // Без задания ивент не будет сохранён
    graphId_t senderId      {};
    graphId_t receiverId    {};

    // Основные параметры
    bool isAccepted             {false};
    ConnectionEventType type    {CET_AnyType};
    void* pEventData            {nullptr};
    QByteArray eventExtraData;
};

}  // namespace Graph
