#pragma once

#include <QDialog>
#include <QAbstractItemModel>

namespace Ui {
class ExtendedComboBoxDialog;
}

namespace QtCustom::Models {
class SearchProxyModel;
class TextSearchEngine;
}

namespace QtCustom::Widgets {

class ExtendedComboBoxDialog : public QDialog
{
    Q_OBJECT

public:
    explicit ExtendedComboBoxDialog(QWidget *parent = nullptr);
    ~ExtendedComboBoxDialog();

    void setSourceModel(QAbstractItemModel* pModel);

    void setSelectedIndex(const QModelIndex& idx);
    QModelIndex getSelectedIndex() const;

    void setDialogModelColumnHidden(int col, bool isColumnHidden = true);
    void setHeaderHidden(bool isHeaderHidden);

private:
    Ui::ExtendedComboBoxDialog *ui;

    QtCustom::Models::TextSearchEngine* m_pSearchEngine {nullptr};
    QtCustom::Models::SearchProxyModel* m_pSearchModel {nullptr};
};

}