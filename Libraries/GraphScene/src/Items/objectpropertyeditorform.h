#ifndef OBJECTPROPERTYEDITORFORM_H
#define OBJECTPROPERTYEDITORFORM_H

#include <ObjectItems/ItemBase.h>

#include <QWidget>

namespace Ui {
class ObjectPropertyEditorForm;
}

class QLabel;

class ObjectPropertyEditorForm : public QWidget {
    Q_OBJECT
public:
    explicit ObjectPropertyEditorForm(QWidget* parent = nullptr);
    ~ObjectPropertyEditorForm();

    void setTargetItem(ObjectViewItems::ItemBase* pTargetItem);

public slots:
    void acceptChanges();
    void cancelChanges();

signals:
    void changedItemData();
    void editCanceled();

private:
    Ui::ObjectPropertyEditorForm* ui;

    ObjectViewItems::ItemBase* m_pTargetItem{nullptr};
};

#endif  // OBJECTPROPERTYEDITORFORM_H
