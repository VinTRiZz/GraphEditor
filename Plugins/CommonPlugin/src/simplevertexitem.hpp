#pragma once

#include <QGraphicsItem>
#include <QPen>
#include <set>

#include <GraphItems/VertexItem.h>

namespace Graph {

enum SimpleVertexShapeType : int {
    SVST_Rect = 0,
    SVST_Ellipse,
    SVST_Parallelogram,
    SVST_Square,
    SVST_Circle,
};

class VertexConnectionItem;

class SimpleVertexItem : public VertexItem {
public:
    explicit SimpleVertexItem(QGraphicsItem* parent = nullptr);
    ~SimpleVertexItem();

    void fromGObject(const GObject& vert) override;
    GObject toGObject() const override;

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
