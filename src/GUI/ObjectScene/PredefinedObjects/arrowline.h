#ifndef ARROWLINE_H
#define ARROWLINE_H

#include <QGraphicsLineItem>

#include "predefinedobjectbase.h"

namespace PredefinedObjects
{

class ArrowedLine : public PredefinedObjectBase
{
public:
    explicit ArrowedLine(QGraphicsItem *parent = nullptr);

    void setLine(const QLineF& iLine);
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

    qreal m_arrowSize {10}; // Размер стрелки

    QLineF m_straightLine;
};

}

#endif // ARROWLINE_H
