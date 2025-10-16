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

class VertexItemBase;

class VertexConnectionLine : public ObjectViewItems::ItemBase, public GraphSceneItem {
public:
    explicit VertexConnectionLine(QGraphicsItem* parent = nullptr);
    ~VertexConnectionLine();

    // GraphSceneItem interface
    const QGraphicsItem* getMainItem() const override { return this; }

    GConnection toConnection() const;
    void fromConnection(const GConnection& con);

    void setVertexFrom(VertexItemBase* pVertexFrom);
    VertexItemBase* getVertexFrom() const;

    void setVertexTo(VertexItemBase* pVertexTo);
    VertexItemBase* getVertexTo() const;

    void resetPositions();

    ObjectViewItems::LineItem* getLineItem() const;

private:
    VertexItemBase* m_fromVertex{nullptr};
    VertexItemBase* m_toVertex{nullptr};

    ObjectViewItems::LineItem* m_connectionLine {nullptr};
};

}  // namespace ObjectViewItems

