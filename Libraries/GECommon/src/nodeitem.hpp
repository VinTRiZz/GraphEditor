#pragma once

#include "abstractitem.hpp"
#include "positionableitem.hpp"

namespace GraphEditor {

/**
 * @brief The NodeItem class    Graph's node item
 */
class NodeItem :
                 public AbstractItem,
                 public PositionableItem<NodeItem>
{
public:
    using AbstractItem::AbstractItem;

    static inline int testVar {15};

    static ItemPtr create(const ItemPtr& parent = {}) {
        testVar++;
        return std::make_shared<NodeItem>(parent);
    }

    template <typename T>
    void setName(T&& name) & { m_name = name; }
    std::string_view getName() const & noexcept { return m_name; }

    template <typename T>
    void setDescription(T&& descr) & { m_description = descr; }
    std::string_view getDescription() const & noexcept { return m_description; }

    virtual bool isValid() const & noexcept  { return true; }

    // For refered item processing
    virtual void positionAboutToChange(const PointAdapter&, const PointAdapter&) & { }
    virtual void positionChanged(const PointAdapter&) & { }
    virtual void processFlagsChange() & { }

private:
    std::string m_name;
    std::string m_description;
};

}