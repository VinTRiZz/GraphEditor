#ifndef GRAPHVIEWBASE_H
#define GRAPHVIEWBASE_H

#include "graphmodebase.h"

#include <GraphObject/Maintainer.h>
#include <ObjectScene/ObjectView.h>
#include <CustomWidgets/ButtonMatrix.h>

#include <boost/noncopyable.hpp>

namespace Graph
{

/**
 * @brief The GraphSceneBase class Основа класса сцены для отображения графа
 */
class GraphSceneBase : public ObjectView
{
public:
    GraphSceneBase(QWidget* parent = nullptr);
    ~GraphSceneBase();

    /**
     * @brief init Инициализация сцены. Обязательна к вызову
     */
    virtual void init();

    /**
     * @brief setMode   Выбрать текущий режим работы
     * @param pMode     Указатель на режим работы. Не удалится после удаления сцены
     */
    void setMode(GraphModeBase* pMode);

    /**
     * @brief writeChangesToGraph   Записать данные сцены в граф (фактически, применить изменения)
     */
    void writeChangesToGraph();

    /**
     * @brief setCurrentGraph   Задать текущий граф
     * @param pGraphMaintaner   Указатель на поставщика текущего графа
     */
    virtual void setGraphMaintaner(const Graph::PMaintainer& pGraphMaintaner);

    /**
     * @brief updateGraph Считать данные из текущего графа. Вызывается также при установке графа
     */
    void updateGraph();

    /**
     * @brief getCurrentGraph   Получить текущий граф
     * @return                  nullptr если не задан или заданный граф
     */
    Graph::PMaintainer getGraphMaintaner() const;

    /**
     * @brief getButtonMatrixHead   Получить головную кнопку матрицы кнопок
     * @return                      Указатель
     */
    ButtonMatrix::HeadButton* getButtonMatrixHead() const;

    /**
     * @brief createConnectionLine  Создать соединительную линию для вершин
     * @param idFrom                Вершина от которой идёт линия
     * @param idTo                  Вершина до которой идёт линия
     * @return                      Указатель на объект, добавленный на сцену
     */
    ObjectViewItems::VertexConnectionLine* createConnectionLine(ObjectViewConstants::objectId_t idFrom, ObjectViewConstants::objectId_t idTo);

    /**
     * @brief createVertex  Создать вершину
     * @return              Указатель на объект, добавленный на сцену
     */
    ObjectViewItems::VertexObject* createVertex();

private:
    void resizeEvent(QResizeEvent* e) override;
    ObjectViewItems::VertexObject* createVertex(ObjectViewConstants::objectId_t vId);

    ObjectViewConstants::objectId_t         m_currentItemId {1};            //! Текущий идентификатор объекта сцены. Используется для создания объектов
    GraphModeBase*                          m_pCurrentMode {nullptr};       //! Текущий режим работы
    Graph::PMaintainer  m_pGraphMaintaner;              //! Текущий граф
    ButtonMatrix::HeadButton*               m_buttonMatrixHead {nullptr};   //! Указатель на кнопку-матрицу (инструментарий)
};

}

#endif // GRAPHVIEWBASE_H
