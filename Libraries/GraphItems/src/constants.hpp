#pragma once

#include <boost/noncopyable.hpp>

#include <Components/CustomQt/ObjectView/ObjectItems.h>

#include <QJsonObject>

#include <QGraphicsItem>

namespace Graph {

/**
 * @brief The GraphConversionConfiguration class Структура информации по уровням
 * объектов на сцене
 */
struct ObjectSceneConfiguration : public boost::noncopyable {
    // Уровни расположения объектов на сцене по их типу
    int connectionLineLayer = 10;  //! Уровень линий соединения вершин
    int vertexLayer = 20;          //! Уровень вершин

    int propertyEditorLayer = 100;  //! Уровень редактора свойств

    double vertexWidth = 100;  //! Ширина и высота вершины

    static ObjectSceneConfiguration& getInstance() {
        static ObjectSceneConfiguration inst;
        return inst;
    }
};

const int OBJECTTYPE_VERTEX {ObjectItems::OBJECTDATAROLE_USERTYPE + 10};
const int OBJECTTYPE_CONNECTION {OBJECTTYPE_VERTEX + 1};

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

enum VertexType : int {
    SimpleVertex = 0,
};

}
