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

    // GraphSceneItem interface
    const QGraphicsItem* getMainItem() const override { return this; }

    void fromVertex(const GVertex& vert) override;
    GVertex toVertex() const override;

    void setImageByHash(const QString& imageHash);

    void setBorderColor(const QColor& penColor) override;
    void setBackgroundColor(const QColor& penColor) override;

    QPainterPath shape() const override;

private:
    ObjectViewItems::PictureObjectItem* m_vertexImage {nullptr};
};

}  // namespace Graph
