#pragma once

#include <Components/CustomQt/ObjectView/ObjectItems.h>

#include <QBrush>
#include <QGraphicsItem>
#include <QGraphicsLineItem>
#include <QGraphicsPolygonItem>
#include <QPen>

#include <GraphObject/GraphObject.h>
#include <GraphObject/PluginObjectInterface.h>

namespace Graph {

class VertexItem;

class VertexConnectionItem :
        public ObjectItems::BasicItem,
        public PluginObjectInterface {
public:
    explicit VertexConnectionItem(QGraphicsItem* parent = nullptr);
    ~VertexConnectionItem();

    void setVertexFrom(VertexItem* pVertexFrom);
    VertexItem* getVertexFrom() const;

    void setVertexTo(VertexItem* pVertexTo);
    VertexItem* getVertexTo() const;

    ObjectItems::AbstractConnectionLine* getLineItem() const;

private:
    VertexItem* m_fromVertex{nullptr};
    VertexItem* m_toVertex{nullptr};

    ObjectItems::AbstractConnectionLine* m_connectionLine {nullptr};
};

}  // namespace ObjectItems

