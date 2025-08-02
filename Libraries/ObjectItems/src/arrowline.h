#ifndef ARROWLINE_H
#define ARROWLINE_H

#include <QGraphicsLineItem>
#include <QPen>

#include "itembase.h"

namespace ObjectViewItems
{

class ArrowedLine : public ItemBase
{
public:
    explicit ArrowedLine(QGraphicsItem *parent = nullptr);

    void setPen(const QPen& drawPen);
    void setSelectedPen(const QPen& drawPen);

    void setLine(const QLineF& iLine);
    void setLine(const QPointF& p1, const QPointF& p2);
    QLineF getLine() const;

    /**
     * @brief setArrowSize  Задать размер стрелки
     * @param arrowSize     Размеры стрелки (стрелка квадратная)
     */
    void setArrowSize(qreal arrowSize);

    /**
     * @brief getArrowSize  Получить размер стрелки
     * @return              Размер полигона стрелки (стрелка квадратная)
     */
    qreal getArrowSize() const;

protected:
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget = nullptr) override;

private:
    void drawArrow(QPainter *painter);
    QVariant itemChange(GraphicsItemChange change, const QVariant &value) override;

    QGraphicsLineItem* m_line {nullptr};

    qreal   m_arrowSize {10}; // Размер стрелки
    QPen    m_drawPen;
    QPen    m_drawSelectedPen;
};

}

#endif // ARROWLINE_H
