#pragma once

#include <GraphObject/Object.h>
#include <Components/CustomQt/ObjectScene/ItemBase.h>
#include <Components/CustomQt/ObjectScene/LabelItem.h>

#include "connectionlineitem.h"
#include "graphsceneitem.hpp"

#include <set>

namespace Graph {

class VertexItemBase : public ObjectViewItems::ItemBase, public GraphSceneItem
{
public:
    explicit VertexItemBase(QGraphicsItem* parent = nullptr);

    void setDisplayName(const QString& iText);

    virtual void fromVertex(const GVertex& vert) = 0;
    virtual GVertex toVertex() const = 0;

    void subscribeAsConnectionFrom(VertexConnectionLine* pLine);
    void unsubscribeConnectionFrom(VertexConnectionLine* pLine);

    void subscribeAsConnectionTo(VertexConnectionLine* pLine);
    void unsubscribeConnectionTo(VertexConnectionLine* pLine);

    bool isLineSubscribed(VertexConnectionLine* pLine);
    void updateConnectionLines();

    QPainterPath shape() const override;

private:
    ObjectViewItems::LabelItem* m_nameItem{nullptr};

protected:
    std::set<VertexConnectionLine*> m_connectionsFromThis;
    std::set<VertexConnectionLine*> m_connectionsToThis;


    QVariant itemChange(GraphicsItemChange change,
                                      const QVariant& value) override;

};

}
