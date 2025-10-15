#pragma once

#include <boost/noncopyable.hpp>

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

}
