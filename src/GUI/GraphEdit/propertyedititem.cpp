#include "propertyedititem.h"

#include <QVBoxLayout>
#include <QHBoxLayout>

#include <QLabel>
#include <QLineEdit>

#include <QJsonObject>

using namespace ObjectViewItems;

PropertyEditItem::PropertyEditItem(QGraphicsItem* parent) :
    ItemBase(parent)
{
    setName("Property editor");

    setType(ObjectViewConstants::OBJECTTYPE_PROPERTY_EDITOR);

    setFlag(QGraphicsItem::ItemIsMovable, true);

    m_targetLocatorItem = new ArrowedLine(this);
    m_targetLocatorItem->setPen(QPen(Qt::black, 4));

    m_editorForm = new QGraphicsProxyWidget(this);
}

PropertyEditItem::~PropertyEditItem()
{

}

void PropertyEditItem::setPropertyEditor(ObjectPropertyEditorForm *pWidget)
{
    m_editorForm->setWidget(pWidget);
}

void PropertyEditItem::setTargetItem(ItemBase *pTargetItem)
{
    m_pTargetItem = pTargetItem;

    if (nullptr != m_propertyEditorWidget) {
        m_propertyEditorWidget->setTargetItem(pTargetItem);
    }
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
