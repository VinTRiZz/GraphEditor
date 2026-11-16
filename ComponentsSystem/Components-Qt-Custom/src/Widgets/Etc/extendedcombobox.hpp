#pragma once

#include <QComboBox>
#include <QToolButton>

namespace Ui {
class ExtendedComboBox;
}

namespace QtCustom::Widgets {

class NullItemProxyModel;
class ExtendedComboBoxDialog;

/**
 * @brief The ExtendedComboBox class Displays extra items (by default, zero-item) and has search button
 */
class ExtendedComboBox : public QWidget
{
    Q_OBJECT
public:
    explicit ExtendedComboBox(QWidget* parent = nullptr);
    ~ExtendedComboBox();

    void setSourceModel(QAbstractItemModel* pModel);
    void setRootIndex(const QModelIndex& idx);
    void setModelColumn(int col);

    void setNullIndexText(const QString& text);
    void setNullIndexData(const QMap<int, QVariant>& idxData);

    void setCurrentIndex(int indexRow);
    int  getCurrentIndex() const;

    // For some cases
    void setDialogModelColumnHidden(int col, bool isColumnHidden = true);
    void setHeaderHidden(bool isHeaderHidden);
    void setButtonHidden(bool isButtonHidden);

private:
    Ui::ExtendedComboBox* ui {nullptr};

    ExtendedComboBoxDialog* m_pDialog {nullptr};
    NullItemProxyModel* m_pComboBoxModel {nullptr};
};

} // namespace QtCustom::Widgets
