#pragma once

#include <QAbstractTableModel>

#include <Components/CustomQt/Orm/TableItemMaster.h>

namespace QtCustom::Orm {

/**
 * @brief The TableSourceModel class Base for all TableItemMaster item models
 */
class TableSourceModel : public QAbstractTableModel
{
    Q_OBJECT
public:
    explicit TableSourceModel(QObject *parent = nullptr);

    // Common columns
    enum Columns : int
    {
        C_id = 0,

        C_SYS_ColumnCount, // DO NOT EDIT
    };

    // Common roles data
    enum Roles : int
    {
        R_id = Qt::UserRole + 1,

        R_SYS_RoleCount, // DO NOT EDIT
    };

    void setTable(TableItemMaster* pTable);
    TableItemMaster* getTable() const;

    // QAbstractTableModel interface
    QVariant headerData(int section, Qt::Orientation orientation, int role) const;
    int rowCount(const QModelIndex &parent = {}) const;
    int columnCount(const QModelIndex &parent = {}) const;
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const;
    Qt::ItemFlags flags(const QModelIndex &index) const;

    ItemHandler getItem(const QModelIndex& idx) const;
    QModelIndex getIndex(const ItemHandler& hdl) const;
    QModelIndex getIndexFromId(const QVariant& hdlId) const;

private:
    TableItemMaster* m_pTable {nullptr};

    mutable std::vector<ItemHandler> m_cache_handlersBeforeRefilter; // used to determine what moved where

protected:
    virtual QVariant getItemData(const ItemHandler& item, int column, int role) const;
    virtual Qt::ItemFlags getItemFlags(const ItemHandler& item, int column) const;
};

}