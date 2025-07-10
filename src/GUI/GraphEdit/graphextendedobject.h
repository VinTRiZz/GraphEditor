#ifndef GRAPHEXTENDEDOBJECT_H
#define GRAPHEXTENDEDOBJECT_H

#include "Graph/graphobject.h"
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

    // Все методы ниже аналогичны методам GraphObject, но они сигналят
    uint addVertex(const GVertex& iVert);
    bool updateVertex(const GVertex& iVert);
    void removeVertex(uint vertexId);

    // Все методы ниже аналогичны методам GraphObject, но они сигналят
    bool addConnection(const GConnection& iCon);
    void removeConnection(uint conFrom, uint conTo);

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

    void vertexAdded(uint vertexId);
    void vertexUpdate(uint vertexId);
    void vertexRemoved(uint vertexId);

    void connectionAdded(uint vertexFrom, uint vertexTo);
    void connectionRemoved(uint vertexFrom, uint vertexTo);
};

}

#endif // GRAPHEXTENDEDOBJECT_H
