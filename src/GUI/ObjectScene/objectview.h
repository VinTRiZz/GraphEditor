#ifndef OBJECTSCENE_H
#define OBJECTSCENE_H

#include <QGraphicsView>

namespace Ui {
class ObjectScene;
}

class ObjectsInternalScene;

/**
 * @brief The ObjectScene class Посредник для отображения объектов на QGraphicScene с доп функционалом
 */
class ObjectView : public QGraphicsView
{
    Q_OBJECT

public:
    explicit ObjectView(QWidget *parent = nullptr);
    ~ObjectView();

    /**
     * @brief resizeScene Принудительно изменить размеры полотна сцены
     * @param iSize Новый размер сцены
     */
    void resizeScene(const QSize& iSize);

    /**
     * @brief setIdGenerator Установить генератор ID объекта (должен возвращать уникальный ID объектов)
     * @param fGen Функтор-генератор
     */
    void setIdGenerator(const std::function<uint()> fGen);

    /**
     * @brief getIdGenerator Функция получения функтора-генератора ID
     * @return Функтор-генератор ID
     */
    std::function<uint()> getIdGenerator() const;

    /**
     * @brief init Создание и настройка внутренних компонент. Вынесена для вызовов при необходимости
     */
    void init();

    /**
     * @brief setContextMenu    Задать контекстное меню
     * @param pMenu             Указатель на контекстное меню
     */
    void setContextMenu(QMenu* pMenu);

    /**
     * @brief clearScene Очищает сцену от объектов, оставляя её полотно нетронутым
     */
    void clearScene();

    /**
     * @brief addObject Добавить объект на сцену. Координатами считаются координаты, заданные в pItem
     * @param pItem Указатель на объект сцены
     * @return ID добавленного объекта, сгенерированный с помощью функтора-генератора
     */
    uint addObject(QGraphicsItem* pItem);

    /**
     * @brief setGrabObject Задать объект, который будет прикреплён к курсору
     * @param pItem         Указатель на объект
     */
    void setGrabObject(QGraphicsItem* pItem);

    /**
     * @brief acceptGrabObject  Принять объект, прикреплённый к курсору (открепить)
     */
    void acceptGrabObject();

    /**
     * @brief rejectGrabObject  Отклонить объект, прикреплённый к курсору (удалить или вернуть на позицию)
     */
    void rejectGrabObject();

    /**
     * @brief getAlObjectIds Возвращает все ID объектов, добавленных в сцену
     * @return Список идентификаторов добавленных объектов
     */
    QList<uint> getAlObjectIds() const;

    /**
     * @brief removeObject Удаляет объект с идентификатором
     * @param itemId Идентификатор объекта
     */
    void removeObject(uint itemId);

private:
    Ui::ObjectScene *ui;
    ObjectsInternalScene*   m_pScene            {nullptr};  //! Сцена для отображения объектов (внутренний класс)
    QMenu*                  m_pContextMenu      {nullptr};  //! Контекстное меню

    std::optional<uint>     m_grabObjectId;                 //! ID объекта, который "прикреплён" к указетелю мыши

    QPointF m_prevPos;                          //! Позиция нажатия на графе
    bool    m_isHoldingLeftButton   {false};    //! Флаг факта того, что пользователь кникнул на сцену ЛКМ
    bool    m_isHoldingMiddleButton {false};    //! Флаг факта того, что пользователь кникнул на сцену СКМ
    bool    m_isMovingByUser        {true};     //! Флаг для перемещений сцены по СКМ

    // Интерфейс QWidget
    void wheelEvent(QWheelEvent* e) override;
    void mousePressEvent(QMouseEvent* e) override;
    void mouseMoveEvent(QMouseEvent* e) override;
    void mouseReleaseEvent(QMouseEvent* e) override;
    void contextMenuEvent(QContextMenuEvent *e) override;
};

#endif // OBJECTSCENE_H
