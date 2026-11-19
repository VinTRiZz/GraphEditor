#include <gtest/gtest.h>

#include "positionableitem.hpp"

using namespace GraphEditor;

class MockPosItem : public PositionableItem<MockPosItem>
{
public:
    PointAdapter m_prevPos;
    PointAdapter m_nextPos;
    PointAdapter m_currentPos;

public:
    void positionAboutToChange(const PointAdapter& prevPos, const PointAdapter& nextPos) {
        m_prevPos = prevPos;
        m_nextPos = nextPos;
    }

    void positionChanged(const PointAdapter& currentPos) {
        m_currentPos = currentPos;
    }
};

TEST(GECommon, PositionableItemCallsProcessors) {
    MockPosItem item;

    PointAdapter startPos{22.4, -12.11};
    PointAdapter nextPos{-5328.1353, 817352.5436};

    item.setPos(startPos);
    ASSERT_EQ(startPos,  item.m_currentPos);

    item.setPos(nextPos);
    ASSERT_EQ(startPos, item.m_prevPos);
    ASSERT_EQ(nextPos,  item.m_nextPos);
    ASSERT_EQ(nextPos,  item.m_currentPos);
}