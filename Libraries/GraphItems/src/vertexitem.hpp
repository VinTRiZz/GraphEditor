#pragma once

#include <GraphObject/Object.h>
#include <Components/CustomQt/ObjectView/ObjectItems.h>

#include "connectionlineitem.hpp"

#include <set>

namespace Graph {

class VertexItem : public ObjectItems::BasicItem
{
public:
    explicit VertexItem(QGraphicsItem* parent = nullptr);

    virtual void fromVertex(const GVertex& vert) = 0;
    virtual GVertex toVertex() const = 0;

    void subscribeAsConnectionFrom(VertexConnectionLine* pLine);
    void unsubscribeConnectionFrom(VertexConnectionLine* pLine);

    void subscribeAsConnectionTo(VertexConnectionLine* pLine);
    void unsubscribeConnectionTo(VertexConnectionLine* pLine);

    bool isLineSubscribed(VertexConnectionLine* pLine);

private:
    ObjectItems::TextLabel* m_nameItem{nullptr};

private slots:
    void updateConnectionLines();

protected:
    std::set<VertexConnectionLine*> m_connectionsFromThis;
    std::set<VertexConnectionLine*> m_connectionsToThis;

    ObjectItems::TextLabel* getLabel() const;
};

}
