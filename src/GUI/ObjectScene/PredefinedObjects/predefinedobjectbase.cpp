#include "predefinedobjectbase.h"

#include <QJsonObject>

namespace PredefinedObjects
{

PredefinedObjectBase::PredefinedObjectBase(QGraphicsItem *parent) :
    QGraphicsItem(parent)
{

}

void PredefinedObjectBase::setType(ObjectSceneConstants::ObjectType objType)
{
    setData(ObjectSceneConstants::OBJECTFIELD_OBJECTTYPE, objType);
}

ObjectSceneConstants::ObjectType PredefinedObjectBase::getType() const
{
    return ObjectSceneConstants::ObjectType(data(ObjectSceneConstants::OBJECTFIELD_OBJECTTYPE).toInt());
}

ObjectSceneConstants::objectId_t PredefinedObjectBase::getObjectId() const
{
    if (!data(ObjectSceneConstants::OBJECTFIELD_PARENTITEM_ID).isNull()) {
        return data(ObjectSceneConstants::OBJECTFIELD_PARENTITEM_ID).toUInt();
    }
    return data(ObjectSceneConstants::OBJECTFIELD_ID).toUInt();
}

void PredefinedObjectBase::paint([[maybe_unused]] QPainter *painter, [[maybe_unused]] const QStyleOptionGraphicsItem *option, [[maybe_unused]] QWidget *widget)
{

}

void PredefinedObjectBase::setBoundingRect(const QRectF &bRect)
{
    m_boundingRect = bRect;
}

void PredefinedObjectBase::setShortName(const QString &text)
{
    setData(ObjectSceneConstants::OBJECTFIELD_NAME_SHORT, text);
}

QString PredefinedObjectBase::getShortName() const
{
    return data(ObjectSceneConstants::OBJECTFIELD_NAME_SHORT).toString();
}

void PredefinedObjectBase::setName(const QString &text)
{
    setData(ObjectSceneConstants::OBJECTFIELD_NAME, text);
    setToolTip(data(ObjectSceneConstants::OBJECTFIELD_NAME).toString());
}

QString PredefinedObjectBase::getName() const
{
    return data(ObjectSceneConstants::OBJECTFIELD_NAME).toString();
}

void PredefinedObjectBase::setDescription(const QString &text)
{
    setData(ObjectSceneConstants::OBJECTFIELD_DESCRIPTION, text);
}

QString PredefinedObjectBase::getDescription() const
{
    return data(ObjectSceneConstants::OBJECTFIELD_DESCRIPTION).toString();
}

void PredefinedObjectBase::setCustomProperties(const QJsonObject &props)
{
    setData(ObjectSceneConstants::OBJECTFIELD_PROPERTY_JSON, props);
}

QJsonObject PredefinedObjectBase::getCustomProperties() const
{
    return data(ObjectSceneConstants::OBJECTFIELD_PROPERTY_JSON).toJsonObject();
}

QRectF PredefinedObjectBase::boundingRect() const
{
    return m_boundingRect;
}

}
