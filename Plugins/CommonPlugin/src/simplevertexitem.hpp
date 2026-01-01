#pragma once

#include <QGraphicsItem>
#include <QPen>
#include <set>

#include <GraphObject/GraphObject.h>

namespace Graph {

enum SimpleVertexShapeType : int {
    SVST_Rect = 0,
    SVST_Ellipse,
    SVST_Parallelogram,
    SVST_Square,
    SVST_Circle,
};

class GObjectConnectionItem;

class SimpleVertexItem : public GObjectItem {
public:
    explicit SimpleVertexItem(QGraphicsItem* parent = nullptr);
    ~SimpleVertexItem();

    void setShapeType(SimpleVertexShapeType vst);
    SimpleVertexShapeType getShapeType() const;

    virtual QJsonObject toJson() const override;
    virtual bool fromJson(const QJsonObject& arr) override;

private:
    SimpleVertexShapeType m_shapeType {SimpleVertexShapeType::SVST_Ellipse};

    QAbstractGraphicsShapeItem* m_vertexShapeItem {nullptr};

    void initShapeItem();

protected:
    void processSizeTypeChange(const QRectF& newSize) override;
};

}  // namespace Graph
