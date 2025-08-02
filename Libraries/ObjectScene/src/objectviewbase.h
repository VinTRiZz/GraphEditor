#ifndef OBJECTVIEWBASE_H
#define OBJECTVIEWBASE_H

#include <optional>

#include <ObjectItems/Constants.h>
#include <ObjectItems/SceneFieltItem.h>
#include "objectsinternalscene.h"

#include <QGraphicsView>

class ObjectViewBase : public QGraphicsView
{
public:
    explicit ObjectViewBase(QWidget *parent);

    bool getIsInited() const;
    bool isIdAvailable(ObjectViewConstants::objectId_t itemId) const;

    void setSceneBrush(const QBrush& sceneBrush);
    void setSceneRect(const QRectF& iRect);
    void setCanvasRect(const QRectF& iRect);

    void addObject(ObjectViewItems::ItemBase *pItem);
    ObjectViewItems::ItemBase* getObject(ObjectViewConstants::objectId_t itemId) const;
    QList<ObjectViewItems::ItemBase *> getAllObjects() const;
    QList<ObjectViewConstants::objectId_t> getAllObjectIds() const;
    void removeAllObjects();
    void removeObject(ObjectViewConstants::objectId_t itemId);
    void removeSpecialObjects(ObjectViewConstants::ObjectType objT);

private:
    ObjectsInternalScene* m_pScene  {nullptr};  //! Сцена для отображения объектов (внутренний класс)
    ObjectViewItems::SceneFieldItem* m_pNullItem {nullptr}; //! Объект, который являет собой пространство сцены (как бы ограниченная плоскость для расположения объектов)
    QHash<ObjectViewConstants::objectId_t, ObjectViewItems::ItemBase*>  m_objectsMap; //! Словарь для сохранения ID объектов

protected:
    using QGraphicsView::scene;
    using QGraphicsView::setScene;

    ObjectViewItems::ItemBase* getParentOfComplex(QGraphicsItem *pItem);
    bool isNullItem(QGraphicsItem *pItem) const;
};

#endif // OBJECTVIEWBASE_H
