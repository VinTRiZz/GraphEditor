#include "objectsinternalscene.h"

#include <Common/Logging.h>
#include <math.h>

#include <QGraphicsView>
#include <QPainter>

ObjectsInternalScene::ObjectsInternalScene(QObject* parent)
    : QGraphicsScene(parent) {
    m_gridPen.setCosmetic(true);
}

ObjectsInternalScene::~ObjectsInternalScene() {}

void ObjectsInternalScene::setGridEnabled(bool enabled) {
    m_isGridEnabled = enabled;
    emit gridSetEnabled(enabled);
}

bool ObjectsInternalScene::getIsGridEnabled() const {
    return m_isGridEnabled;
}

void ObjectsInternalScene::setGridSize(int sizePx) {
    if (sizePx < 2 || sizePx > 10e6) {
        throw std::invalid_argument(
            "Invalid size of grid (less than 2 or more than 10e6 px)");
    }
    m_baseGridSize = sizePx;
    emit gridSizeChanged(sizePx);
}

int ObjectsInternalScene::getGridSize() const {
    return m_baseGridSize;
}

void ObjectsInternalScene::setGridPen(const QPen &gPen)
{
    m_gridPen = gPen;
}

void ObjectsInternalScene::resetGridPen()
{
    m_gridPen = {QColor(15, 90, 180, 100), 1};
}

void ObjectsInternalScene::drawForeground(QPainter* painter,
                                          const QRectF& rect) {
    QGraphicsScene::drawForeground(painter, rect);
    drawGrid(painter, rect);
}

void ObjectsInternalScene::drawGrid(QPainter* painter, const QRectF& rect) {
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
