#ifndef GRAPHOBJECT_H
#define GRAPHOBJECT_H

#include <math.h>

#include <optional>

#include "gvertex.h"
#include "gconnection.h"
#include "graphcommon.h"

namespace Graph
{

/**
 * @brief The GraphObject class Объект графа, заключающий в себе свойства графа и его наполнение
 */
class GraphObject
{
public:
    GraphObject();

    bool operator ==(const GraphObject& gObj_) const;
    bool operator !=(const GraphObject& gObj_) const;

    // ============================================================== //
    // ================= Работа с вершинами графа =================== //
    // ============================================================== //

    /**
     * @brief addVertex Добавить вершину в граф
     * @param iVert     Структура с описанием свойств вершины
     * @return          true если добавление успешно
     */
    bool addVertex(const GVertex& iVert);

    /**
     * @brief updateVertex  Заменить свойства вершины на другие
     * @param iVert         Вершина с новыми свойствами. Обязательно должен быть задан ID
     * @return              false если вершина не найдена
     */
    bool updateVertex(const GVertex& iVert);

    /**
     * @brief getVertex Получить вершину по её ID
     * @param vertexId  ID вершины
     * @return          std::nullopt если вершины с таким ID нет
     */
    std::optional<GVertex> getVertex(GraphCommon::graphId_t vertexId) const;

    /**
     * @brief getAllVertices    Получить все вершины графа
     * @return                  вектор вершин графа
     */
    std::list<GVertex> getAllVertices() const;

    /**
     * @brief getVerticesCount  Получить количество вершин графа
     * @return                  Количество вершин
     */
    std::size_t getVerticesCount() const;

    /**
     * @brief removeVertex  Удалить вершину по ID
     * @param vertexId      ID вершины
     */
    void removeVertex(GraphCommon::graphId_t vertexId);

    /**
     * @brief clearVertices Удалить все вершины. Также удаляет соединения, связанные с ними
     */
    void clearVertices();


    // ============================================================== //
    // ============== Работа с соединениями вершин графа ============ //
    // ============================================================== //

    /**
     * @brief addConnection Добавить ребро графа
     * @param iCon          Структура с информацией о ребре
     * @return              false если нет одной из вершин для соединения
     */
    bool addConnection(const GConnection& iCon);

    /**
     * @brief getConnectionsFromVertex  Получить все соединения, исходящие из вершины с ID = vertexId
     * @param vertexId                  ID вершины
     * @return                          Вектор входящих соединений
     */
    std::vector<GConnection> getConnectionsFromVertex(GraphCommon::graphId_t vertexId) const;

    /**
     * @brief getConnection Получить соединение по ID вершин, от и к которой оно идёт
     * @param vertexFromId  Вершина от которой исходит соединение
     * @param vertexToId    Вершина к которой исходит соединение
     * @return              std::nullopt если такого соединения нет
     */
    std::optional<GConnection> getConnection(GraphCommon::graphId_t vertexFromId, GraphCommon::graphId_t vertexToId) const;

    /**
     * @brief getAllConnections Получить все рёбра в графе
     * @return вектор рёбер графа
     */
    std::vector<GConnection> getAllConnections() const;

    /**
     * @brief getConnectionsCount   Получить количество соединений графа
     * @return                      Количество соединений
     */
    std::size_t getConnectionsCount() const;

    /**
     * @brief removeConnection  Удалить ребро
     * @param conFrom           Вершина от которой исходит ребро
     * @param conTo             Вершина к которой исходит ребро
     */
    void removeConnection(GraphCommon::graphId_t conFrom, GraphCommon::graphId_t conTo);

    /**
     * @brief removeConnections Удалить все рёбра, соединённые с этой вершиной
     * @param conFrom           Вершина, от которой исходит ребро
     */
    void removeConnections(GraphCommon::graphId_t conFrom);

    /**
     * @brief clearConnections  Полное удаление всех рёбер в графе
     */
    void clearConnections();

private:
    // Через STL векторы для удобства работы с алгоритмами
    std::list<GVertex>  m_vertices;                                     //! Вершины графа
    std::multimap<GraphCommon::graphId_t, GConnection>  m_connections;  //! Рёбра графа. Ключ -- ID из которого исходит ребро
};

}

#endif // GRAPHOBJECT_H
