#include "propertyedititem.h"

#include <QVBoxLayout>
#include <QHBoxLayout>

#include <QLabel>
#include <QLineEdit>

#include <QJsonObject>

namespace ObjectViewItems
{

struct PropertyEditItem::Impl
{
    QWidget hostWidget;
    QLabel  nameLabel;
    QLabel  shortNameLabel;
    QLabel  descriptionLabel;

    PropertyEditWidget* customPropertyWidget {nullptr};
};

PropertyEditItem::PropertyEditItem(QGraphicsItem* parent) :
    ItemBase(parent),
    d {new Impl}
{
    setName("Property editor");

    setType(ObjectSceneConstants::OBJECTTYPE_PROPERTY_EDITOR);

    setFlag(QGraphicsItem::ItemIsMovable, true);

    m_targetLocatorItem = new ArrowedLine(this);
    m_targetLocatorItem->setPen(QPen(Qt::black, 4));

    m_editorForm = new QGraphicsProxyWidget(this);
    m_editorForm->setWidget(&d->hostWidget);
}

PropertyEditItem::~PropertyEditItem()
{

}

void PropertyEditItem::setCustomPropertyEditor(PropertyEditWidget *pWidget)
{
    if (nullptr != d->customPropertyWidget) {
        throw std::runtime_error("Can not set property editor if already set one");
    }
    d->customPropertyWidget = pWidget;
    d->customPropertyWidget->setParent(&d->hostWidget);
}

PropertyEditWidget *PropertyEditItem::getCustomPropertyEditor() const
{
    return d->customPropertyWidget;
}

void PropertyEditItem::setTargetItem(ItemBase *pTargetItem)
{
    m_pTargetItem = pTargetItem;
    d->customPropertyWidget->setCustomProperties(m_pTargetItem->getCustomProperties());
}

QVariant PropertyEditItem::itemChange(GraphicsItemChange change, const QVariant &value)
{
    if (change == ItemPositionChange) {
        if (value.toBool()) {
            auto targetLine = QLineF(pos(), m_pTargetItem->boundingRect().center());
            m_targetLocatorItem->setLine(targetLine.p1(), targetLine.center());
        } else {
            m_targetLocatorItem->hide();
        }
    }

    return QGraphicsItem::itemChange(change, value);
}

}
