#ifndef GRAPHEXTENDEDOBJECT_H
#define GRAPHEXTENDEDOBJECT_H

#include <QObject>

#include "graphobject.h"

namespace Graph {

/**
 * @brief The GraphExtendedObject class Обёртка над GraphObject с эмитом
 * сигналов
 */
class GraphExtendedObject
    : public QObject
    , public GraphObject {
    Q_OBJECT
public:
    explicit GraphExtendedObject(QObject* parent = nullptr);
    ~GraphExtendedObject();

    GraphExtendedObject& operator=(const GraphObject& obj);

    // Все методы ниже аналогичны методам GraphObject, но они сигналят об
    // изменениях объекта
    GraphCommon::graphId_t addVertex(const GVertex& iVert);
    bool updateVertex(const GVertex& iVert);
    void removeVertex(GraphCommon::graphId_t vertexId);

    // Все методы ниже аналогичны методам GraphObject, но они сигналят об
    // изменениях объекта
    bool addConnection(const GConnection& iCon);
    void removeConnection(GraphCommon::graphId_t conFrom,
                          GraphCommon::graphId_t conTo);

signals:
    void vertexAdded(GraphCommon::graphId_t vertexId);
    void vertexUpdate(GraphCommon::graphId_t vertexId);
    void vertexRemoved(GraphCommon::graphId_t vertexId);

    void connectionAdded(GraphCommon::graphId_t vertexFrom,
                         GraphCommon::graphId_t vertexTo);
    void connectionRemoved(GraphCommon::graphId_t vertexFrom,
                           GraphCommon::graphId_t vertexTo);
};

}  // namespace Graph

#endif  // GRAPHEXTENDEDOBJECT_H
