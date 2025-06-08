#ifndef GRAPHOBJECT_H
#define GRAPHOBJECT_H

#include <QGraphicsItem>

#include <QJsonObject>

namespace Graph
{

/**
 * @brief The GVertex class Вершина графа
 */
struct GVertex
{
    uint        id                  {0};
    QPixmap     pxmap               {};
    QString     shortName           {};
    QString     name                {};
    QString     description         {};
    QJsonObject customProperties    {};
    QColor      borderColor         {Qt::black};
    QColor      backgroundColor     {Qt::transparent};
};

/**
 * @brief The GConnection class Соединение между вершинами графа
 */
struct GConnection
{
    uint    idFrom              {0};
    uint    idTo                {0};
    double  connectionWeight    {0};
    QString name                {};
    QColor  lineColor           {Qt::black};
    bool    isDirected          {false};
};


/**
 * @brief The GraphObject class Объект графа, заключающий в себе свойства графа и его наполнение
 */
class GraphObject
{
public:
    GraphObject();

    void setIdGenerator(const std::function<uint()>& fGen);

    /**
     * @brief toItem Конвертировать в QGraphicsItem
     * @return QGraphicsItem указатель или nullptr, в случае ошибки
     */
    QGraphicsItem* toItem() const;

    // ============================================================== //
    // ================= Работа с вершинами графа =================== //
    // ============================================================== //

    /**
     * @brief addVertex Добавить вершину в граф
     * @param iVert Структура с описанием свойств вершины
     * @return ID добавленной вершины
     */
    uint addVertex(const GVertex& iVert);

    /**
     * @brief updateVertex Заменить свойства вершины на другие
     * @param iVert Вершина с новыми свойствами. Обязательно должен быть задан ID
     * @return false если вершина не найдена
     */
    bool updateVertex(const GVertex& iVert);

    /**
     * @brief getAllVertices Получить все вершины графа
     * @return вектор вершин графа
     */
    std::vector<GVertex> getAllVertices() const;

    /**
     * @brief removeVertex Удалить вершину по ID
     * @param vertexId ID вершины
     */
    void removeVertex(uint vertexId);


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
     * @brief getAllConnections Получить все рёбра в графе
     * @return вектор рёбер графа
     */
    std::vector<GConnection> getAllConnections() const;

    /**
     * @brief removeConnection Удалить ребро
     * @param conFrom Вершина от которой исходит ребро
     * @param conTo Вершина к которой исходит ребро
     */
    void removeConnection(uint conFrom, uint conTo);


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

private:
    std::function<uint()>   m_idGenerator; //! Генератор ID для вершин

    // Через STL векторы для удобства работы с алгоритмами
    std::vector<GVertex>        m_vertices;     //! Вершины графа
    std::vector<GConnection>    m_connections;  //! Рёбра графа

    QString     m_name                      {"Untitled graph"};
    QString     m_description               {"Empty description of a graph"};
    QDateTime   m_createTime;
    QDateTime   m_editTime;
};

}

#endif // GRAPHOBJECT_H
