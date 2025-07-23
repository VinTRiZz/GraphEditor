#ifndef OBJECTPROPERTYEDITORFORM_H
#define OBJECTPROPERTYEDITORFORM_H

#include <QWidget>

#include <ObjectScene/SceneItems.h>

namespace Ui {
class ObjectPropertyEditorForm;
}

class QLabel;

class ObjectPropertyEditorForm : public QWidget
{
    Q_OBJECT
public:
    explicit ObjectPropertyEditorForm(QWidget *parent = nullptr);
    ~ObjectPropertyEditorForm();

    void setTargetItem(ObjectViewItems::ItemBase* pTargetItem);

public slots:
    void acceptChanges();
    void cancelChanges();

signals:
    void changedItemData();
    void editCanceled();

private:
    Ui::ObjectPropertyEditorForm *ui;

    ObjectViewItems::ItemBase* m_pTargetItem {nullptr};

    void setColor(QLabel* pLabel, const QColor& color);
    QColor getColor(QLabel* pLabel);

    QPixmap pixmapFromPath(const QString& targetPath) const;
};

#endif // OBJECTPROPERTYEDITORFORM_H
