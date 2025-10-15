#pragma once

#include <Components/CustomQt/ObjectScene/PictureObjectItem.h>

#include <QGraphicsItem>
#include <QPen>
#include <set>

#include "graphsceneitem.hpp"

#include <GraphObject/Object.h>

namespace Graph {

class VertexConnectionLine;

class VertexObject : public ObjectViewItems::PictureObjectItem, public GraphSceneItem {
public:
    explicit VertexObject(QGraphicsItem* parent = nullptr);
    ~VertexObject();

    // GraphSceneItem interface
    const QGraphicsItem* getMainItem() const override { return this; }

    void fromVertex(const GVertex& vert);
    GVertex toVertex() const;

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

}  // namespace Graph
