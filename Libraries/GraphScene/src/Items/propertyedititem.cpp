#include "propertyedititem.h"

#include <QHBoxLayout>
#include <QJsonObject>
#include <QLabel>
#include <QLineEdit>
#include <QVBoxLayout>

using namespace ObjectViewItems;

PropertyEditItem::PropertyEditItem(QGraphicsItem* parent) : ItemBase(parent) {
    setSystemName("Редактор свойств");
    setType(ObjectViewConstants::OBJECTTYPE_PROPERTY_EDITOR);

    setFlag(QGraphicsItem::ItemIsMovable, true);
    m_editorForm = new QGraphicsProxyWidget(this);
    m_editorForm->setFlag(QGraphicsItem::ItemIgnoresTransformations, true);
    m_editorForm->setScale(0.7);
}

PropertyEditItem::~PropertyEditItem() {}

void PropertyEditItem::setPropertyEditor(ObjectPropertyEditorForm* pWidget) {
    m_editorForm->setWidget(pWidget);
    m_propertyEditorWidget = pWidget;

    QObject::connect(pWidget, &ObjectPropertyEditorForm::changedItemData,
                     pWidget, [this]() { hide(); });

    QObject::connect(pWidget, &ObjectPropertyEditorForm::editCanceled, pWidget,
                     [this]() { hide(); });
}

QRect PropertyEditItem::getEditorRect() const {
    return m_editorForm->widget()->rect();
}

void PropertyEditItem::setTargetItem(ItemBase* pTargetItem) {
    m_pTargetItem = pTargetItem;

    if (nullptr != m_propertyEditorWidget) {
        m_propertyEditorWidget->setTargetItem(pTargetItem);
    }
}
