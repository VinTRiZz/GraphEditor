#pragma once

#include <Components/CustomQt/ObjectView/ObjectItems.h>
#include <QGraphicsView>

#include "ovconstants.hpp"

namespace OVLayers {

class OVInternalScene;

/**
 * @brief The OVSceneConfigLayer class "Нулевой" уровень, настраивает сцену и полотно
 */
class OVCanvasLayer : public QGraphicsView
{
    Q_OBJECT

    // Блокировка интерфейса
    using QGraphicsView::setScene;      // Запрещено к использованию извне
    using QGraphicsView::setSceneRect;  // Используйте setCanvasRect
    using QGraphicsView::items;         // Используйте getItems
    using QGraphicsView::itemAt;        // Используйте getTopItem
    using QGraphicsView::resetTransform;// Используйте resetScale()
    using QGraphicsView::scene;         // Используйте дублирующие сцену методы (например, addObject == addItem)

public:
    explicit OVCanvasLayer(QWidget* parent = nullptr);

    void setCanvasRect(const QRectF& iRect);
    QRectF getCanvasRect() const;

    OVInternalScene* getScene() const;
    QGraphicsRectItem* getCanvas() const;

    ObjectItems::BasicItem* getObject(const QPoint& viewportPos) const;
    QGraphicsItem*          getTopItem(const QPoint& viewportPos) const;
    QList<QGraphicsItem*>   getItems(const QPoint& viewportPos, bool sorted = false) const;

    double getCurrentScale() const;

    bool isIdFree(ObjectItems::objectId_t id) const;
    ObjectItems::objectId_t getFreeSystemId() const;
    ObjectItems::objectId_t getFreeObjectId() const;

    void addObject(ObjectItems::BasicItem* pItem);
    std::unordered_map<ObjectItems::objectId_t, ObjectItems::BasicItem*> getObjects() const;
    ObjectItems::BasicItem* getObject(ObjectItems::objectId_t itemId) const;
    void removeObject(ObjectItems::BasicItem* pItem);

public slots:
    void setNavigationEnabled(bool isEn);

    void resetScale();
    void zoomIn();
    void zoomOut();
    void customZoom(double scaleCoeff);

signals:
    void scaleChanged(double);
    void visibleRectMoved();

protected:
    virtual bool isSystemItem(QGraphicsItem* pItem) const;

    void wheelEvent(QWheelEvent* e) override;
    void mousePressEvent(QMouseEvent* e) override;
    void mouseMoveEvent(QMouseEvent* e) override;
    void mouseReleaseEvent(QMouseEvent* e) override;

    bool eventFilter(QObject* object, QEvent* event) override;

private:
    OVInternalScene* m_pInternalScene {nullptr};
    QGraphicsRectItem* m_pCanvasItem {nullptr};

    bool m_isSelecting {false};
    QGraphicsRectItem* m_selectionRectItem {nullptr};

    std::unordered_map<ObjectItems::objectId_t, ObjectItems::BasicItem*> m_registeredItems;

    bool m_isNavigationEnabled {true}; //! Флаг включения навигации

    bool m_isHoldingMiddleButton{
        false};  //! Флаг факта того, что пользователь кликнул СКМ на сцене

    QPointF m_prevPos;  //! Позиция нажатия на графе
};

}
