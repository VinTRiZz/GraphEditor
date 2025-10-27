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

}
