#ifndef GRAPHVIEWBASE_H
#define GRAPHVIEWBASE_H

#include "../GraphEdit/graphextendedobject.h"
#include "graphmodebase.h"

#include "GUI/ObjectScene/objectview.h"
#include "GUI/CustomWidgets/buttonmatrix.h"

#include <boost/noncopyable.hpp>

namespace Graph
{

/**
 * @brief The GraphConversionConfiguration class Структура для настройки конверсий графов
 */
struct GraphConversionConfiguration : boost::noncopyable
{
    // Уровни расположения объектов на сцене по их типу
    uint mainRectLayer       = 1;    //! Уровень главного объекта ("полотна" графа)

    uint connectionLineLayer = 10;   //! Уровень линий соединения вершин

    uint vertexLayer         = 30;   //! Уровень вершин
    uint vertexDataRectLayer = 31;   //! Уровень рамки текста названий вершин
    uint vertexDataLayer     = 32;   //! Уровень текста названий вершин

    static GraphConversionConfiguration& getInstance() {
        static GraphConversionConfiguration inst;
        return inst;
    }
};

class GraphSceneBase : public ObjectView
{
public:
    enum ObjectType : int {
        OBJECT_TYPE_UNKNOWN,
        OBJECT_TYPE_VERTEX,
        OBJECT_TYPE_CONNECTION,
    };

    GraphSceneBase(QWidget* parent = nullptr);
    ~GraphSceneBase();

    virtual void init();

    void setMode(GraphModeBase* pMode);

    /**
     * @brief setCurrentGraph   Задать текущий граф
     * @param pGraph            Указатель на текущий граф
     */
    virtual void setCurrentGraph(Graph::GraphExtendedObject* pGraph);

    /**
     * @brief getCurrentGraph   Получить текущий граф
     * @return                  nullptr если не задан или заданный граф
     */
    Graph::GraphExtendedObject* getCurrentGraph() const;

protected:
    Graph::GraphExtendedObject* m_pGraph {nullptr};
    GraphModeBase*              m_pCurrentMode {nullptr};

    void updateGraph();
};

}

#endif // GRAPHVIEWBASE_H
