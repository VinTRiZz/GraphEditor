#ifndef OBJECTSINTERNALSCENE_H
#define OBJECTSINTERNALSCENE_H

#include <QGraphicsScene>
#include <QHash>

namespace Ui {
class ObjectsInternalScene;
}

class ObjectsInternalScene final : public QGraphicsScene
{
    Q_OBJECT

public:
    explicit ObjectsInternalScene(QObject *parent = nullptr);
    ~ObjectsInternalScene();

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
     * @brief clearScene Очищает сцену от ВСЕХ элементов, кроме полотна
     */
    void clearScene();

    /**
     * @brief addObject Добавить объект на сцену. Координатами считаются координаты, заданные в pItem
     * @param pItem Указатель на объект сцены. Его parentItem замещается nullItem сцены
     * @return ID добавленного объекта, сгенерированный с помощью функтора-генератора
     */
    uint addObject(QGraphicsItem* pItem);

    /**
     * @brief getObject Получить объект по его ID
     * @param objectId  ID объекта
     * @return          Указатель на объект или nullptr если такого нет
     */
    QGraphicsItem* getObject(uint objectId);

    /**
     * @brief getAlObjectIds Возвращает все ID объектов, добавленных в сцену
     * @return Список идентификаторов добавленных объектов
     */
    [[nodiscard]] QList<uint> getAlObjectIds() const;

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
    QHash<uint, QGraphicsItem*> m_objectsMap;                   //! Словарь для сохранения ID объектов
};

#endif // OBJECTSINTERNALSCENE_H
