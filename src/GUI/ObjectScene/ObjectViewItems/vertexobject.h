#ifndef VERTEXOBJECT_H
#define VERTEXOBJECT_H

#include <QGraphicsItem>
#include <QPen>
#include <set>

#include "itembase.h"

namespace ObjectViewItems
{

class VertexConnectionLine;

class VertexObject : public ItemBase
{
public:
    explicit VertexObject(QGraphicsItem *parent = nullptr);
    ~VertexObject();

    void setImage(const QImage& img);

    void setShortName(const QString& iText) override;

    void setNodeColor(const QColor& borderColor, const QBrush &backgroundBrush);
    void setTextBackgroundBrush(const QBrush& textBackground);

    QColor getBorderColor() const;
    QColor getBackgroundColor() const;
    QImage getImage() const;

    void setRect(const QRectF& iRect);

    QPainterPath shape() const override;

    void subscribeAsConnectionFrom(VertexConnectionLine* pLine);
    void unsubscribeConnectionFrom(VertexConnectionLine* pLine);

    void subscribeAsConnectionTo(VertexConnectionLine* pLine);
    void unsubscribeConnectionTo(VertexConnectionLine* pLine);

    void updateConnectionLines();

private:
    QPen   m_selectedPen;
    QLineF m_straightLine;

    QGraphicsPathItem*      m_selectedRectItem  {nullptr};
    QGraphicsPixmapItem*    m_vertexImage       {nullptr};
    QGraphicsEllipseItem*   m_vertexEllipse     {nullptr};
    QGraphicsRectItem*      m_vertexTextRect    {nullptr};
    QGraphicsTextItem*      m_vertexText        {nullptr};

    std::set<VertexConnectionLine*> m_connectionsFromThis;
    std::set<VertexConnectionLine*> m_connectionsToThis;

    QVariant itemChange(GraphicsItemChange change, const QVariant &value) override;

    void setupTextItem();
};

}

#endif // VERTEXOBJECT_H
