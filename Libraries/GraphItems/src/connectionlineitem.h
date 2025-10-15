#pragma once

#include <Components/CustomQt/ObjectScene/ElegantArrowLine.h>

#include <QBrush>
#include <QGraphicsItem>
#include <QGraphicsLineItem>
#include <QGraphicsPolygonItem>
#include <QPen>

namespace ObjectViewItems {

class VertexObject;

class VertexConnectionLine : public ElegantArrowLine {
public:
    explicit VertexConnectionLine(QGraphicsItem* parent = nullptr);
    ~VertexConnectionLine();

    void setVertexFrom(VertexObject* pVertexFrom);
    VertexObject* getVertexFrom() const;

    void setVertexTo(VertexObject* pVertexTo);
    VertexObject* getVertexTo() const;

    void resetPositions();

private:
    VertexObject* m_fromVertex{nullptr};
    VertexObject* m_toVertex{nullptr};
};

}  // namespace ObjectViewItems

