#ifndef INTERACTIVEOBJECTVIEW_H
#define INTERACTIVEOBJECTVIEW_H

#include "objectviewbase.h"

class InteractiveObjectView : public ObjectViewBase {
    Q_OBJECT
public:
    explicit InteractiveObjectView(QWidget* parent);

    void zoomIn();
    void zoomOut();
    void customZoom(double scaleCoeff);

    double getCurrentScale() const;

    void setCanvasRect(const QRectF& iRect) override;

    void setContextMenu(QMenu* pMenu);
    QGraphicsItem* getContextMenuItem();

    void setMovingCallback(
        const std::function<void(const QPointF&)>& callbackFunc);

    QGraphicsItem* getGrabObject() const;

public slots:
    void setGrabObject(QGraphicsItem* pItem);
    void acceptGrabObject();
    void rejectGrabObject();

signals:
    void clickedOnItem(QGraphicsItem* pTargetItem);
    void pressedOnItem(QGraphicsItem* pTargetItem);
    void releasedOnItem(QGraphicsItem* pTargetItem);

    void scaleChanged();

private:
    QMenu* m_mainContextMenu{nullptr};  //! Основное контекстное меню
    QAction* m_contextAction{
        nullptr};  //! Действие с пометкой "Контекст" в контестном меню
    QGraphicsItem* m_contextMenuItem{
        nullptr};  //! Объект, который находился под указателем мыши во время
                   //! вызова контекстного меню

    QPointF m_grabObjectPos;  //! Положение объекта до grab
    std::optional<ObjectViewConstants::objectId_t>
        m_grabObjectId;  //! ID объекта, который "прикреплён" к указателю мыши

    QPointF m_prevPos;  //! Позиция нажатия на графе
    bool m_isHoldingLeftButton{
        false};  //! Флаг факта того, что пользователь кникнул на сцену ЛКМ
    bool m_isHoldingMiddleButton{
        false};  //! Флаг факта того, что пользователь кникнул на сцену СКМ
    bool m_isMovingByUser{true};   //! Флаг для перемещений сцены по СКМ
    double m_currentZoomValue{1};  //! Текущий коэффициент увеличения

    std::function<void(const QPointF&)>
        m_movingCallback;  //! Колбек для обработки перемещения по сцене

    ObjectViewItems::CenterItem*        m_pCenterItem {nullptr}; //! Центральный элемент сцены
    ObjectViewItems::SceneMarkerItem*   m_centerPointerItem {nullptr};

    /**
     * @brief updateCenterPoint Обновить позицию центральной точки сцены
     */
    void updateCenterPoint();

    /**
     * @brief updateCenterMarker Показывает или скрывает маркер центра
     */
    void updateCenterMarker();

    // Интерфейс QWidget
protected:
    void wheelEvent(QWheelEvent* e) override;
    void mousePressEvent(QMouseEvent* e) override;
    void mouseMoveEvent(QMouseEvent* e) override;
    void mouseReleaseEvent(QMouseEvent* e) override;
    void contextMenuEvent(QContextMenuEvent* e) override;
};

#endif  // INTERACTIVEOBJECTVIEW_H
