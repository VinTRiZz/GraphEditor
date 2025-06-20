#ifndef ARROWLINE_H
#define ARROWLINE_H

#include <QGraphicsLineItem>

namespace PredefinedObjects
{

class ArrowedLine : public QGraphicsLineItem
{
public:
    ArrowedLine(QGraphicsItem *parent = nullptr);

    void setArrowSize(qreal size);
    qreal arrowSize() const;

protected:
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget = nullptr) override;

private:
    void drawArrow(QPainter *painter);

    qreal m_arrowSize; // Размер стрелки
};

}

#endif // ARROWLINE_H
