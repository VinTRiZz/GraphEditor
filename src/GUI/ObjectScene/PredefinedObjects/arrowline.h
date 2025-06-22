#ifndef ARROWLINE_H
#define ARROWLINE_H

#include <QGraphicsLineItem>

namespace PredefinedObjects
{

class ArrowedLine : public QGraphicsLineItem
{
public:
    ArrowedLine(QGraphicsItem *parent = nullptr);

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
};

}

#endif // ARROWLINE_H
