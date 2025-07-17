#include "itembase.h"

#include <QJsonObject>

#include <boost/core/demangle.hpp>

namespace ObjectViewItems
{

static ObjectSceneConstants::objectId_t getSystemId() {
    static ObjectSceneConstants::objectId_t currentId {-1};
    return --currentId;
};

ItemBase::ItemBase(QGraphicsItem *parent) :
    QGraphicsItem(parent)
{
    setName(QString("Unchanged name of: \"%0\"").arg(boost::core::demangle(typeid(this).name()).c_str()));
}

void ItemBase::setType(ObjectSceneConstants::ObjectType objType)
{
    setData(ObjectSceneConstants::OBJECTFIELD_OBJECTTYPE, objType);
}

ObjectSceneConstants::ObjectType ItemBase::getType() const
{
    return ObjectSceneConstants::ObjectType(data(ObjectSceneConstants::OBJECTFIELD_OBJECTTYPE).toInt());
}

void ItemBase::setSystemId()
{
    setObjectId(getSystemId());
}

void ItemBase::setObjectId(ObjectSceneConstants::objectId_t id)
{
    setData(ObjectSceneConstants::OBJECTFIELD_ID, id);
}

ObjectSceneConstants::objectId_t ItemBase::getObjectId() const
{
    if (!data(ObjectSceneConstants::OBJECTFIELD_PARENTITEM_ID).isNull()) {
        return data(ObjectSceneConstants::OBJECTFIELD_PARENTITEM_ID).toLongLong();
    }
    return data(ObjectSceneConstants::OBJECTFIELD_ID).toLongLong();
}

void ItemBase::paint([[maybe_unused]] QPainter *painter, [[maybe_unused]] const QStyleOptionGraphicsItem *option, [[maybe_unused]] QWidget *widget)
{

}

void ItemBase::setBoundingRect(const QRectF &bRect)
{
    m_boundingRect = bRect;
}

void ItemBase::setShortName(const QString &text)
{
    setData(ObjectSceneConstants::OBJECTFIELD_NAME_SHORT, text);
}

QString ItemBase::getShortName() const
{
    return data(ObjectSceneConstants::OBJECTFIELD_NAME_SHORT).toString();
}

void ItemBase::setName(const QString &text)
{
    setData(ObjectSceneConstants::OBJECTFIELD_NAME, text);
    setToolTip(data(ObjectSceneConstants::OBJECTFIELD_NAME).toString());
}

QString ItemBase::getName() const
{
    return data(ObjectSceneConstants::OBJECTFIELD_NAME).toString();
}

void ItemBase::setDescription(const QString &text)
{
    setData(ObjectSceneConstants::OBJECTFIELD_DESCRIPTION, text);
}

QString ItemBase::getDescription() const
{
    return data(ObjectSceneConstants::OBJECTFIELD_DESCRIPTION).toString();
}

void ItemBase::setCustomProperties(const QJsonObject &props)
{
    setData(ObjectSceneConstants::OBJECTFIELD_PROPERTY_JSON, props);
}

QJsonObject ItemBase::getCustomProperties() const
{
    return data(ObjectSceneConstants::OBJECTFIELD_PROPERTY_JSON).toJsonObject();
}

QRectF ItemBase::boundingRect() const
{
    return m_boundingRect;
}

}
