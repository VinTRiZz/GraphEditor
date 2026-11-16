#include "extendedcomboboxdialog.hpp"
#include "ui_extendedcomboboxdialog.h"

#include <Components/CustomQt/Models/SearchProxyModel.h>

namespace QtCustom::Widgets {

ExtendedComboBoxDialog::ExtendedComboBoxDialog(QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::ExtendedComboBoxDialog)
{
    ui->setupUi(this);

    m_pSearchEngine = new QtCustom::Models::TextSearchEngine(this);

    m_pSearchModel = new QtCustom::Models::SearchProxyModel(this);
    m_pSearchModel->setEngine(m_pSearchEngine);
    ui->tableView->setModel(m_pSearchModel);

    connect(ui->tableView, &QTableView::doubleClicked,
            this, &QDialog::accept);

    connect(ui->lineEditSearch, &QLineEdit::textChanged,
            this, [this](const auto& textChange){
        m_pSearchEngine->setTarget(textChange);
    });

    ui->tableView->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    ui->tableView->verticalHeader()->hide();
}

ExtendedComboBoxDialog::~ExtendedComboBoxDialog()
{
    delete ui;
}

void ExtendedComboBoxDialog::setSourceModel(QAbstractItemModel *pModel)
{
    m_pSearchModel->setSourceModel(pModel);
}

void ExtendedComboBoxDialog::setSelectedIndex(const QModelIndex &idx)
{
    ui->tableView->setCurrentIndex(m_pSearchModel->mapFromSource(idx));
}

QModelIndex ExtendedComboBoxDialog::getSelectedIndex() const
{
    return m_pSearchModel->mapFromSource(ui->tableView->currentIndex());
}

void ExtendedComboBoxDialog::setDialogModelColumnHidden(int col, bool isColumnHidden)
{
    ui->tableView->setColumnHidden(col, isColumnHidden);
}

void ExtendedComboBoxDialog::setHeaderHidden(bool isHeaderHidden)
{
    ui->tableView->horizontalHeader()->setHidden(isHeaderHidden);
}

}