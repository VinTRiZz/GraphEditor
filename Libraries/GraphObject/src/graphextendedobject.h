#ifndef GRAPHEXTENDEDOBJECT_H
#define GRAPHEXTENDEDOBJECT_H

#include "graphobject.h"

#include <QObject>

namespace Graph
{

/**
 * @brief The GraphExtendedObject class Обёртка над GraphObject с эмитом сигналов
 */
class GraphExtendedObject : public QObject, public GraphObject
{
    Q_OBJECT
public:
    explicit GraphExtendedObject(QObject* parent = nullptr);
    ~GraphExtendedObject();

    GraphExtendedObject& operator =(const GraphObject& obj);

    // Все методы ниже аналогичны методам GraphObject, но они сигналят
    GraphCommon::graphId_t addVertex(const GVertex& iVert);
    bool updateVertex(const GVertex& iVert);
    void removeVertex(GraphCommon::graphId_t vertexId);

    // Все методы ниже аналогичны методам GraphObject, но они сигналят
    bool addConnection(const GConnection& iCon);
    void removeConnection(GraphCommon::graphId_t conFrom, GraphCommon::graphId_t conTo);

    // Все методы ниже аналогичны методам GraphObject, но они сигналят
    void setName(const QString& iName);
    void setDescription(const QString& iDescr);
    void setCreateTime(const QDateTime& iDateTime);
    void setEditTime(const QDateTime& iDateTime);

    // Все методы ниже аналогичны методам GraphObject, но они сигналят
    void setCustomValue(const QString& key, const QVariant& value);
    void removeCustomValue(const QString& key);

signals:
    void graphPropertyChanged();
    void graphCustomPropertyChanged();

    void vertexAdded(GraphCommon::graphId_t vertexId);
    void vertexUpdate(GraphCommon::graphId_t vertexId);
    void vertexRemoved(GraphCommon::graphId_t vertexId);

    void connectionAdded(GraphCommon::graphId_t vertexFrom, GraphCommon::graphId_t vertexTo);
    void connectionRemoved(GraphCommon::graphId_t vertexFrom, GraphCommon::graphId_t vertexTo);
};

}

#endif // GRAPHEXTENDEDOBJECT_H
