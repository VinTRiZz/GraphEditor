#ifndef OBJECTSCENE_H
#define OBJECTSCENE_H

#include <QGraphicsView>

#include <optional>

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

public:
    explicit ObjectView(QWidget *parent = nullptr);
    ~ObjectView();

    void setIdGenerator(const std::function<uint()> fGen);
    std::function<uint()> getIdGenerator() const;

    void resizeScene(const QSize& iSize);

    void init();
    bool isInited() const;

    void setContextMenu(QMenu* pMenu);
    QGraphicsItem* getContextMenuItem();

    uint addObject(QGraphicsItem* pItem);
    QList<uint> getAlObjectIds() const;
    void removeAllObjects();
    void removeObject(uint itemId);

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
    std::optional<uint>     m_grabObjectId;                 //! ID объекта, который "прикреплён" к указателю мыши

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
