#ifndef OBJECTSINTERNALSCENE_H
#define OBJECTSINTERNALSCENE_H

#include <QGraphicsScene>
#include <QHash>

#include "objectsceneconstants.h"

namespace Ui {
class ObjectsInternalScene;
}

class ObjectsInternalScene final : public QGraphicsScene
{
    Q_OBJECT

public:
    explicit ObjectsInternalScene(QObject *parent = nullptr);
    ~ObjectsInternalScene();

    // TODO: Удалить после динамического ресайза
    [[deprecated("Необходимо написать динамический ресайз сцены")]]
    void resizeScene(const QSize& iSize);

    QGraphicsItem* getParentOfComplex(QGraphicsItem* pItem);
    bool isNullItem(QGraphicsItem* pItem) const;

    void init();
    void clearScene();

    ObjectSceneConstants::objectId_t addObject(QGraphicsItem* pItem);
    QGraphicsItem* getObject(ObjectSceneConstants::objectId_t objectId);
    [[nodiscard]] QList<ObjectSceneConstants::objectId_t> getAlObjectIds() const;
    void removeObject(ObjectSceneConstants::objectId_t itemId);

    void setBackgroundColor(const QColor& bgrColor);
    void setBorderColor(const QColor& borderColor);

private:
    std::function<ObjectSceneConstants::objectId_t()>       m_idGenerator;              //! Генератор ID для объектов
    QGraphicsItem*              m_pNullItem     {nullptr};  //! Объект, который являет собой пространство сцены (как бы ограниченная плоскость для расположения объектов)
    QHash<ObjectSceneConstants::objectId_t, QGraphicsItem*> m_objectsMap;               //! Словарь для сохранения ID объектов
};

#endif // OBJECTSINTERNALSCENE_H
