#pragma once

#include <Components/CustomQt/ObjectScene/ElegantArrowLine.h>
#include <Components/CustomQt/ObjectScene/ArrowLine.h>

#include <QBrush>
#include <QGraphicsItem>
#include <QGraphicsLineItem>
#include <QGraphicsPolygonItem>
#include <QPen>

namespace ObjectViewItems {

class VertexObject;

class VertexConnectionLine : public ItemBase {
public:
    explicit VertexConnectionLine(QGraphicsItem* parent = nullptr);
    ~VertexConnectionLine();

    void setVertexFrom(VertexObject* pVertexFrom);
    VertexObject* getVertexFrom() const;

    void setVertexTo(VertexObject* pVertexTo);
    VertexObject* getVertexTo() const;

    void resetPositions();

    LineItem* getLineItem() const;

private:
    VertexObject* m_fromVertex{nullptr};
    VertexObject* m_toVertex{nullptr};

    LineItem* m_connectionLine {nullptr};
};

}  // namespace ObjectViewItems

