#pragma once

#include "abstractconnectionline.hpp"

namespace ObjectItems {

class ArrowedConnectionLine : public AbstractConnectionLine
{
public:
    explicit ArrowedConnectionLine(QGraphicsItem *parent = nullptr);

private:
    QGraphicsLineItem* m_line{nullptr};
    QGraphicsPathItem* m_forwardArrow {nullptr};
    QGraphicsPathItem* m_backwardArrow {nullptr};

    void updateLines();

protected:
    void hoverEnterEvent(QGraphicsSceneHoverEvent* e) override;
    void hoverLeaveEvent(QGraphicsSceneHoverEvent* e) override;
};

} // namespace ObjectItems

