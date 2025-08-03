#include "graphextendedobject.h"

namespace Graph {

GraphExtendedObject::GraphExtendedObject(QObject* parent) : QObject(parent) {}

GraphExtendedObject::~GraphExtendedObject() {}

GraphExtendedObject& GraphExtendedObject::operator=(const GraphObject& obj) {
    GraphObject::operator=(obj);
    return *this;
}

GraphCommon::graphId_t GraphExtendedObject::addVertex(const GVertex& iVert) {
    auto id = GraphObject::addVertex(iVert);
    emit vertexAdded(id);
    return id;
}

bool GraphExtendedObject::updateVertex(const GVertex& iVert) {
    auto res = GraphObject::updateVertex(iVert);
    if (res) {
        emit vertexUpdate(iVert.id);
    }
    return res;
}

void GraphExtendedObject::removeVertex(GraphCommon::graphId_t vertexId) {
    GraphObject::removeVertex(vertexId);
    emit vertexRemoved(vertexId);
}

bool GraphExtendedObject::addConnection(const GConnection& iCon) {
    auto res = GraphObject::addConnection(iCon);
    if (res) {
        emit connectionAdded(iCon.idFrom, iCon.idTo);
    }
    return res;
}

void GraphExtendedObject::removeConnection(GraphCommon::graphId_t conFrom,
                                           GraphCommon::graphId_t conTo) {
    GraphObject::removeConnection(conFrom, conTo);
    emit connectionRemoved(conFrom, conTo);
}

}  // namespace Graph
