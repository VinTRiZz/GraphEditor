#pragma once

#include "abstractconnectionline.hpp"

namespace ObjectItems {

class ElegantConnectionLine : public AbstractConnectionLine
{
public:
    explicit ElegantConnectionLine(QGraphicsItem* parent = nullptr);

private:
    QGraphicsPathItem* m_line{nullptr};
    QGraphicsPathItem* m_lineSelected{nullptr};
    QGraphicsPathItem* m_forwardArrow{nullptr};
    QGraphicsPathItem* m_backwardArrow{nullptr};

    QPainterPath createLinePath() const;
};

} // namespace ObjectItems

