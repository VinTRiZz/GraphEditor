#ifndef GRAPHVIEWBASE_H
#define GRAPHVIEWBASE_H

#include "graphextendedobject.h"

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


class GraphViewBase : public ObjectView
{
    // Блокировка работы ObjectView во избежание поломки интерфейса
    using ObjectView::setScene;

public:
    GraphViewBase(QWidget* parent = nullptr);
    ~GraphViewBase();

    virtual void init();

    virtual void startMode() = 0;
    virtual bool isModeStarted() const = 0;
    virtual void stopMode() = 0;

    /**
     * @brief setCurrentGraph   Задать текущий граф
     * @param pGraph            Указатель на текущий граф
     */
    void setCurrentGraph(Graph::GraphExtendedObject* pGraph);

private:
    struct Impl;
    std::unique_ptr<Impl> d;

protected:
    Graph::GraphExtendedObject* m_pGraph {nullptr};
    ButtonMatrix::HeadButton    m_overlayButton;

    void updateGraph();
};

}

#endif // GRAPHVIEWBASE_H
