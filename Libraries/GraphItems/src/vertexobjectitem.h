#ifndef VERTEXOBJECT_H
#define VERTEXOBJECT_H

#include <Components/CustomQt/ObjectScene/PictureObjectItem.h>

#include <QGraphicsItem>
#include <QPen>
#include <set>

namespace ObjectViewItems {

class VertexConnectionLine;

class VertexObject : public PictureObjectItem {
public:
    explicit VertexObject(QGraphicsItem* parent = nullptr);
    ~VertexObject();

    void setImageByHash(const QString& imageHash);

    void setBorderColor(const QColor& penColor) override;
    void setBackgroundColor(const QColor& penColor) override;

    bool isLineSubscribed(VertexConnectionLine* pLine);

    void subscribeAsConnectionFrom(VertexConnectionLine* pLine);
    void unsubscribeConnectionFrom(VertexConnectionLine* pLine);

    void subscribeAsConnectionTo(VertexConnectionLine* pLine);
    void unsubscribeConnectionTo(VertexConnectionLine* pLine);

    void updateConnectionLines();

private:
    std::set<VertexConnectionLine*> m_connectionsFromThis;
    std::set<VertexConnectionLine*> m_connectionsToThis;

protected:
    QVariant itemChange(GraphicsItemChange change,
                        const QVariant& value) override;
};

}  // namespace ObjectViewItems

#endif  // VERTEXOBJECT_H
