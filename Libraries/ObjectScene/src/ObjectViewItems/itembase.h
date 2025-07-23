#ifndef PREDEFINEDOBJECTBASE_H
#define PREDEFINEDOBJECTBASE_H

#include <QGraphicsItem>

#include "../constants.h"

#include <QBrush>
#include <QPen>

namespace ObjectViewItems
{

/**
 * @brief The ItemBase class Основа для каждого класса объекта на сцене
 */
class ItemBase : public QGraphicsItem
{
public:
    explicit ItemBase(QGraphicsItem* parent = nullptr);
    ~ItemBase();

    void unregister();

    void setType(ObjectViewConstants::ObjectType objType);
    ObjectViewConstants::ObjectType getType() const;

    void setSystemId();
    virtual void setObjectId(ObjectViewConstants::objectId_t id);
    virtual ObjectViewConstants::objectId_t getObjectId() const;

    virtual void setShortName(const QString& text);
    virtual QString getShortName() const;

    virtual void setName(const QString& text);
    virtual QString getName() const;

    virtual void setDescription(const QString& text);
    virtual QString getDescription() const;

    virtual void setCustomProperties(const QJsonObject& props);
    virtual QJsonObject getCustomProperties() const;

    virtual void setMainColor(const QColor &penColor);
    virtual QColor getMainColor() const;

    virtual void setBackgroundColor(const QColor &penColor);
    virtual QColor getBackgroundColor() const;

    virtual void setSelectedColor(const QColor &penColor);
    virtual QColor getSelectedColor() const;

    QRectF boundingRect() const override;

private:
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget*widget) override;

    QRectF m_boundingRect;

protected:
    void setBoundingRect(const QRectF& bRect);
};

}

#endif // PREDEFINEDOBJECTBASE_H
