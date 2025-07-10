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

    void resizeScene(const QSize& iSize);
    bool isNullItem(QGraphicsItem* pItem) const;

    void setIdGenerator(const std::function<uint()> fGen);
    std::function<uint()> getIdGenerator() const;

    void init();
    void clearScene();

    uint addObject(QGraphicsItem* pItem);
    QGraphicsItem* getObject(uint objectId);
    [[nodiscard]] QList<uint> getAlObjectIds() const;
    void removeObject(uint itemId);

    void setBackgroundColor(const QColor& bgrColor);
    void setBorderColor(const QColor& borderColor);

private:
    std::function<uint()>       m_idGenerator;              //! Генератор ID для объектов
    QGraphicsItem*              m_pNullItem     {nullptr};  //! Объект, который являет собой пространство сцены (как бы ограниченная плоскость для расположения объектов)
    QHash<uint, QGraphicsItem*> m_objectsMap;               //! Словарь для сохранения ID объектов
};

#endif // OBJECTSINTERNALSCENE_H
