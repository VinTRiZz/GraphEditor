#pragma once

#include <QGraphicsItem>
#include <QPen>
#include <set>

#include "vertexitem.hpp"

#include <GraphObject/Object.h>

namespace Graph {

class VertexConnectionLine;

class VertexObjectItem : public VertexItem {
public:
    explicit VertexObjectItem(QGraphicsItem* parent = nullptr);
    ~VertexObjectItem();

    void fromVertex(const GVertex& vert) override;
    GVertex toVertex() const override;

private:
    QGraphicsPixmapItem* m_vertexImage {nullptr};
};

}  // namespace Graph
