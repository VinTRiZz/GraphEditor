#ifndef PROPERTYEDITITEM_H
#define PROPERTYEDITITEM_H

#include "GUI/ObjectScene/ObjectViewItems/itembase.h"
#include "GUI/ObjectScene/ObjectViewItems/arrowline.h"

#include "objectpropertyeditorform.h"

#include <QGraphicsProxyWidget>
#include <QWidget>

class PropertyEditItem : public ObjectViewItems::ItemBase
{
public:
    PropertyEditItem(QGraphicsItem* parent = nullptr);
    ~PropertyEditItem();

    /**
     * @brief setPropertyEditor Задать редактор свойств
     * @param pWidget           Виджет редактора свойств
     */
    void setPropertyEditor(ObjectPropertyEditorForm* pWidget);

    /**
     * @brief setTargetItem Задать объект, свойства которого нужно редактировать
     * @param pTargetItem   Целевой объект
     */
    void setTargetItem(ItemBase* pTargetItem);

private:
    ItemBase*                       m_pTargetItem {nullptr};
    ObjectViewItems::ArrowedLine*   m_targetLocatorItem {nullptr};

    ObjectPropertyEditorForm*   m_propertyEditorWidget {nullptr};
    QGraphicsProxyWidget*       m_editorForm {nullptr};

    QVariant itemChange(GraphicsItemChange change, const QVariant &value) override;
};


#endif // PROPERTYEDITITEM_H
