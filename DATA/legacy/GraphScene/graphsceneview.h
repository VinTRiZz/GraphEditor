#pragma once

#include <Components/CustomQt/ButtonMatrix.h>
#include <GraphObject/Maintainer.h>
#include <Components/CustomQt/ObjectView/InformationLayer.h>

#include <boost/noncopyable.hpp>

#include <GraphItems/VertexObjectItem.h>
#include <GraphItems/VertexConnectionItem.h>
#include "graphmodebase.h"

namespace Graph {

/**
 * @brief The GraphSceneBase class Основа класса сцены для отображения графа
 */
class GraphSceneView : public OVLayers::OVInformationLayer {
public:
    GraphSceneView(QWidget* parent = nullptr);
    ~GraphSceneView();

    /**
     * @brief setMode   Выбрать текущий режим работы
     * @param pMode     Указатель на режим работы. Не удалится после удаления
     * сцены
     */
    void setMode(GraphModeBase* pMode);

    /**
     * @brief writeChangesToGraph   Записать данные сцены в граф (фактически,
     * применить изменения)
     */
    void writeChangesToGraph();

    /**
     * @brief setCurrentGraph   Задать текущий граф
     * @param pGraphMaintaner   Указатель на поставщика текущего графа
     */
    virtual void setGraphMaintaner(const Graph::PMaintainer& pGraphMaintaner);

    /**
     * @brief updateGraph Считать данные из текущего графа. Вызывается также при
     * установке графа
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
    Graph::VertexConnectionLine* createConnectionLine(
        ObjectItems::objectId_t idFrom,
        ObjectItems::objectId_t idTo);

    /**
     * @brief createVertex  Создать вершину
     * @return              Указатель на объект, добавленный на сцену
     */
    Graph::VertexObjectItem* createVertex();

private:
    void resizeEvent(QResizeEvent* e) override;
    Graph::VertexObjectItem* createVertex(
        ObjectItems::objectId_t vId);

    ObjectItems::objectId_t m_currentItemId{
        1};  //! Текущий идентификатор объекта сцены. Используется для создания
             //! объектов
    GraphModeBase* m_pCurrentMode{nullptr};  //! Текущий режим работы
    Graph::PMaintainer m_pGraphMaintaner;    //! Текущий граф
    ButtonMatrix::HeadButton* m_buttonMatrixHead{
        nullptr};  //! Указатель на кнопку-матрицу (инструментарий)
};

}  // namespace Graph
