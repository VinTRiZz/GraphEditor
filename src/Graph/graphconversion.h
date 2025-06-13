#ifndef GRAPHCONVERSION_H
#define GRAPHCONVERSION_H

#include <QGraphicsItem>

#include <boost/noncopyable.hpp>

#include "graphobject.h"

namespace GraphConversion
{

/**
 * @brief The GraphConversionConfiguration class Структура для настройки конверсий графов
 */
struct GraphConversionConfiguration : boost::noncopyable
{
    // Уровни расположения объектов на сцене по их типу
    uint mainRectLayer       = 1;    //! Уровень главного объекта ("полотна" графа)

    uint connectionLineLayer = 10;   //! Уровень линий соединения вершин
    uint connectionRectLayer = 11;   //! Уровень прямоугольников с названиями вершин (для контраста)
    uint connectionTextLayer = 12;   //! Уровень текста названий вершин

    uint vertexLayer         = 30;   //! Уровень вершин
    uint vertexDataLayer     = 31;   //! Уровень текста названий вершин

    static GraphConversionConfiguration& getInstance() {
        static GraphConversionConfiguration inst;
        return inst;
    }
};

/**
 * @brief toItem        Конвертировать граф в объект сцены
 * @param targetGraph   Целевой объект графа для конвертации
 * @return              Указатель на объект сцены
 */
QGraphicsItem* toItem(const Graph::GraphObject& targetGraph);

/**
 * @brief fromItem      Получить объект графа из объекта сцены
 * @param pItem         Указатель на объект сцены, из которого берутся данные
 * @param targetGraph   Целевой объект графа (будет обнулён перед записью)
 * @return              true при успешной конвертации
 */
bool fromItem(const QGraphicsItem* pItem, Graph::GraphObject& targetGraph);


}

#endif // GRAPHCONVERSION_H
