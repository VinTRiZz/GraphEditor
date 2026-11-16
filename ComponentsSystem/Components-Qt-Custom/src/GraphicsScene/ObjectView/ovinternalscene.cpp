#include "ovinternalscene.hpp"

#include <QPainter>

#include <math.h>

#include "ovconstants.hpp"

namespace OVLayers {

OVInternalScene::OVInternalScene(QObject *parent)
    : QGraphicsScene{parent}
{
    m_gridPen.setCosmetic(true);
    resetGridPen();
    setGridSize(50);
}

OVInternalScene::~OVInternalScene()
{

}

void OVInternalScene::setGridEnabled(bool enabled) {
    m_isGridEnabled = enabled;
    update();
    emit gridEnabled(enabled);
}

bool OVInternalScene::getIsGridEnabled() const {
    return m_isGridEnabled;
}

void OVInternalScene::setGridSize(int sizePx) {
    if (sizePx < 2 || sizePx > 10e6) {
        throw std::invalid_argument(
            "Invalid size of grid (less than 2 or more than 10e6 px)");
    }
    m_baseGridSize = sizePx;
    emit gridSizeChanged(sizePx);
}

int OVInternalScene::getGridSize() const {
    return m_baseGridSize;
}

void OVInternalScene::setGridPen(const QPen& gPen) {
    m_gridPen = gPen;
    update(sceneRect());
}

void OVInternalScene::resetGridPen() {
    m_gridPen = {DEFAULT_GRIDCOLOR, 2};
    update(sceneRect());
}

void OVInternalScene::drawForeground(QPainter* painter,
                                          const QRectF& rect) {
    QGraphicsScene::drawForeground(painter, rect);
    drawGrid(painter, rect);
}

void OVInternalScene::drawGrid(QPainter* painter, const QRectF& rect) {
    if (!m_isGridEnabled)
        return;

    // Настройка пера для сетки
    painter->setPen(m_gridPen);

    // Вычисляем первую линию
    qreal left = std::floor(rect.left() / m_baseGridSize) * m_baseGridSize;
    qreal top = std::floor(rect.top() / m_baseGridSize) * m_baseGridSize;

    // Рисуем вертикальные линии
    for (qreal x = left; x <= rect.right(); x += m_baseGridSize) {
        painter->drawLine(QLineF(x, rect.top(), x, rect.bottom()));
    }

    // Рисуем горизонтальные линии
    for (qreal y = top; y <= rect.bottom(); y += m_baseGridSize) {
        painter->drawLine(QLineF(rect.left(), y, rect.right(), y));
    }
}


}
