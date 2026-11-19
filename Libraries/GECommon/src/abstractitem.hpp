#pragma once

#include <stdint.h>
#include <memory>

#include <optional>
#include <variant>

#include <Components/ExtraClasses/Error.h>

#include "itemflagmanager.hpp"

namespace GraphEditor {

// Common defaults
using id_t = std::optional<uint64_t>;

class AbstractItem;
using ItemPtr = std::shared_ptr<AbstractItem>;
using ItemParentPtr = std::weak_ptr<AbstractItem>;

// Flags that must be common for every item
enum CommonItemFlags
{
    Readable    = 0b1,
    Writable    = 0b10,
    ReadWrite   = 0b11,
};

/**
 * @brief The AbstractItem class    Basic item for graph items data manipulation
 */
class AbstractItem
{
public:
    explicit AbstractItem(const ItemParentPtr& parent) : m_parent {parent} { }
    AbstractItem(const AbstractItem&) = default;
    AbstractItem(AbstractItem&&) = default;
    virtual ~AbstractItem() = default;

    // ID manipulation
    void setId(const id_t& id) & { m_id = id; }
    void setId(id_t&& id) & { m_id = std::move(id); }
    const id_t& getId() const & noexcept { return m_id; }

    virtual bool isValid() const & noexcept = 0;

    // Used to determine item capabilities
    ItemFlagManagerPtr getFlagManager() const { return m_pFlagManager; }
    void setFlagManager(const ItemFlagManagerPtr& pManager) {
        m_pFlagManager = pManager;
        processFlagsChange();
    }
    virtual void processFlagsChange() & = 0;

private:
    ItemParentPtr m_parent;
    id_t m_id;
    ItemFlagManagerPtr m_pFlagManager;
};

} // namespace GraphEditor
