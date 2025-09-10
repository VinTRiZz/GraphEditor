#include "itembase.h"

#include <QJsonObject>
#include <boost/core/demangle.hpp>

#include "scenefielditem.h"

using namespace ObjectViewConstants;

namespace ObjectViewItems {

static objectId_t getSystemId() {
    static objectId_t currentId{-1};
    return --currentId;
};

ItemBase::ItemBase(QGraphicsItem* parent) : QGraphicsItem(parent) {
    setSystemName("Неизвестный тип");
}

ItemBase::~ItemBase() {}

void ItemBase::unregister() {
    if (dynamic_cast<SceneFieldItem*>(parentItem()) != nullptr) {
        static_cast<SceneFieldItem*>(parentItem())->removeRegisteredItem(this);
    }
}

QString ItemBase::getSystemName() const {
    return data(OBJECTFIELD_NAME_SYSTEM).toString();
}

void ItemBase::setType(ObjectType objType) {
    setData(OBJECTFIELD_OBJECTTYPE, objType);
}

ObjectType ItemBase::getType() const {
    return ObjectType(data(OBJECTFIELD_OBJECTTYPE).toInt());
}

void ItemBase::setSystemId() {
    setObjectId(getSystemId());
}

void ItemBase::setObjectId(objectId_t id) {
    setData(OBJECTFIELD_ID, id);

    for (auto pChild : childItems()) {
        registerSubitem(pChild);
    }
}

objectId_t ItemBase::getObjectId() const {
    if (!data(OBJECTFIELD_PARENTITEM_ID).isNull()) {
        return data(OBJECTFIELD_PARENTITEM_ID).toLongLong();
    }
    return data(OBJECTFIELD_ID).toLongLong();
}

void ItemBase::paint([[maybe_unused]] QPainter* painter,
                     [[maybe_unused]] const QStyleOptionGraphicsItem* option,
                     [[maybe_unused]] QWidget* widget) {}

void ItemBase::setBoundingRect(const QRectF& bRect) {
    m_boundingRect = bRect;
}

void ItemBase::setSystemName(const QString& iText) {
    setData(OBJECTFIELD_NAME_SYSTEM, iText);
}

void ItemBase::registerSubitem(QGraphicsItem* pItem) {
    pItem->setData(ObjectViewConstants::OBJECTFIELD_PARENTITEM_ID,
                   getObjectId());
}

void ItemBase::processEvent([[maybe_unused]] ItemBase *pSenderItem, [[maybe_unused]] EventType eventType)
{
    // Must be implemented in child items
}

QVariant ItemBase::itemChange(GraphicsItemChange change, const QVariant &value)
{
    auto res = QGraphicsItem::itemChange(change, value);
    switch (change)
    {
    case GraphicsItemChange::ItemSelectedHasChanged:
        for  (auto& item : m_subscribedItems[EventType::Selection]) {
            processEvent(this, EventType::Selection);
        }
        break;

    case GraphicsItemChange::ItemPositionHasChanged:
        for  (auto& item : m_subscribedItems[EventType::Move]) {
            processEvent(this, EventType::Move);
        }
        break;

    default:
        // Do nothing
        break;
    }
    return res;
}

void ItemBase::setDisplayName(const QString& text) {
    setData(OBJECTFIELD_DISPLAY_NAME, text);
}

QString ItemBase::getDisplayName() const {
    return data(OBJECTFIELD_DISPLAY_NAME).toString();
}

void ItemBase::setDescription(const QString& text) {
    setData(OBJECTFIELD_DESCRIPTION, text);
}

QString ItemBase::getDescription() const {
    return data(OBJECTFIELD_DESCRIPTION).toString();
}

void ItemBase::setBorderColor(const QColor& penColor) {
    setData(OBJECTFIELD_COLOR_BORDER, penColor);
}

QColor ItemBase::getBorderColor() const {
    return QColor(data(OBJECTFIELD_COLOR_BORDER).toString());
}

void ItemBase::setBackgroundColor(const QColor& penColor) {
    setData(OBJECTFIELD_COLOR_BACKGROUND, penColor);
}

QColor ItemBase::getBackgroundColor() const {
    return QColor(data(OBJECTFIELD_COLOR_BACKGROUND).toString());
}

void ItemBase::setSelectionColor(const QColor& penColor) {
    setData(OBJECTFIELD_COLOR_SELECTED, penColor);
}

QColor ItemBase::getSelectionColor() const {
    return QColor(data(OBJECTFIELD_COLOR_SELECTED).toString());
}

QRectF ItemBase::boundingRect() const {
    return m_boundingRect;
}

void ItemBase::subscribeForEvent(ItemBase *pItem, EventType etype)
{
    if (etype == EventType::All) {
        m_subscribedItems[EventType::Move].push_back(pItem);
        m_subscribedItems[EventType::Selection].push_back(pItem);

        // TODO: Сюда добавлять новые ивенты обязательно
        return;
    }

    m_subscribedItems[etype].push_back(pItem);
}

void ItemBase::unsbscribeFromEvent(ItemBase *pItem, EventType etype)
{
    if (etype == EventType::All) {
        unsbscribeFromEvent(pItem, EventType::Move);
        unsbscribeFromEvent(pItem, EventType::Selection);

        // TODO: Сюда добавлять новые ивенты обязательно
        return;
    }

    auto& targetList = m_subscribedItems[etype];
    auto targetIt = std::find(targetList.begin(), targetList.end(), pItem);
    if (targetIt != targetList.end()) {
        targetList.erase(targetIt);
    }
}

std::list<ItemBase *> ItemBase::getSubscribed(EventType etype)
{
    if (etype == EventType::All) {
        auto& res = m_subscribedItems[EventType::Move];

        auto& res2 = m_subscribedItems[EventType::Selection];
        std::copy(res2.begin(), res2.end(), std::back_inserter(res));

        // TODO: Сюда добавлять новые ивенты обязательно
        return res;
    }
    return m_subscribedItems[etype];
}

}  // namespace ObjectViewItems
