#include "itembase.h"

#include <QJsonObject>

#include <boost/core/demangle.hpp>

using namespace ObjectViewConstants;

namespace ObjectViewItems
{

static objectId_t getSystemId() {
    static objectId_t currentId {-1};
    return --currentId;
};

ItemBase::ItemBase(QGraphicsItem *parent) :
    QGraphicsItem(parent)
{
    setName(QString("Unchanged name of: \"%0\"").arg(boost::core::demangle(typeid(this).name()).c_str()));
}

void ItemBase::setType(ObjectType objType)
{
    setData(OBJECTFIELD_OBJECTTYPE, objType);
}

ObjectType ItemBase::getType() const
{
    return ObjectType(data(OBJECTFIELD_OBJECTTYPE).toInt());
}

void ItemBase::setSystemId()
{
    setObjectId(getSystemId());
}

void ItemBase::setObjectId(objectId_t id)
{
    setData(OBJECTFIELD_ID, id);
}

objectId_t ItemBase::getObjectId() const
{
    if (!data(OBJECTFIELD_PARENTITEM_ID).isNull()) {
        return data(OBJECTFIELD_PARENTITEM_ID).toLongLong();
    }
    return data(OBJECTFIELD_ID).toLongLong();
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
    setData(OBJECTFIELD_NAME_SHORT, text);
}

QString ItemBase::getShortName() const
{
    return data(OBJECTFIELD_NAME_SHORT).toString();
}

void ItemBase::setName(const QString &text)
{
    setData(OBJECTFIELD_NAME, text);
    setToolTip(data(OBJECTFIELD_NAME).toString());
}

QString ItemBase::getName() const
{
    return data(OBJECTFIELD_NAME).toString();
}

void ItemBase::setDescription(const QString &text)
{
    setData(OBJECTFIELD_DESCRIPTION, text);
}

QString ItemBase::getDescription() const
{
    return data(OBJECTFIELD_DESCRIPTION).toString();
}

void ItemBase::setCustomProperties(const QJsonObject &props)
{
    setData(OBJECTFIELD_PROPERTY_JSON, props);

    if (props.contains(CustomPropertyName::PROPERTY_MAIN_COLOR)) {
        m_mainColor = props.value(CustomPropertyName::PROPERTY_MAIN_COLOR).toString();
    }

    if (props.contains(CustomPropertyName::PROPERTY_BACKGROUND_COLOR)) {
        m_backgroundColor = props.value(CustomPropertyName::PROPERTY_BACKGROUND_COLOR).toString();
    }

    if (props.contains(CustomPropertyName::PROPERTY_SELECTED_COLOR)) {
        m_selectedColor = props.value(CustomPropertyName::PROPERTY_SELECTED_COLOR).toString();
    }
}

QJsonObject ItemBase::getCustomProperties() const
{
    QJsonObject res;
    res[CustomPropertyName::PROPERTY_MAIN_COLOR] = m_mainColor.name();
    res[CustomPropertyName::PROPERTY_BACKGROUND_COLOR] = m_backgroundColor.name();
    res[CustomPropertyName::PROPERTY_SELECTED_COLOR] = m_selectedColor.name();
    return data(OBJECTFIELD_PROPERTY_JSON).toJsonObject();
}

QRectF ItemBase::boundingRect() const
{
    return m_boundingRect;
}

}
