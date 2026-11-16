#pragma once

#include "basicitem.hpp"
#include "arrowedconnectionline.hpp"

#include <QFlags>

namespace ObjectItems {

enum ArrowDirection : int {
    AI_AD_None    = 0,
    AI_AD_Up      = 0b0001,
    AI_AD_Down    = 0b0010,
    AI_AD_Left    = 0b0100,
    AI_AD_Right   = 0b1000,

    AI_AD_Horizontal = AI_AD_Left | AI_AD_Right,
    AI_AD_Vertical = AI_AD_Up | AI_AD_Down,
    AI_AD_All     = AI_AD_Horizontal | AI_AD_Vertical,
};

class AnchorItem : public ObjectItems::BasicItem
{
    Q_OBJECT
public:
    explicit AnchorItem(QGraphicsItem* parent = nullptr);
    ~AnchorItem();

    void connectToAnchor(ObjectItems::AbstractConnectionLine* pConnection, bool isPositionFrom);
    void disconnectFromAnchor(ObjectItems::AbstractConnectionLine* pConnection);

    void setCenterRadius(double rad);
    void setArrowSize(double aSize);
    void setArrowLength(double len);

    // Настройки стрелок
    void setArrowDirections(ArrowDirection directions);
    ArrowDirection arrowDirections() const;

    // Callback для клика по стрелке
    using ArrowClickCallback = std::function<void(AnchorItem*, ArrowDirection)>;
    void setArrowClickCallback(ArrowClickCallback callback);
    ArrowClickCallback& arrowClickCallback();

private:
    void updateArrowLines();

    ArrowDirection m_arrowDirections{ArrowDirection::AI_AD_None};
    ArrowClickCallback m_arrowClickCallback;

    QGraphicsEllipseItem*   m_invisibleHoverInterceptor {nullptr};
    QGraphicsEllipseItem*   m_roundItem {nullptr};
    QGraphicsLineItem*      m_crossDiag1Item {nullptr};
    QGraphicsLineItem*      m_crossDiag2Item {nullptr};

    bool m_isHovered {false};
    std::map<ArrowDirection, ArrowedConnectionLine*> m_arrowLines;
    double m_centerRadius {0};
    double m_linesLength {10};

protected:
    void hoverEnterEvent(QGraphicsSceneHoverEvent* e) override;
    void hoverMoveEvent(QGraphicsSceneHoverEvent* e) override;
    void hoverLeaveEvent(QGraphicsSceneHoverEvent* e) override;
};

} // namespace ObjectItems

