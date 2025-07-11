#ifndef OBJECTSCENE_H
#define OBJECTSCENE_H

#include <QGraphicsView>

#include <optional>

#include "objectsceneconstants.h"

namespace Ui {
class ObjectScene;
}

class ObjectsInternalScene;
class QMenu;

/**
 * @brief The ObjectScene class Посредник для отображения объектов на QGraphicScene с доп функционалом
 */
class ObjectView : public QGraphicsView
{
    Q_OBJECT

    // Запрет интерфейса чтобы избежать поломки дочерними классами
    using QGraphicsView::scene;
    using QGraphicsView::setScene;

public:
    explicit ObjectView(QWidget *parent = nullptr);
    ~ObjectView();

    void init();
    bool isInited() const;

    void setContextMenu(QMenu* pMenu);
    QGraphicsItem* getContextMenuItem();

    ObjectSceneConstants::objectId_t addObject(QGraphicsItem* pItem);
    QList<ObjectSceneConstants::objectId_t> getAlObjectIds() const;
    void removeAllObjects();
    void removeObject(ObjectSceneConstants::objectId_t itemId);

    QGraphicsItem* getGrabObject() const;

public slots:
    void setGrabObject(QGraphicsItem* pItem);
    void acceptGrabObject();
    void rejectGrabObject();

private:
    Ui::ObjectScene *ui;
    ObjectsInternalScene*   m_pScene            {nullptr};  //! Сцена для отображения объектов (внутренний класс)
    QMenu*                  m_pContextMenu      {nullptr};  //! Контекстное меню
    QGraphicsItem*          m_contextMenuItem   {nullptr};  //! Объект, который находился под указателем мыши во время вызова контекстного меню

    QPointF                                         m_grabObjectPos;    //! Положение объекта до grab
    std::optional<ObjectSceneConstants::objectId_t> m_grabObjectId;     //! ID объекта, который "прикреплён" к указателю мыши

    QPointF m_prevPos;                                      //! Позиция нажатия на графе
    bool    m_isHoldingLeftButton   {false};                //! Флаг факта того, что пользователь кникнул на сцену ЛКМ
    bool    m_isHoldingMiddleButton {false};                //! Флаг факта того, что пользователь кникнул на сцену СКМ
    bool    m_isMovingByUser        {true};                 //! Флаг для перемещений сцены по СКМ

    // Интерфейс QWidget
    void wheelEvent(QWheelEvent* e) override;
    void mousePressEvent(QMouseEvent* e) override;
    void mouseMoveEvent(QMouseEvent* e) override;
    void mouseReleaseEvent(QMouseEvent* e) override;
    void contextMenuEvent(QContextMenuEvent *e) override;

signals:
    void clickedOnItem(QGraphicsItem* pTargetItem);
    void pressedOnItem(QGraphicsItem* pTargetItem);
    void releasedOnItem(QGraphicsItem* pTargetItem);
};

#endif // OBJECTSCENE_H
