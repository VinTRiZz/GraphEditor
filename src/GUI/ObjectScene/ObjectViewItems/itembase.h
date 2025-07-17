#ifndef PREDEFINEDOBJECTBASE_H
#define PREDEFINEDOBJECTBASE_H

#include <QGraphicsItem>

#include "../objectsceneconstants.h"

namespace ObjectViewItems
{

class ItemBase : public QGraphicsItem
{
public:
    explicit ItemBase(QGraphicsItem* parent = nullptr);

    void setType(ObjectSceneConstants::ObjectType objType);
    ObjectSceneConstants::ObjectType getType() const;

    void setSystemId();
    void setObjectId(ObjectSceneConstants::objectId_t id);
    ObjectSceneConstants::objectId_t getObjectId() const;

    virtual void setShortName(const QString& text);
    QString getShortName() const;

    virtual void setName(const QString& text);
    QString getName() const;

    virtual void setDescription(const QString& text);
    QString getDescription() const;

    virtual void setCustomProperties(const QJsonObject& props);
    QJsonObject getCustomProperties() const;

    QRectF boundingRect() const override;

private:
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget*widget) override;

    QRectF m_boundingRect;

protected:
    void setBoundingRect(const QRectF& bRect);
};

}

#endif // PREDEFINEDOBJECTBASE_H
