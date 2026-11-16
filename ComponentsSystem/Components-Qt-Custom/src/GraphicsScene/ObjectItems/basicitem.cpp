#include "basicitem.hpp"

#include <QPainter>
#include <QStyleOptionGraphicsItem>

#include <math.h>

#include <QGraphicsSceneMouseEvent>
#include <QGraphicsSceneHoverEvent>

#include "../ObjectView/ovinternalscene.hpp"

#include <Components/Logger/Logger.h>

namespace ObjectItems
{

BasicItem::BasicItem(QGraphicsItem *parent) :
    QObject(),
    QGraphicsItem(parent),
    BasicItemInterface()
{
    setFlag(ItemSendsScenePositionChanges, true);
    setFlag(ItemHasNoContents, true);
    setFlag(ItemSendsGeometryChanges, true);
    setSystemName("Unknown");
    setObjectType(ObjectType::OIT_BasicItem);
}

BasicItem::~BasicItem()
{

}

QPainterPath BasicItem::shape() const
{
    QPainterPath path;
    for (QGraphicsItem* child : childItems()) {
        path.addPath(child->mapToParent(child->shape()));
    }
    return path;
}

QRectF BasicItem::boundingRect() const
{
    return shape().boundingRect();
}

QMenu *BasicItem::createContextMenu()
{
    auto res = new QMenu;
    res->setTitle(getDisplayName().isEmpty() ? getSystemName() : getDisplayName());

    auto pOpacityAction = new QAction("Прозрачный", res);
    connect(pOpacityAction, &QAction::triggered,
            this, [this, pOpacityAction, res]() {
        if (opacity() < 0.9) {
            setOpacity(1);
        } else {
            setOpacity(0.2);
        }
        res->deleteLater();
    });
    pOpacityAction->setCheckable(true);
    pOpacityAction->setChecked(opacity() < 0.9);

    res->addAction(pOpacityAction);

    return res;
}

BasicItem *BasicItem::getParentObject() const
{
    return data(ObjectDataRole::OBJECTDATAROLE_PARENTITEM_POINTER).value<BasicItem*>();
}

BasicItem *BasicItem::getParentObjectOfComplex() const
{
    return data(ObjectDataRole::OBJECTDATAROLE_COMPLEX_PARENTITEM_POINTER).value<BasicItem*>();
}

void BasicItem::paint(
        QPainter *painter,
        const QStyleOptionGraphicsItem *option,
        QWidget *widget)
{

}

QVariant BasicItem::itemChange(GraphicsItemChange change, const QVariant &value)
{
    auto res = QGraphicsItem::itemChange(change, value);

    switch (change)
    {
    case ItemPositionChange:
        emit itemAboutToMove(value.value<QPointF>());
        break;

    case ItemPositionHasChanged:
        emit itemMoved(pos());
        break;

    case ItemScenePositionHasChanged:
        emit itemMovedOnScene(scenePos());
        break;

    case ItemSelectedHasChanged:
        if (value.toBool()) {
            emit itemSelected();
        } else {
            emit itemDeselected();
        }
        emit itemSelectionChanged(value.toBool());
        break;

    case ItemParentChange:
        setData(ObjectDataRole::OBJECTDATAROLE_PARENTITEM_POINTER, value);
        if (nullptr != parentItem()) {
            setData(ObjectDataRole::OBJECTDATAROLE_COMPLEX_PARENTITEM_POINTER,
                               parentItem()->data(ObjectDataRole::OBJECTDATAROLE_COMPLEX_PARENTITEM_POINTER));
        } else {
            setData(ObjectDataRole::OBJECTDATAROLE_COMPLEX_PARENTITEM_POINTER, {});
        }
        break;

    case ItemChildAddedChange:
    {
        auto pItem = value.value<QGraphicsItem*>();
        pItem->setParentItem(this);
        pItem->setData(ObjectDataRole::OBJECTDATAROLE_PARENTITEM_POINTER, QVariant::fromValue(this));
        pItem->setData(ObjectDataRole::OBJECTDATAROLE_COMPLEX_PARENTITEM_POINTER,
                       data(ObjectDataRole::OBJECTDATAROLE_COMPLEX_PARENTITEM_POINTER));
    }
        break;

    default:
        break;
    }

    return res;
}

void BasicItem::mousePressEvent(QGraphicsSceneMouseEvent *e)
{
    QGraphicsItem::mousePressEvent(e);
    m_prevClickScreenPos = e->screenPos();
    m_clickOffset = e->scenePos() - scenePos();
    e->setAccepted(true);
}

void BasicItem::mouseMoveEvent(QGraphicsSceneMouseEvent *e)
{
    QGraphicsItem::mouseMoveEvent(e);
    if (!(e->buttons() & Qt::LeftButton) ||
        !(flags() & ItemIsMovable)) {
        return;
    }

    if (m_isDeltaGot) {
        processMoveEvent(e);
    } else if (QLineF(m_prevClickScreenPos, e->screenPos()).length() > m_startMoveDelta) {
        processMoveEvent(e);
        m_isDeltaGot = true;
    }
}

void BasicItem::mouseReleaseEvent(QGraphicsSceneMouseEvent *e)
{
    QGraphicsItem::mouseReleaseEvent(e);
    if (!m_isDeltaGot) {
        emit itemClicked();
    }
    m_isDeltaGot = false;
}

void BasicItem::hoverEnterEvent(QGraphicsSceneHoverEvent *e)
{
    m_isHovered = true;
    QGraphicsItem::hoverEnterEvent(e);
}

void BasicItem::hoverLeaveEvent(QGraphicsSceneHoverEvent *e)
{
    m_isHovered = false;
    QGraphicsItem::hoverLeaveEvent(e);
}

bool BasicItem::getIsHovered() const
{
    return m_isHovered;
}

QPen BasicItem::getCurrentPen() const
{
    return getIsHovered() ? getLineHoverPen() : (isSelected() ? getLineSelectionPen() : getLinePen());
}

QBrush BasicItem::getCurrentBrush() const
{
    return getIsHovered() ? getBackgroundHoverBrush() : (isSelected() ? getBackgroundSelectionBrush() : getBackgroundBrush());
}

void BasicItem::processIdChange()
{
    setData(ObjectDataRole::OBJECTDATAROLE_ID, getItemId());
    emit idChanged();
}

void BasicItem::processDisplayNameChange()
{
    emit displayNameChanged();
}

void BasicItem::processInternalDataChange()
{
    emit internalDataChanged();
}

void BasicItem::processColorChange()
{
    emit graphicalDataChanged();
}

void BasicItem::processMoveEvent(QGraphicsSceneMouseEvent *e)
{
    auto targetPos = e->scenePos() - m_clickOffset;
    if (parentItem()) {
        targetPos = parentItem()->mapFromScene(targetPos);
    }

    auto itemScene = static_cast<OVLayers::OVInternalScene*>(scene());
    if (nullptr != itemScene && itemScene->getIsGridEnabled()) {
        auto gridSize = itemScene->getGridSize() / 2;
        targetPos.setX(static_cast<int>(targetPos.x() + 1) / gridSize * gridSize);
        targetPos.setY(static_cast<int>(targetPos.y() + 1) / gridSize * gridSize);
    }

    setPos(targetPos);
}

}
