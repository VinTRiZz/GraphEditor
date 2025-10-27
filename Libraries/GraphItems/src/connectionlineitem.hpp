#pragma once

#include <Components/CustomQt/ObjectView/ObjectItems.h>

#include <QBrush>
#include <QGraphicsItem>
#include <QGraphicsLineItem>
#include <QGraphicsPolygonItem>
#include <QPen>

#include <GraphObject/Object.h>

namespace Graph {

class VertexItem;

class VertexConnectionLine : public ObjectItems::BasicItem {
public:
    explicit VertexConnectionLine(QGraphicsItem* parent = nullptr);
    ~VertexConnectionLine();

    GConnection toConnection() const;
    void fromConnection(const GConnection& con);

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

