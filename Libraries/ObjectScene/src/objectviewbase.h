#ifndef OBJECTVIEWBASE_H
#define OBJECTVIEWBASE_H

#include <ObjectItems/Constants.h>
#include <ObjectItems/SceneFieltItem.h>

#include <QGraphicsView>
#include <optional>

#include "objectsinternalscene.h"

class ObjectViewBase : public QGraphicsView {
    // Запрет на использование из-за багов
    using QGraphicsView::setScene;
    using QGraphicsView::setSceneRect;

public:
    explicit ObjectViewBase(QWidget* parent);

    bool getIsInited() const;
    bool isIdAvailable(ObjectViewConstants::objectId_t itemId) const;

    void setGridEnabled(bool isGEnabled);
    bool getIsGridEnabled() const;

    void setGridSize(int gridSizePx);
    int getGridSize() const;

    void setCanvasRect(const QRectF& iRect);

    void addObject(ObjectViewItems::ItemBase* pItem);
    ObjectViewItems::ItemBase* getObject(
        ObjectViewConstants::objectId_t itemId) const;
    QList<ObjectViewItems::ItemBase*> getAllObjects() const;
    QList<ObjectViewConstants::objectId_t> getAllObjectIds() const;
    void removeAllObjects();
    void removeObject(ObjectViewConstants::objectId_t itemId);
    void removeSpecialObjects(ObjectViewConstants::ObjectType objT);

public slots:
    void setSceneBrush(const QBrush& sceneBrush);

    void setCanvasBrush(const QBrush& canvasBrush);
    void setCanvasOpacity(double opac);

    void setGridColor(const QColor& gColor);

private:
    ObjectsInternalScene* m_pScene{
        nullptr};  //! Сцена для отображения объектов (внутренний класс)
    ObjectViewItems::SceneFieldItem* m_pNullItem{
        nullptr};  //! Объект, который являет собой пространство сцены (как бы
                   //! ограниченная плоскость для расположения объектов)
    QHash<ObjectViewConstants::objectId_t, ObjectViewItems::ItemBase*>
        m_objectsMap;  //! Словарь для сохранения ID объектов

protected:
    ObjectsInternalScene* scene() const;
    ObjectViewItems::ItemBase* getParentOfComplex(QGraphicsItem* pItem);
    bool isNullItem(QGraphicsItem* pItem) const;
};

#endif  // OBJECTVIEWBASE_H
