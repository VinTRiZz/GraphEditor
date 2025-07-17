#include "itembase.h"

#include <QJsonObject>

#include <boost/core/demangle.hpp>

namespace ObjectViewItems
{

static ObjectViewConstants::objectId_t getSystemId() {
    static ObjectViewConstants::objectId_t currentId {-1};
    return --currentId;
};

ItemBase::ItemBase(QGraphicsItem *parent) :
    QGraphicsItem(parent)
{
    setName(QString("Unchanged name of: \"%0\"").arg(boost::core::demangle(typeid(this).name()).c_str()));
}

void ItemBase::setType(ObjectViewConstants::ObjectType objType)
{
    setData(ObjectViewConstants::OBJECTFIELD_OBJECTTYPE, objType);
}

ObjectViewConstants::ObjectType ItemBase::getType() const
{
    return ObjectViewConstants::ObjectType(data(ObjectViewConstants::OBJECTFIELD_OBJECTTYPE).toInt());
}

void ItemBase::setSystemId()
{
    setObjectId(getSystemId());
}

void ItemBase::setObjectId(ObjectViewConstants::objectId_t id)
{
    setData(ObjectViewConstants::OBJECTFIELD_ID, id);
}

ObjectViewConstants::objectId_t ItemBase::getObjectId() const
{
    if (!data(ObjectViewConstants::OBJECTFIELD_PARENTITEM_ID).isNull()) {
        return data(ObjectViewConstants::OBJECTFIELD_PARENTITEM_ID).toLongLong();
    }
    return data(ObjectViewConstants::OBJECTFIELD_ID).toLongLong();
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
    setData(ObjectViewConstants::OBJECTFIELD_NAME_SHORT, text);
}

QString ItemBase::getShortName() const
{
    return data(ObjectViewConstants::OBJECTFIELD_NAME_SHORT).toString();
}

void ItemBase::setName(const QString &text)
{
    setData(ObjectViewConstants::OBJECTFIELD_NAME, text);
    setToolTip(data(ObjectViewConstants::OBJECTFIELD_NAME).toString());
}

QString ItemBase::getName() const
{
    return data(ObjectViewConstants::OBJECTFIELD_NAME).toString();
}

void ItemBase::setDescription(const QString &text)
{
    setData(ObjectViewConstants::OBJECTFIELD_DESCRIPTION, text);
}

QString ItemBase::getDescription() const
{
    return data(ObjectViewConstants::OBJECTFIELD_DESCRIPTION).toString();
}

void ItemBase::setCustomProperties(const QJsonObject &props)
{
    setData(ObjectViewConstants::OBJECTFIELD_PROPERTY_JSON, props);
}

QJsonObject ItemBase::getCustomProperties() const
{
    return data(ObjectViewConstants::OBJECTFIELD_PROPERTY_JSON).toJsonObject();
}

QRectF ItemBase::boundingRect() const
{
    return m_boundingRect;
}

}
