#ifndef OBJECTSINTERNALSCENE_H
#define OBJECTSINTERNALSCENE_H

#include <QGraphicsScene>
#include <QHash>

#include "predefinedobjects.h"
#include "objectsceneconstants.h"

namespace Ui {
class ObjectsInternalScene;
}

class ObjectsInternalScene final : public QGraphicsScene
{
    Q_OBJECT

    // Запреты чтобы не сломать внутреннюю логику сцены
    using QGraphicsScene::addItem;
    using QGraphicsScene::addEllipse;
    using QGraphicsScene::addRect;
    using QGraphicsScene::addLine;
    using QGraphicsScene::addPath;
    using QGraphicsScene::addPixmap;
    using QGraphicsScene::addPolygon;
    using QGraphicsScene::addSimpleText;
    using QGraphicsScene::addText;
    using QGraphicsScene::addWidget;

public:
    explicit ObjectsInternalScene(QObject *parent = nullptr);
    ~ObjectsInternalScene();

    // TODO: Удалить после динамического ресайза
    [[deprecated("Необходимо написать динамический ресайз сцены")]]
    void resizeScene(const QSize& iSize);

    ObjectViewItems::ItemBase* getParentOfComplex(QGraphicsItem *pItem);
    bool isNullItem(QGraphicsItem *pItem) const;

    void init();
    void clearScene();
    void removeSpecialObjects(ObjectViewConstants::ObjectType objT);

    void addObject(ObjectViewItems::ItemBase* pItem);
    ObjectViewItems::ItemBase* getObject(ObjectViewConstants::objectId_t objectId);
    QList<ObjectViewItems::ItemBase*> getAllObjects() const;
    [[nodiscard]] QList<ObjectViewConstants::objectId_t> getAllObjectIds() const;
    void removeObject(ObjectViewConstants::objectId_t itemId);

    void setBackgroundColor(const QColor& bgrColor);
    void setBorderColor(const QColor& borderColor);

private:
    ObjectViewItems::DynamicAreaItem* m_pNullItem {nullptr};  //! Объект, который являет собой пространство сцены (как бы ограниченная плоскость для расположения объектов)
    QHash<ObjectViewConstants::objectId_t, ObjectViewItems::ItemBase*> m_objectsMap; //! Словарь для сохранения ID объектов
};

#endif // OBJECTSINTERNALSCENE_H
