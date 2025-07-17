#ifndef GRAPHVIEWBASE_H
#define GRAPHVIEWBASE_H

#include "../GraphEdit/graphextendedobject.h"
#include "graphmodebase.h"

#include "GUI/ObjectScene/objectview.h"
#include "GUI/CustomWidgets/buttonmatrix.h"
#include "GUI/ObjectScene/predefinedobjects.h"

#include <boost/noncopyable.hpp>

namespace Graph
{

/**
 * @brief The GraphConversionConfiguration class Структура для настройки конверсий графов
 */
struct GraphSceneConfiguration : boost::noncopyable
{
    // Уровни расположения объектов на сцене по их типу
    uint connectionLineLayer = 10;   //! Уровень линий соединения вершин
    uint vertexLayer         = 20;   //! Уровень вершин

    double vertexWidth       = 100;  //! Ширина и высота вершины

    static GraphSceneConfiguration& getInstance() {
        static GraphSceneConfiguration inst;
        return inst;
    }
};

class GraphSceneBase : public ObjectView
{
public:
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

    /**
     * @brief getButtonMatrixHead   Получить головную кнопку матрицы кнопок
     * @return                      Указатель
     */
    ButtonMatrix::HeadButton* getButtonMatrixHead() const;

    PredefinedObjects::VertexConnectionLine* createConnectionLine();
    PredefinedObjects::VertexObject* createVertex();

protected:
    Graph::GraphExtendedObject* m_pGraph {nullptr};
    GraphModeBase*              m_pCurrentMode {nullptr};
    ButtonMatrix::HeadButton*   m_buttonMatrixHead {nullptr};

    void updateGraph();
};

}

#endif // GRAPHVIEWBASE_H
