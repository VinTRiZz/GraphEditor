#pragma once

#include "basicitem.hpp"

#include <QPen>

namespace ObjectItems {

/**
 * @brief The LineDirectionType enum Тип направления линии соединения
 */
enum LineDirectionType : short {
    None = 0,
    Forward = 0b01,
    Backward = 0b10,
    Bidirectional = Forward | Backward,
};

/**
 * @brief The LineAngleType enum Угол между сторонами треугольника стрелки
 */
enum LineAngleType : short {
    A_15 = 0,
    A_30,
    A_45,
    A_60,
    A_75,
    A_90,
};

class AbstractConnectionLine : public BasicItem
{
public:
    explicit AbstractConnectionLine(QGraphicsItem *parent = nullptr);

    void subscribeForMoves(BasicItem* pItem, bool isFrom = true, const QPointF& offsetPos = {});
    void unsubscribeForMoves(BasicItem* pItem);

    void setDirection(LineDirectionType arrType);
    LineDirectionType getDirection() const;

    void setArrowHeight(double arHeight);
    double getArrowHeight() const;

    void setArrowAngle(LineAngleType lineAngle);
    LineAngleType getArrowAngle() const;

    void setArrowFilled(bool isFilled);
    bool getIsArrowFilled() const;

    void setLine(const QLineF& line);
    void setLine(const QPointF& p1, const QPointF& p2);
    QLineF getLine() const;

    void setPositionFrom(const QPointF& posFrom);
    QPointF getPositionFrom() const;

    void setPositionTo(const QPointF& posTo);
    QPointF getPositionTo() const;

    void resetCachedArrowPath(); // FIXME: Исправить баг с "нулевым" шейпом иным способом

private:
    QLineF              m_straightLine  {};
    LineDirectionType   m_directionType     {LineDirectionType::None};
    LineAngleType       m_arrowAngle    {LineAngleType::A_30};
    QSizeF              m_arrowSize     {10, 10};
    bool                m_isArrowFilled {true};

    mutable bool m_isArrowSizeChanged {true};
    mutable QPainterPath m_cachedArrowpath;

protected:
    QPainterPath createArrowPath() const; // Направлена вверх после создания
};

} // namespace ObjectItems

