#ifndef GRAPHOBJECT_H
#define GRAPHOBJECT_H

#include <QGraphicsItem>

#include <QJsonObject>

#include <math.h>

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
     * @brief updateVertex Заменить свойства вершины на другие
     * @param iVert Вершина с новыми свойствами. Обязательно должен быть задан ID
     * @return false если вершина не найдена
     */
    bool updateVertex(const GVertex& iVert);

    /**
     * @brief getVertex Получить вершину по её ID
     * @param vertexId  ID вершины
     * @return          std::nullopt если вершины с таким ID нет
     */
    std::optional<GVertex> getVertex(GraphCommon::graphId_t vertexId) const;

    /**
     * @brief getAllVertices Получить все вершины графа
     * @return вектор вершин графа
     */
    std::list<GVertex> getAllVertices() const;

    /**
     * @brief getVerticesCount  Получить количество вершин графа
     * @return                  Количество вершин
     */
    std::size_t getVerticesCount() const;

    /**
     * @brief removeVertex Удалить вершину по ID
     * @param vertexId ID вершины
     */
    void removeVertex(GraphCommon::graphId_t vertexId);


    // ============================================================== //
    // ============== Работа с соединениями вершин графа ============ //
    // ============================================================== //

    /**
     * @brief addConnection Добавить ребро графа
     * @param iCon Структура с информацией о ребре
     * @return false если нет одной из вершин для соединения
     */
    bool addConnection(const GConnection& iCon);

    /**
     * @brief getConnectionsToVertex Получить все соединения, входящие в вершину с ID = vertexId
     * @param vertexId ID вершины
     * @return Вектор входящих соединений
     */
    std::vector<GConnection> getConnectionsToVertex(GraphCommon::graphId_t vertexId) const;

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
     * @brief removeConnection Удалить ребро
     * @param conFrom Вершина от которой исходит ребро
     * @param conTo Вершина к которой исходит ребро
     */
    void removeConnection(GraphCommon::graphId_t conFrom, GraphCommon::graphId_t conTo);


    // ============================================================== //
    // ================= Стандартные свойства графа ================= //
    // ============================================================== //

    /**
     * @brief setName Изменить название графа
     * @param iName Новое название
     */
    void        setName(const QString& iName);

    /**
     * @brief getName Получить название графа
     * @return Строка с названием графа
     */
    QString     getName() const;

    /**
     * @brief setDescription Установить описание графа
     * @param iDescr Описание графа
     */
    void        setDescription(const QString& iDescr);

    /**
     * @brief getDescription Получить описание графа
     * @return Описание графа
     */
    QString     getDescription() const;

    /**
     * @brief setCreateTime Установить время создания графа
     * @param iDateTime Время создания графа
     */
    void        setCreateTime(const QDateTime& iDateTime);

    /**
     * @brief getCreateTime Получить время создания графа
     * @return Время создания графа
     */
    QDateTime   getCreateTime() const;

    /**
     * @brief setEditTime Установить время изменения графа
     * @param iDateTime Время изменения графа
     */
    void        setEditTime(const QDateTime& iDateTime);

    /**
     * @brief getEditTime Получить время изменения графа
     * @return Время изменения графа
     */
    QDateTime   getEditTime() const;

    /**
     * @brief setCustomValue Задание значения пользовательским данным
     * @param key Ключ (уникальный)
     * @param value Значение (любое, желательно без указателей)
     */
    void setCustomValue(const QString& key, const QVariant& value);

    /**
     * @brief removeCustomValue Удалить пользовательские данные по ключу
     * @param key Ключ (уникальный)
     */
    void removeCustomValue(const QString& key);

    /**
     * @brief getCustomValue Получение пользовательского значения
     * @param key Ключ (уникаьный)
     * @return Значение или NULL QVariant при отсутствии такого ключа
     */
    QVariant getCustomValue(const QString& key) const;

    /**
     * @brief getCustomValueMap Получение всех пользовательских данных
     * @return Словарь ключ-значение пользовательских данных
     */
    std::map<QString, QVariant> getCustomValueMap() const;

private:
    std::function<GraphCommon::graphId_t(bool)>   m_idGenerator; //! Генератор ID для вершин

    // Через STL векторы для удобства работы с алгоритмами
    std::list<GVertex>  m_vertices;     //! Вершины графа
    std::multimap<GraphCommon::graphId_t, GConnection>    m_connections;  //! Рёбра графа. Ключ -- целевой ID вершины (в которую "входит" стрелка связи)

    // Основные параметры графа
    QString     m_name;
    QString     m_description;
    QDateTime   m_createTime;
    QDateTime   m_editTime;

    std::map<QString, QVariant> m_customDataValues; //! Пользовательские данные
};

}

#endif // GRAPHOBJECT_H
