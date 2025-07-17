#ifndef PROPERTYEDITITEM_H
#define PROPERTYEDITITEM_H

#include "itembase.h"
#include "arrowline.h"

#include <QGraphicsProxyWidget>
#include <QWidget>

namespace ObjectViewItems
{

class PropertyEditWidget : public QWidget
{
public:
    PropertyEditWidget(QWidget* parent = nullptr) : QWidget(parent) { }

    virtual void setCustomProperties(const QJsonObject&) = 0;
    virtual QJsonObject getCustomProperties() = 0;
};

class PropertyEditItem : public ItemBase
{
public:
    PropertyEditItem(QGraphicsItem* parent = nullptr);
    ~PropertyEditItem();

    /**
     * @brief setCustomPropertyEditor   Вызывается 1 раз, задаёт виджет для работы с кастом свойствами
     * @param pWidget                   Виджет-редактор кастом свойств. Удалится в деструкторе класса
     */
    void setCustomPropertyEditor(PropertyEditWidget* pWidget);

    /**
     * @brief getCustomPropertyEditor   Получение виджета-редактора кастом свойств.
     * @return                          nullptr если не задан
     */
    PropertyEditWidget* getCustomPropertyEditor() const;

    /**
     * @brief setTargetItem Задать объект, свойства которого нужно редактировать
     * @param pTargetItem   Целевой объект
     */
    void setTargetItem(ItemBase* pTargetItem);

private:
    struct Impl;
    std::shared_ptr<Impl> d;

    ItemBase*   m_pTargetItem {nullptr};
    ArrowedLine*            m_targetLocatorItem {nullptr};
    QGraphicsProxyWidget*   m_editorForm {nullptr};

    QVariant itemChange(GraphicsItemChange change, const QVariant &value) override;
};

}

#endif // PROPERTYEDITITEM_H
