#ifndef PREDEFINEDOBJECTBASE_H
#define PREDEFINEDOBJECTBASE_H

#include <QBrush>
#include <QGraphicsItem>
#include <QPen>

#include "constants.h"

namespace ObjectViewItems {

/**
 * @brief The ItemBase class Основа для каждого класса объекта на сцене
 */
class ItemBase : public QGraphicsItem {
public:
    explicit ItemBase(QGraphicsItem* parent = nullptr);
    ~ItemBase();

    void unregister();
    QString getSystemName() const;

    void setType(ObjectViewConstants::ObjectType objType);
    ObjectViewConstants::ObjectType getType() const;

    void setSystemId();
    virtual void setObjectId(ObjectViewConstants::objectId_t id);
    virtual ObjectViewConstants::objectId_t getObjectId() const;

    virtual void setDisplayName(const QString& text);
    virtual QString getDisplayName() const;

    virtual void setDescription(const QString& text);
    virtual QString getDescription() const;

    virtual void setBorderColor(const QColor& penColor);
    virtual QColor getBorderColor() const;

    virtual void setBackgroundColor(const QColor& penColor);
    virtual QColor getBackgroundColor() const;

    virtual void setSelectionColor(const QColor& penColor);
    virtual QColor getSelectionColor() const;

    QRectF boundingRect() const override;

    enum class EventType : int {
        All,
        Move,
        Selection,
    };

    void subscribeForEvent(ItemBase* pItem, EventType etype);
    void unsbscribeFromEvent(ItemBase* pItem, EventType etype);
    std::list<ItemBase*> getSubscribed(EventType etype);

private:
    QRectF m_boundingRect;
    std::map<EventType, std::list<ItemBase*> > m_subscribedItems;

protected:
    void paint(QPainter* painter, const QStyleOptionGraphicsItem* option,
               QWidget* widget) override;
    void setBoundingRect(const QRectF& bRect);

    void setSystemName(const QString& iText);

    void registerSubitem(QGraphicsItem* pItem);

    virtual void processEvent(ItemBase* pSenderItem, EventType eventType);

    // QGraphicsItem interface
    virtual QVariant itemChange(GraphicsItemChange change, const QVariant &value) override;
};

}  // namespace ObjectViewItems

#endif  // PREDEFINEDOBJECTBASE_H
