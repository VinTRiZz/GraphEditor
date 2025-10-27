#ifndef OBJECTPROPERTYEDITORFORM_H
#define OBJECTPROPERTYEDITORFORM_H

#include <Components/CustomQt/ObjectView/ObjectItems.h>

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

    void setTargetItem(ObjectItems::BasicItem* pTargetItem);

public slots:
    void acceptChanges();
    void cancelChanges();

signals:
    void changedItemData();
    void editCanceled();

private:
    Ui::ObjectPropertyEditorForm* ui;

    ObjectItems::BasicItem* m_pTargetItem{nullptr};

    QWidget* m_selectedIconLabel{nullptr};
    void initHistoryGalery();
    void initIcons();

    void addHistoryImage(const QString& targetPath);
    void selectImage(const QString& imageHash);
};

#endif  // OBJECTPROPERTYEDITORFORM_H
