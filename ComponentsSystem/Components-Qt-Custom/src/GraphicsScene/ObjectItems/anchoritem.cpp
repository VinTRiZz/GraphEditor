#include "anchoritem.hpp"

#include <QPainter>

#include <QDebug>

#include <QCursor>

#include <Components/Logger/Logger.h>

namespace ObjectItems {

// Реализация AnchorItem
AnchorItem::AnchorItem(QGraphicsItem* parent)
    : BasicItem(parent)
{
    setSystemName("Anchor");
    setObjectType(ObjectType::OIT_AnchorItem);
    setZValue(1'000); // Нужно заменять при создании

    setAcceptHoverEvents(true);
    setFlag(QGraphicsItem::ItemIsSelectable, false);
    setFlag(QGraphicsItem::ItemIsMovable, false);

    createSubitem(m_roundItem);
    m_roundItem->setAcceptHoverEvents(true);
    m_roundItem->setPen(getLinePen());
    m_roundItem->setBrush(getBackgroundBrush());

    createSubitem(m_crossDiag1Item);
    m_crossDiag1Item->setZValue(1);
    m_crossDiag1Item->setAcceptHoverEvents(true);
    m_crossDiag1Item->setPen(getLinePen());

    createSubitem(m_crossDiag2Item);
    m_crossDiag2Item->setAcceptHoverEvents(true);
    m_crossDiag2Item->setZValue(1);
    m_crossDiag2Item->setPen(getLinePen());

    createSubitem(m_invisibleHoverInterceptor);
    m_invisibleHoverInterceptor->setAcceptHoverEvents(true);
    m_invisibleHoverInterceptor->setOpacity(0.1);
    m_invisibleHoverInterceptor->setPen(getLineHoverPen());
    m_invisibleHoverInterceptor->setBrush(getBackgroundHoverBrush());
    m_invisibleHoverInterceptor->hide();
    m_invisibleHoverInterceptor->setZValue(-1);

    auto addLine = [this](ArrowDirection dir){
        ArrowedConnectionLine* pLine {nullptr};
        createSubitem(pLine);
        pLine->setZValue(1);
        pLine->setAcceptHoverEvents(true);
        pLine->setDirection(LineDirectionType::Forward);
        pLine->setArrowAngle(LineAngleType::A_90);
        connect(pLine, &BasicItem::itemClicked,
                this, [this, dir](){
            if (m_arrowClickCallback) {
                m_arrowClickCallback(this, dir);
            }
        });
        m_arrowLines.emplace(dir, pLine);
    };
    addLine(ArrowDirection::AI_AD_Up);
    addLine(ArrowDirection::AI_AD_Right);
    addLine(ArrowDirection::AI_AD_Down);
    addLine(ArrowDirection::AI_AD_Left);

    connect(this, &BasicItem::graphicalDataChanged,
            this, [this](){
        auto currentPen = getCurrentPen();
        auto currentBrush = getCurrentBrush();
        m_roundItem->setPen(currentPen);
        m_roundItem->setBrush(getBackgroundBrush());
        m_crossDiag1Item->setPen(getLinePen());
        m_crossDiag2Item->setPen(getLinePen());
    });

    setCenterRadius(10);
    setArrowSize(4);
    setArrowLength(m_linesLength);
    setBackgroundBrush(Qt::darkGray);
    setBackgroundHoverBrush(QColor(130, 190, 100));

    auto linePen = QPen(Qt::black, 1, Qt::SolidLine, Qt::RoundCap);
    setLinePen(linePen);

    auto lineHoverPen = linePen;
    lineHoverPen.setColor(QColor(130, 190, 100));
    setLineHoverPen(lineHoverPen);
    updateArrowLines();
}

AnchorItem::~AnchorItem()
{

}

void AnchorItem::connectToAnchor(AbstractConnectionLine *pConnection, bool isPositionFrom)
{
    if (isPositionFrom) {
        connect(this, &BasicItem::itemMovedOnScene,
                pConnection, [this, pConnection](){
            pConnection->setPositionFrom(pos());
        });

        auto conParent = pConnection->parentItem();
        QPointF curpos = scenePos();
        if (conParent) {
            curpos = conParent->mapFromScene(curpos);
        }
        pConnection->setPositionFrom(curpos);
    } else {
        connect(this, &BasicItem::itemMovedOnScene,
                pConnection, [this, pConnection](){
            pConnection->setPositionTo(pos());
        });
        auto conParent = pConnection->parentItem();
        QPointF curpos = scenePos();
        if (conParent) {
            curpos = conParent->mapFromScene(curpos);
        }
        pConnection->setPositionTo(curpos);
    }
}

void AnchorItem::disconnectFromAnchor(ObjectItems::AbstractConnectionLine* pConnection)
{
    disconnect(this, &BasicItem::itemMovedOnScene,
               pConnection, nullptr);
}

void AnchorItem::setCenterRadius(double rad)
{
    m_centerRadius = rad;

    // Round
    m_roundItem->setRect(QRectF(-m_centerRadius,
                                -m_centerRadius,
                                m_centerRadius * 2,
                                m_centerRadius * 2));

    // Cross
    auto crossRadius = m_centerRadius * 0.5;
    m_crossDiag1Item->setLine(QLineF(-crossRadius, -crossRadius,
                            crossRadius, crossRadius));
    m_crossDiag2Item->setLine(QLineF(crossRadius, -crossRadius,
                            -crossRadius, crossRadius));
}

void AnchorItem::setArrowSize(double aSize)
{
    auto centerPos = QPointF(m_centerRadius, m_centerRadius);
    for (auto& [arrowDir, arrowLine] : m_arrowLines) {
        arrowLine->setArrowHeight(aSize);
    }
}

void AnchorItem::setArrowLength(double len)
{
    m_linesLength = len;
    updateArrowLines();
}

void AnchorItem::setArrowDirections(ArrowDirection directions)
{
    m_arrowDirections = directions;
    updateArrowLines();
}

void AnchorItem::updateArrowLines()
{
    auto drawPen = m_isHovered ? getLineHoverPen() : (isSelected() ? getLineSelectionPen() : getLinePen());
    auto lineStartOffset = m_centerRadius * 1.2;

    for (auto& [arrowDir, arrowLine] : m_arrowLines) {
        arrowLine->setVisible(m_isHovered && (m_arrowDirections & arrowDir));

        if (!arrowLine->isVisible()) {
            arrowLine->setLine({}); // Чтобы не "хавало" ховер ивенты
            continue;
        }

        auto line = arrowLine->getLine();

        auto xModifier = arrowDir & AI_AD_Right ? 1 : -1;
        auto lineXlen = arrowDir & AI_AD_Horizontal ? (m_linesLength + lineStartOffset) * xModifier : 0;

        auto yModifier = arrowDir & AI_AD_Down ? 1 : -1;
        auto lineYlen = arrowDir & AI_AD_Vertical ? (m_linesLength + lineStartOffset) * yModifier : 0;

        line.setP1({arrowDir & AI_AD_Horizontal ? xModifier * lineStartOffset : 0, arrowDir & AI_AD_Vertical ? yModifier * lineStartOffset : 0});
        line.setP2({lineXlen, lineYlen});

        arrowLine->setLine(line);
    }
}

void AnchorItem::hoverEnterEvent(QGraphicsSceneHoverEvent *e)
{
    m_isHovered = true;
    m_invisibleHoverInterceptor->show();
    m_roundItem->setBrush(getBackgroundHoverBrush());
    updateArrowLines();

    auto lineTotalLen = m_centerRadius * 1.5 + m_linesLength;
    auto hInterRect = QRectF(-lineTotalLen - m_centerRadius, -lineTotalLen - m_centerRadius,
                             lineTotalLen + m_centerRadius, lineTotalLen + m_centerRadius);
    hInterRect.moveCenter(m_roundItem->rect().center());
    m_invisibleHoverInterceptor->setRect(hInterRect);
    BasicItem::hoverEnterEvent(e);
}

void AnchorItem::hoverMoveEvent(QGraphicsSceneHoverEvent *e)
{
    updateArrowLines();
    BasicItem::hoverMoveEvent(e);
}

void AnchorItem::hoverLeaveEvent(QGraphicsSceneHoverEvent *e)
{
    m_isHovered = false;
    m_invisibleHoverInterceptor->hide();
    m_roundItem->setBrush(getBackgroundBrush());
    updateArrowLines();
    m_invisibleHoverInterceptor->setRect({});
    BasicItem::hoverLeaveEvent(e);
}

ArrowDirection AnchorItem::arrowDirections() const { return m_arrowDirections; }

void AnchorItem::setArrowClickCallback(ArrowClickCallback callback) { m_arrowClickCallback = callback; }

AnchorItem::ArrowClickCallback &AnchorItem::arrowClickCallback() { return m_arrowClickCallback; }

} // namespace ObjectItems
