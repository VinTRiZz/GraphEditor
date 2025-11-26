#pragma once

#include "simplevertexitem.hpp"

namespace Graph {

class VertexShapeProvider
{
public:
    static QAbstractGraphicsShapeItem* createShape(SimpleVertexShapeType svst, VertexSizeType vst);
    static void scaleShape(QAbstractGraphicsShapeItem* pShape, VertexSizeType vst);

private:
    static QGraphicsRectItem*      createRect(VertexSizeType vst);
    static QGraphicsRectItem*      createSquare(VertexSizeType vst);
    static QGraphicsEllipseItem*   createEllipse(VertexSizeType vst);
    static QGraphicsEllipseItem*   createCircle(VertexSizeType vst);
    static QGraphicsPolygonItem*   createParallelogram(VertexSizeType vst);
};

}
