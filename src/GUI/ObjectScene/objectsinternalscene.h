#ifndef OBJECTSINTERNALSCENE_H
#define OBJECTSINTERNALSCENE_H

#include <QGraphicsScene>
#include <QMap>

namespace Ui {
class ObjectsInternalScene;
}

class ObjectsInternalScene : public QGraphicsScene
{
    Q_OBJECT

public:
    explicit ObjectsInternalScene(QObject *parent = nullptr);
    ~ObjectsInternalScene();

    /**
     * @brief setIdGenerator Установить генератор ID объекта (должен возвращать уникальный ID объектов)
     * @param fGen Функтор-генератор
     */
    void setIdGenerator(const std::function<uint()> fGen);

    /**
     * @brief init Создание и настройка внутренних компонент. Вынесена для вызовов при необходимости
     */
    void init();

    /**
     * @brief addObject Добавить объект на сцену. Координатами считаются координаты, заданные в pItem
     * @param pItem Указатель на объект сцены. Его parentItem замещается nullItem сцены
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

    /**
     * @brief setBackgroundColor Задать цвет плоскости, на которой будут отображаться объекты
     * @param bgrColor Цвет в соответствии с Qt типами
     */
    void setBackgroundColor(const QColor& bgrColor);

    /**
     * @brief setBackgroundColor Задать цвет рамки плоскости, на которой будут отображаться объекты
     * @param borderColor Цвет в соответствии с Qt типами
     */
    void setBorderColor(const QColor& borderColor);

private:
    std::function<uint()>       m_idGenerator;                  //! Генератор ID для объектов
    QGraphicsItem*              m_pNullItem         {nullptr};  //! Объект, который являет собой пространство сцены (как бы ограниченная плоскость для расположения объектов)
    QMap<uint, QGraphicsItem*>  m_objectsMap;                   //! Словарь для сохранения ID объектов
};

#endif // OBJECTSINTERNALSCENE_H
