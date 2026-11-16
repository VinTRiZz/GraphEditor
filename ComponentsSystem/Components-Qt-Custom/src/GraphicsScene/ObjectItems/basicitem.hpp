#pragma once

#include <QGraphicsItem>
#include <QAction>
#include <QMenu>

#include "basiciteminterface.hpp"

#include "debugmaster.hpp"

namespace ObjectItems {

class BasicItem :
        public QObject,
        public QGraphicsItem,
        public BasicItemInterface
{
    Q_OBJECT
public:
    explicit BasicItem(QGraphicsItem* parent = nullptr);
    ~BasicItem();

    QPainterPath shape() const override;
    QRectF boundingRect() const override;

    virtual QMenu* createContextMenu();

    // Require connect -- указывает, надо ли соединять айтем с изменением QPen / QBrush
    template <bool requireConnect = true, typename ItemTypeT, typename...InitArgs>
    std::enable_if_t<std::is_base_of_v<QGraphicsItem, ItemTypeT>, void> createSubitem(ItemTypeT*& pItem, InitArgs&&...args) {
        pItem = new ItemTypeT(args..., this);

        if constexpr (std::is_base_of_v<BasicItem, ItemTypeT> && requireConnect) {
            QObject::connect(this, &BasicItem::graphicalDataChanged,
                             pItem, [this, pItem](){
                pItem->setLinePen(getLinePen());
                pItem->setLineHoverPen(getLineHoverPen());
                pItem->setLineSelectionPen(getLineSelectionPen());
            });
        }

        if constexpr (std::is_base_of_v<QAbstractGraphicsShapeItem, ItemTypeT> && requireConnect) {
            QObject::connect(this, &BasicItem::graphicalDataChanged,
                             this, [this, pItem](){
                pItem->setPen(getCurrentPen());
                pItem->setBrush(getCurrentBrush());
            });
        }
    }

    BasicItem* getParentObject() const;
    BasicItem* getParentObjectOfComplex() const;

signals:
    void idChanged();
    void displayNameChanged();
    void internalDataChanged();
    void graphicalDataChanged();

    void itemSelected();
    void itemSelectionChanged(bool isSelected);
    void itemDeselected();

    void itemClicked();

    void itemAboutToMove(const QPointF& newPos);
    void itemMoved(const QPointF& newPos);

    void itemMovedOnScene(const QPointF& newScenePos);

protected:
    // QGraphicsItem interface
    virtual void paint(QPainter* painter, const QStyleOptionGraphicsItem* option,
               QWidget* widget) override;
    virtual QVariant itemChange(GraphicsItemChange change, const QVariant &value) override;

    void mousePressEvent(QGraphicsSceneMouseEvent* e) override;
    void mouseMoveEvent(QGraphicsSceneMouseEvent* e) override;
    void mouseReleaseEvent(QGraphicsSceneMouseEvent* e) override;

    void hoverEnterEvent(QGraphicsSceneHoverEvent* e) override;
    void hoverLeaveEvent(QGraphicsSceneHoverEvent* e) override;

    bool getIsHovered() const;
    QPen getCurrentPen() const;
    QBrush getCurrentBrush() const;

    // BasicItemInterface interface
private:
    void processIdChange() override;
    void processDisplayNameChange() override;
    void processInternalDataChange() override;
    void processColorChange() override;

    bool m_isClickedOnMe {false};
    bool m_isHovered {false};

    QPointF m_prevClickScreenPos; // TODO: Придумать получше вариант, маппинг тут геморройный
    QPointF m_clickOffset;
    bool    m_isDeltaGot {false};   // Оптимизация
    double  m_startMoveDelta {2.0}; // Расстояние между точкой нажатия и точкой релиза для начала перемещения
    void    processMoveEvent(QGraphicsSceneMouseEvent* e);

    friend class DebugMaster;
};

}
