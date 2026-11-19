#pragma once

#include <stdint.h>
#include <memory>

namespace GraphEditor {

class ItemFlagManager;
using ItemFlagManagerPtr = std::shared_ptr<ItemFlagManager>;

/**
 * @brief The ItemFlagManager class Class to handle flags of an item or item group
 */
class ItemFlagManager
{
public:
    static ItemFlagManagerPtr create() {
        return std::make_shared<ItemFlagManager>();
    }

    void setFlag(uint64_t flag, bool isSet) & { isSet ? m_flags |= flag : m_flags &= ~flag; }
    void setFlags(uint64_t flags) & { m_flags &= flags; }
    uint64_t getItemFlags() const & { return m_flags; };

private:
    uint64_t m_flags;
};

}