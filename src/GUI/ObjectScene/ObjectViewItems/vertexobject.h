#ifndef VERTEXOBJECT_H
#define VERTEXOBJECT_H

#include <QGraphicsItem>
#include <QPen>
#include <set>

#include "itembase.h"
#include "labelitem.h"

namespace ObjectViewItems
{

class VertexConnectionLine;

class VertexObject : public ItemBase
{
public:
    explicit VertexObject(QGraphicsItem *parent = nullptr);
    ~VertexObject();

    LabelItem* getLabel() const;

    void setImage(const QImage& img);
    void setShortName(const QString& iText) override;
    void setName(const QString& iText) override;
    void setSelectedColor(const QColor &penColor) override;

    void setNodeColor(const QColor& borderColor, const QBrush &backgroundBrush);

    QImage getImage() const;

    void setRect(const QRectF& iRect);

    QPainterPath shape() const override;

    bool isLineSubscribed(VertexConnectionLine* pLine);

    void subscribeAsConnectionFrom(VertexConnectionLine* pLine);
    void unsubscribeConnectionFrom(VertexConnectionLine* pLine);

    void subscribeAsConnectionTo(VertexConnectionLine* pLine);
    void unsubscribeConnectionTo(VertexConnectionLine* pLine);

    void updateConnectionLines();

    void setCustomProperties(const QJsonObject& props) override;
    QJsonObject getCustomProperties() const override;

private:
    QGraphicsPathItem*      m_selectedRectItem  {nullptr};
    QGraphicsPixmapItem*    m_vertexImage       {nullptr};
    QGraphicsEllipseItem*   m_vertexEllipse     {nullptr};
    LabelItem*              m_nameItem          {nullptr};

    std::set<VertexConnectionLine*> m_connectionsFromThis;
    std::set<VertexConnectionLine*> m_connectionsToThis;

    QVariant itemChange(GraphicsItemChange change, const QVariant &value) override;
};

}

#endif // VERTEXOBJECT_H
