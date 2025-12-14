#pragma once

#include <optional>
#include <unordered_set>

#include <QObject>

#include "gobject.h"

namespace Graph {

class GraphObjectManager;
using GraphObjectManagerPtr = std::shared_ptr<GraphObjectManager>;

/**
 * @brief The GraphMetaInformation class Метаинформация о графе (название, дата создания и т.д.)
 */
class GraphMetaInformation : public QObject {
    Q_OBJECT

    QString m_name;
    QString m_description;
    QDateTime m_createTime;
    QDateTime m_editTime;
    QString m_savepath;

public:
    using QObject::QObject;

    bool operator==(const GraphMetaInformation& gObj_) const;
    bool operator!=(const GraphMetaInformation& gObj_) const;

    QString getName() const;
    void setName(const QString &newName);

    QString getDescription() const;
    void setDescription(const QString &newDescription);

    QDateTime getCreateTime() const;
    void setCreateTime(const QDateTime &newCreateTime);

    QDateTime getEditTime() const;
    void setEditTime(const QDateTime &newEditTime);

    QString getSavepath() const;
    void setSavepath(const QString &newSavepath);

signals:
    void dataChanged();
};

/**
 * @brief The GraphObject class Объект графа, заключающий в себе свойства графа
 * и его наполнение
 */
class GraphObject : public QObject {
    Q_OBJECT
public:
    explicit GraphObject(QObject* parent = nullptr);

    /**
     * @brief clearGraphData Утилитарный метод для полной очистки объекта без пересоздания
     */
    void clearGraphData();

    bool operator==(const GraphObject& gObj_) const;
    bool operator!=(const GraphObject& gObj_) const;

    // ============================================================== //
    // ================= Работа с метаинформацией =================== //
    // ============================================================== //
    GraphMetaInformation* getMetaInfo() const;


    // ============================================================== //
    // ================= Работа с вершинами графа =================== //
    // ============================================================== //
    void addVertex(const GObject& iVert);
    bool updateVertex(const GObject& iVert);
    const std::unordered_set<GObject>& getAllVertices() const;
    std::size_t getVerticesCount() const;
    void removeVertex(graphId_t vertexId);
    void clearVertices();

private:
    GraphMetaInformation* m_metaInfo {nullptr};
    std::unordered_set<GObject> m_vertices;  //! Вершины графа
};

/**
 * @brief The GraphObjectManager class Класс для передачи в дочерние. Гарантирует, что объект графа будет валидный
 */
class GraphObjectManager {
public:
    void setObject(GraphObject* pObject);
    GraphObject* getObject();

    static GraphObjectManagerPtr createGraphInstance();

private:
    GraphObject* m_graphObject {nullptr};
    QMetaObject::Connection m_destroyedConnection; // Для отключения
};

/**
 * @brief The GraphObjectUser class Декоратор для упрощения кода
 */
class GraphObjectUser {
public:
    void setGraph(const Graph::GraphObjectManagerPtr& pGraph);
    bool isGraphSet() const;
    Graph::GraphObjectManagerPtr getGraph() const;

protected:
    virtual void processGraphChange() { }

private:
    Graph::GraphObjectManagerPtr m_pGraph;
};

}  // namespace Graph
