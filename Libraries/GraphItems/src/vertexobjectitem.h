#pragma once

#include <QGraphicsItem>
#include <QPen>
#include <set>

#include "vertexitembase.hpp"

#include <GraphObject/Object.h>
#include <Components/CustomQt/ObjectScene/PictureObjectItem.h>

namespace Graph {

class VertexConnectionLine;

class VertexObjectItem : public VertexItemBase {
public:
    explicit VertexObjectItem(QGraphicsItem* parent = nullptr);
    ~VertexObjectItem();

    void fromVertex(const GVertex& vert) override;
    GVertex toVertex() const override;

    void setBorderColor(const QColor& penColor) override;
    void setBackgroundColor(const QColor& penColor) override;

    QPainterPath shape() const override;

private:
    ObjectViewItems::PictureObjectItem* m_vertexImage {nullptr};
};

}  // namespace Graph
