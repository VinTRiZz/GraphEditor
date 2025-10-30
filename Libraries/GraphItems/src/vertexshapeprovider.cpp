#include "vertexshapeprovider.hpp"

namespace Graph {

QAbstractGraphicsShapeItem *VertexShapeProvider::createShape(SimpleVertexShapeType svst, VertexSizeType vst)
{
    switch (svst)
    {
    case SimpleVertexShapeType::SVST_Rect:
        return createRect(vst);

    case SimpleVertexShapeType::SVST_Square:
        return createSquare(vst);

    case SimpleVertexShapeType::SVST_Ellipse:
        return createEllipse(vst);

    case SimpleVertexShapeType::SVST_Circle:
        return createCircle(vst);

    case SimpleVertexShapeType::SVST_Parallelogram:
        return createParallelogram(vst);

    }
    throw std::invalid_argument(std::string("Invalid shape to create: ") + std::to_string(svst));
    return {};
}

void VertexShapeProvider::scaleShape(QAbstractGraphicsShapeItem *pShape, VertexSizeType vst)
{
    auto resRect = toVertexBoundingRect(vst);

    auto pRect = dynamic_cast<QGraphicsRectItem*>(pShape);
    if (pRect) {
        auto sideW = pRect->rect().width();
        auto sideH = pRect->rect().height();

        auto scaleCoeff = sideW / sideH;

        auto vbrCenter = resRect.center();
        resRect.setWidth(resRect.height() * scaleCoeff);
        resRect.setHeight(resRect.width() / scaleCoeff);
        resRect.moveCenter(vbrCenter);

        pRect->setRect(resRect);
        return;
    }

    auto pEllipse = dynamic_cast<QGraphicsEllipseItem*>(pShape);
    if (pEllipse) {
        auto sideW = pEllipse->rect().width();
        auto sideH = pEllipse->rect().height();

        auto scaleCoeff = sideW / sideH;

        auto vbrCenter = resRect.center();
        resRect.setWidth(resRect.height() * scaleCoeff);
        resRect.setHeight(resRect.width() / scaleCoeff);
        resRect.moveCenter(vbrCenter);

        pEllipse->setRect(resRect);
        return;
    }

    auto pPoly = dynamic_cast<QGraphicsPolygonItem*>(pShape);
    if (pPoly) {
        auto poly = pPoly->polygon();

        auto sideW = resRect.width();
        auto sideH = resRect.height();

        auto brect = poly.boundingRect();
        auto curW = brect.width();
        auto curH = brect.width();

        QTransform scaleTransf;
        scaleTransf.scale(sideW / curW, sideH / curH);
        poly = scaleTransf.map(poly);

        auto deltaPos = resRect.center() - poly.boundingRect().center();
        poly.translate(deltaPos);

        pPoly->setPolygon(poly);
        return;
    }
}

QGraphicsRectItem *VertexShapeProvider::createRect(VertexSizeType vst)
{
    auto pRes = new QGraphicsRectItem;

    auto resRect = toVertexBoundingRect(vst);
    auto rectCenter = resRect.center();
    resRect.setWidth(0.8 * resRect.width());
    resRect.setHeight(0.6 * resRect.height());
    resRect.moveCenter(rectCenter);
    pRes->setRect(resRect);

    return pRes;
}

QGraphicsRectItem *VertexShapeProvider::createSquare(VertexSizeType vst)
{
    auto pRes = new QGraphicsRectItem;

    auto resRect = toVertexBoundingRect(vst);
    auto minSize = std::min(resRect.width(), resRect.height());
    auto rectCenter = resRect.center();
    resRect.setWidth(minSize);
    resRect.setHeight(minSize);
    resRect.moveCenter(rectCenter);
    pRes->setRect(resRect);

    return pRes;
}

QGraphicsEllipseItem *VertexShapeProvider::createEllipse(VertexSizeType vst)
{
    auto pRes = new QGraphicsEllipseItem;

    auto resRect = toVertexBoundingRect(vst);
    auto vbrCenter = resRect.center();
    resRect.setWidth(0.7 * resRect.width());
    resRect.setHeight(0.5 * resRect.height());
    resRect.moveCenter(vbrCenter);
    pRes->setRect(resRect);

    return pRes;
}

QGraphicsEllipseItem *VertexShapeProvider::createCircle(VertexSizeType vst)
{
    auto pRes = new QGraphicsEllipseItem;

    auto resRect = toVertexBoundingRect(vst);
    auto vbrCenter = resRect.center();
    auto minSize = std::min(resRect.width(), resRect.height());
    resRect.setWidth(minSize);
    resRect.setHeight(minSize);
    resRect.moveCenter(vbrCenter);
    pRes->setRect(resRect);

    return pRes;
}

QGraphicsPolygonItem *VertexShapeProvider::createParallelogram(VertexSizeType vst)
{
    auto pRes = new QGraphicsPolygonItem;

    auto resRect = toVertexBoundingRect(vst);
    auto vbrCenter = resRect.center();

    QPolygonF resPoly;
    resPoly.append(resRect.bottomLeft());

    auto nextPos = resRect.bottomRight();
    nextPos.setX(nextPos.x() - resRect.width() * 0.1);
    resPoly.append(nextPos);

    nextPos = resRect.topRight();
    resPoly.append(nextPos);

    nextPos = resRect.topLeft();
    nextPos.setX(nextPos.x() + resRect.width() * 0.1);
    resPoly.append(nextPos);

    // Перемещаем центр
    auto deltaPos = vbrCenter - resPoly.boundingRect().center();
    resPoly.translate(deltaPos);

    pRes->setPolygon(resPoly);

    return pRes;
}


}
