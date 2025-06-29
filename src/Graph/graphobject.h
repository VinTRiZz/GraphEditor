#ifndef GRAPHOBJECT_H
#define GRAPHOBJECT_H

#include <QGraphicsItem>

#include <QJsonObject>

#include <math.h>

namespace Graph
{

const unsigned GRAPH_MAX_SHORTNAME_SIZE { 10 };


/**
 * @brief The GVertex class Вершина графа
 */
struct GVertex
{
    uint        id                  {0};                //! ID вершины
    double      posX                {0};                //! При наличии, положение на графе. Записывается после первой отрисовки
    double      posY                {0};                //! При наличии, положение на графе. Записывается после первой отрисовки

    // Основные поля
    QString     shortName           {};                 //! Краткое имя врешины (отображаемое)
    QString     name                {};                 //! Полное имя вершины
    QString     description         {};                 //! Описание вершины
    QJsonObject customProperties    {};                 //! JSON с пользовательскими свойствами

    // Отрисовка
    QColor      borderColor         {Qt::black};        //! Цвет границы
    QColor      backgroundColor     {Qt::transparent};  //! Цвет фона
    QPixmap     pxmap               {};                 //! Изображение, которое будет отображаться вместо вершины

    /**
     * @brief isShortnameValid Проверка корректности длины краткого наименования вершины
     * @return false если длина больше разрешённой
     */
    bool isShortnameValid() const {
        return static_cast<unsigned>(shortName.size()) < GRAPH_MAX_SHORTNAME_SIZE;
    }

    /**
     * @brief isValid Проверка на корректность данных структуры
     * @return false если объект не может использоваться в графе
     */
    bool isValid() const {
        return isShortnameValid() && (id != 0) && ((borderColor.isValid() && backgroundColor.isValid()) || !pxmap.isNull());
    }

    /**
     * @brief operator ==   Оператор сравнения для тестирования и сравнения между собой графов
     * @param oVert_        Другая вершина
     * @return              true если вершина совпадает с этой
     */
    bool operator ==(const GVertex& oVert_) const {
        return (
                    (id == oVert_.id) &&
                    (posX == oVert_.posX) &&
                    (posY == oVert_.posY) &&
                    (shortName == oVert_.shortName) &&
                    (name == oVert_.name) &&
                    (description == oVert_.description) &&
                    (customProperties == oVert_.customProperties) &&
                    (borderColor == oVert_.borderColor) &&
                    (backgroundColor == oVert_.backgroundColor) &&
                    (pxmap.toImage() == oVert_.pxmap.toImage())
        );
    }

    /**
     * @brief operator !=   Аналог оператора сравнения, является его инверсией
     * @param oVert_        Другая вершина
     * @return              true если вершина НЕ совпадает с этой
     */
    bool operator !=(const GVertex& oVert_) const {
        return !(*this == oVert_);
    }
};


/**
 * @brief The GConnection class Соединение между вершинами графа
 */
struct GConnection
{
    uint    idFrom              {0};            //! ID из которого исходит ребро
    uint    idTo                {0};            //! ID в которое входит ребро
    double  connectionWeight    {0};            //! Вес ребра
    QString name                {};             //! Название ребра
    QColor  lineColor           {Qt::black};    //! Цвет ребра
    bool    isDirected          {false};        //! Флаг наличия направления у ребра

    /**
     * @brief isValid Проверка на корректность данных структуры
     * @return false если объект не может быть использован в графе
     */
    bool isValid() const {
        return (idFrom != idTo) && (idFrom != 0) && (idTo != 0) && lineColor.isValid();
    }

    bool operator ==(const GConnection& oCon_) const {
        return (
                    (idFrom == oCon_.idFrom) &&
                    (idTo == oCon_.idTo) &&
                    (fabs(oCon_.connectionWeight - connectionWeight) < 1e-6) &&
                    (name == oCon_.name) &&
                    (lineColor == oCon_.lineColor) &&
                    (isDirected == oCon_.isDirected)
        );
    }

    bool operator !=(const GConnection& oCon_) const {
        return !(*this == oCon_);
    }
};


/**
 * @brief The GraphObject class Объект графа, заключающий в себе свойства графа и его наполнение
 */
class GraphObject
{
public:
    GraphObject();

    bool operator ==(const GraphObject& gObj_) const;

    bool operator !=(const GraphObject& gObj_) const;

    /**
     * @brief setIdGenerator Установить генератор ID для вершин
     * @param fGen функтор генератора ID
     */
    void setIdGenerator(const std::function<uint()>& fGen);

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
     * @brief getConnectionsToVertex Получить все соединения, входящие в вершину с ID = vertexId
     * @param vertexId ID вершины
     * @return Вектор входящих соединений
     */
    std::vector<GConnection> getConnectionsToVertex(uint vertexId) const;

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
    std::function<uint()>   m_idGenerator; //! Генератор ID для вершин

    // Через STL векторы для удобства работы с алгоритмами
    std::vector<GVertex>                m_vertices;     //! Вершины графа
    std::multimap<uint, GConnection>    m_connections;  //! Рёбра графа. Ключ -- целевой ID вершины (в которую "входит" стрелка связи)

    QString     m_name                      {"Untitled graph"};
    QString     m_description               {"Empty description of a graph"};
    QDateTime   m_createTime;
    QDateTime   m_editTime;

    std::map<QString, QVariant> m_customDataValues; //! Пользовательские данные
};

}

#endif // GRAPHOBJECT_H
