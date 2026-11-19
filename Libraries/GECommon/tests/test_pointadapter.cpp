#include <gtest/gtest.h>

#include "pointadapter.hpp"

using namespace GraphEditor;

TEST(GECommon, Initial) {
    PointAdapter pa {-2.15, 99.132};
    ASSERT_EQ(pa.x, -2.15);
    ASSERT_EQ(pa.y, 99.132);
    pa.x = 10;
    pa.y = 15;
}


// Has same as QPointF interface
struct ExamplePoint
{
    double x() const { return m_x; }
    void setX(double x) { m_x = x; }

    double y() const { return m_y; }
    void setY(double y) { m_y = y; }

    bool operator ==(const ExamplePoint& op) const {
        return (op.m_x == m_x) && (op.m_y == m_y);
    }

private:
    double m_x;
    double m_y;
};

// May appear in code
struct ExamplePointGetterStyle
{
    double getX() const { return m_x; }
    void setX(double x) { m_x = x; }

    double getY() const { return m_y; }
    void setY(double y) { m_y = y; }

    bool operator ==(const ExamplePointGetterStyle& op) const {
        return (op.m_x == m_x) && (op.m_y == m_y);
    }

private:
    double m_x;
    double m_y;
};

TEST(GECommon, ConvertingFrom) {
    ExamplePoint ip;
    ip.setX(10.1);
    ip.setY(-5.123);
    auto ipcopy = ip;

    ExamplePointGetterStyle ipgs;
    ipgs.setX(8.1);
    ipgs.setY(-11.123);
    auto ipgscopy = ipgs;

    // Check that point did not changed and convertion succeed
    auto p = PointAdapter::fromPoint(ip);
    ASSERT_EQ(ipcopy, ip);
    ASSERT_EQ(ip.x(), p.x);
    ASSERT_EQ(ip.y(), p.y);

    // Check that point did not changed and convertion succeed
    auto pgs = PointAdapter::fromPoint(ipgs);
    ASSERT_EQ(ipgscopy, ipgs);
    ASSERT_EQ(ipgs.getX(), pgs.x);
    ASSERT_EQ(ipgs.getY(), pgs.y);

    // To check constexpr if branch
    auto pcopy = PointAdapter::fromPoint(p);
    ASSERT_EQ(pcopy, p);
}

TEST(GECommon, ConvertingTo) {
    ExamplePoint ip;
    ip.setX(10.1);
    ip.setY(-5.123);

    ExamplePointGetterStyle ipgs;
    ipgs.setX(8.1);
    ipgs.setY(-11.123);

    // Check that point did not changed and convertion succeed
    auto p = PointAdapter::fromPoint(ip);
    auto ipC = p.convert<decltype(ip)>();
    ASSERT_EQ(ipC, ip);

    // Check that point did not changed and convertion succeed
    auto pgs = PointAdapter::fromPoint(ipgs);
    auto ipgsC = pgs.convert<decltype(ipgs)>();
    ASSERT_EQ(ipgsC, ipgs);

    // To check constexpr if branch
    auto pcopy = p.convert<decltype(p)>();
    ASSERT_EQ(pcopy, p);
}