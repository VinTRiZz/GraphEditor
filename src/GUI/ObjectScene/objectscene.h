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
class ObjectScene : public QGraphicsView
{
    Q_OBJECT

public:
    explicit ObjectScene(QWidget *parent = nullptr);
    ~ObjectScene();

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
     * @brief addObject Добавить объект на сцену. Координатами считаются координаты, заданные в pItem
     * @param pItem Указатель на объект сцены
     * @return ID добавленного объекта, сгенерированный с помощью функтора-генератора
     */
    uint addObject(QGraphicsItem* pItem);

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
    ObjectsInternalScene*       m_pScene            {nullptr};  //! Сцена для отображения объектов (внутренний класс)
};

#endif // OBJECTSCENE_H
