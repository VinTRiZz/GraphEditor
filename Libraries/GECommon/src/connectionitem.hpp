#pragma once

#include "abstractitem.hpp"
#include "pointadapter.hpp"

#include <deque>

namespace GraphEditor {

/**
 * @brief The ConnectionItem class  Connection between items in graph
 */
class ConnectionItem : public AbstractItem
{
public:
    void addNode(const ItemPtr& pNode);
    void removeNode(const ItemPtr& pNode);

    virtual void nodeAboutToMove(const ItemPtr&, const PointAdapter&) { }
    virtual void nodeMoved(const ItemPtr&) { }

    virtual bool isValid() const & noexcept  { return true; }

private:
    std::deque<ItemPtr> m_nodes;
};

}