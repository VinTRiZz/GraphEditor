#pragma once

#include <Components/CustomQt/ObjectScene/ElegantArrowLine.h>
#include <Components/CustomQt/ObjectScene/ArrowLine.h>

#include <QBrush>
#include <QGraphicsItem>
#include <QGraphicsLineItem>
#include <QGraphicsPolygonItem>
#include <QPen>

#include "graphsceneitem.hpp"
#include <GraphObject/Object.h>

namespace Graph {

class VertexObject;

class VertexConnectionLine : public ObjectViewItems::ItemBase, public GraphSceneItem {
public:
    explicit VertexConnectionLine(QGraphicsItem* parent = nullptr);
    ~VertexConnectionLine();

    // GraphSceneItem interface
    const QGraphicsItem* getMainItem() const override { return this; }

    GConnection toConnection() const;
    void fromConnection(const GConnection& con);

    void setVertexFrom(VertexObject* pVertexFrom);
    VertexObject* getVertexFrom() const;

    void setVertexTo(VertexObject* pVertexTo);
    VertexObject* getVertexTo() const;

    void resetPositions();

    ObjectViewItems::LineItem* getLineItem() const;

private:
    VertexObject* m_fromVertex{nullptr};
    VertexObject* m_toVertex{nullptr};

    ObjectViewItems::LineItem* m_connectionLine {nullptr};
};

}  // namespace ObjectViewItems

