#pragma once

#include "pointadapter.hpp"

namespace GraphEditor {

namespace detail {
template <typename BaseT>
concept CBaseCanProcessPositionChange =
    requires (BaseT baseV, const PointAdapter& prevPos, const PointAdapter& nextPos) {
        baseV.positionAboutToChange(prevPos, nextPos);
        baseV.positionChanged(nextPos);
    };
}

/**
 * @brief The PositionableItem class    Item with position change processing
 */
template <typename BaseT>
class PositionableItem
{
public:
    void setPos(const PointAdapter& pos) &
        requires detail::CBaseCanProcessPositionChange<BaseT>
    {
        static_cast<BaseT*>(this)->positionAboutToChange(m_pos, pos);
        m_pos = pos;
        static_cast<BaseT*>(this)->positionChanged(m_pos);
    }

    const PointAdapter& getPos() const & noexcept { return m_pos; }

private:
    PointAdapter m_pos;
};

}