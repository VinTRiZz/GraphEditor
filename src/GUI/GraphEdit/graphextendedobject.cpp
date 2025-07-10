#include "graphextendedobject.h"

namespace Graph
{

GraphExtendedObject::GraphExtendedObject(QObject *parent) :
    QObject(parent)
{

}

GraphExtendedObject::~GraphExtendedObject()
{

}

uint GraphExtendedObject::addVertex(const GVertex &iVert)
{
    auto id = GraphObject::addVertex(iVert);
    emit vertexAdded(id);
    return id;
}

bool GraphExtendedObject::updateVertex(const GVertex &iVert)
{
    auto res = GraphObject::updateVertex(iVert);
    if (res) {
        emit vertexUpdate(iVert.id);
    }
    return res;
}

void GraphExtendedObject::removeVertex(uint vertexId)
{
    GraphObject::removeVertex(vertexId);
    emit vertexRemoved(vertexId);
}

bool GraphExtendedObject::addConnection(const GConnection &iCon)
{
    auto res = GraphObject::addConnection(iCon);
    if (res) {
        emit connectionAdded(iCon.idFrom, iCon.idTo);
    }
    return res;
}

void GraphExtendedObject::removeConnection(uint conFrom, uint conTo)
{
    GraphObject::removeConnection(conFrom, conTo);
    emit connectionRemoved(conFrom, conTo);
}

void GraphExtendedObject::setName(const QString &iName)
{
    GraphObject::setName(iName);
    emit graphPropertyChanged();
}

void GraphExtendedObject::setDescription(const QString &iDescr)
{
    GraphObject::setDescription(iDescr);
    emit graphPropertyChanged();
}

void GraphExtendedObject::setCreateTime(const QDateTime &iDateTime)
{
    GraphObject::setCreateTime(iDateTime);
    emit graphPropertyChanged();
}

void GraphExtendedObject::setEditTime(const QDateTime &iDateTime)
{
    GraphObject::setEditTime(iDateTime);
    emit graphPropertyChanged();
}

void GraphExtendedObject::setCustomValue(const QString &key, const QVariant &value)
{
    GraphObject::setCustomValue(key, value);
    emit graphPropertyChanged();
}

void GraphExtendedObject::removeCustomValue(const QString &key)
{
    GraphObject::removeCustomValue(key);
    emit graphPropertyChanged();
}

}
