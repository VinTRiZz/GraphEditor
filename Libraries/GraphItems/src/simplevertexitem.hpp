#pragma once

#include <QGraphicsItem>
#include <QPen>
#include <set>

#include "vertexitem.hpp"

#include <GraphObject/Object.h>

#include <Components/CustomQt/ObjectView/ObjectItems.h>

namespace Graph {

enum SimpleVertexShapeType : int {
    SVST_Rect = 0,
    SVST_Square,
    SVST_Ellipse,
    SVST_Circle,
    SVST_Parallelogram,
};

class VertexConnectionLine;

class SimpleVertexItem : public VertexItem {
public:
    explicit SimpleVertexItem(QGraphicsItem* parent = nullptr);
    ~SimpleVertexItem();

    void fromVertex(const GVertex& vert) override;
    GVertex toVertex() const override;

    void setShapeType(SimpleVertexShapeType vst);
    SimpleVertexShapeType getShapeType() const;

private:
    SimpleVertexShapeType m_shapeType {SimpleVertexShapeType::SVST_Ellipse};

    QAbstractGraphicsShapeItem* m_vertexShapeItem {nullptr};

    void initShapeItem();

protected:
    void processSizeTypeChange(const QRectF& newSize) override;
};

}  // namespace Graph
