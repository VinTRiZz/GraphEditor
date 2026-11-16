#include "tablesourcemodel.hpp"

#include <QColor>

namespace QtCustom::Orm {

TableSourceModel::TableSourceModel(QObject *parent)
    : QAbstractTableModel{parent}
{

}

void TableSourceModel::setTable(TableItemMaster *pTable)
{
    beginResetModel();
    if (m_pTable) {
        disconnect(m_pTable, nullptr, this, nullptr);
    }
    m_pTable = pTable;
    if (m_pTable) {
        connect(m_pTable, &TableItemMaster::sig_recordsLoadStarted,
                this, &TableSourceModel::beginResetModel);
        connect(m_pTable, &TableItemMaster::sig_recordsLoaded,
                this, &TableSourceModel::endResetModel);

        connect(m_pTable, &TableItemMaster::sig_itemAboutToBeAdded,
                this, [this](const auto& addedHdl){
                    int insertionRow {0};
                    auto items = m_pTable->findItem([&insertionRow, addedHdl](const auto& hdl) -> bool {
                        auto res = (hdl->getId().toInt() < addedHdl->getId().toInt());
                        insertionRow += res;
                        return res;
                    });
                    beginInsertRows(QModelIndex(), insertionRow, insertionRow);
                });
        connect(m_pTable, &TableItemMaster::sig_itemAdded,
                this, &TableSourceModel::endInsertRows);

        connect(m_pTable, &TableItemMaster::sig_itemUpdated,
                this, [this](const auto& hdl){
                    auto row = m_pTable->getItemRow(hdl);
                    emit dataChanged(createIndex(row, 0), createIndex(row, columnCount() - 1));
                });

        connect(m_pTable, &TableItemMaster::sig_itemAboutToBeRemoved,
                this, [this](const auto& hdl){
                    auto row = m_pTable->getItemRow(hdl);
                    beginRemoveRows(QModelIndex(), row, row);
                });
        connect(m_pTable, &TableItemMaster::sig_itemRemoved,
                this, &TableSourceModel::endRemoveRows);
    }
    endResetModel();
}

TableItemMaster *TableSourceModel::getTable() const
{
    return m_pTable;
}

QVariant TableSourceModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if (orientation != Qt::Horizontal) {
        return {};
    }

    if (role == Qt::DisplayRole) {
        switch (section)
        {
        case C_id:  return "ID";
        }
        return {};
    }
    return QAbstractTableModel::headerData(section, orientation, role);
}

int TableSourceModel::rowCount(const QModelIndex &parent) const
{
    if (parent.isValid() || !m_pTable) {
        return 0;
    }
    return m_pTable->getAllItems()->size();
}

int TableSourceModel::columnCount(const QModelIndex &parent) const
{
    if (!parent.isValid())
        return 0;
    return C_SYS_ColumnCount;
}

QVariant TableSourceModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid() || index.row() >= rowCount() || index.row() < 0)
        return QVariant();

    auto item = getItem(index);
    return getItemData(item, index.column(), role);
}

Qt::ItemFlags TableSourceModel::flags(const QModelIndex &index) const
{
    auto item = getItem(index);
    return getItemFlags(item, index.column());
}

ItemHandler TableSourceModel::getItem(const QModelIndex &idx) const
{
    if (!m_pTable) { return {}; }
    return m_pTable->getItemByRow(idx.row());
}

QModelIndex TableSourceModel::getIndex(const ItemHandler &hdl) const
{
    if (!m_pTable || !hdl || (hdl->getTableName() != m_pTable->getTable())) { return {}; }
    return createIndex(m_pTable->getItemRow(hdl), 0, nullptr);
}

QModelIndex TableSourceModel::getIndexFromId(const QVariant &hdlId) const
{
    if (!m_pTable) { return {}; }
    return getIndex(m_pTable->getItem(hdlId));
}

QVariant TableSourceModel::getItemData(const ItemHandler &item, int column, int role) const
{
    if (role == Qt::DisplayRole || role == Qt::EditRole) {
        switch (column)
        {
        case C_id:
            return item->getId();
        }
        return {};
    }

    if (role == Qt::ToolTipRole) {
        switch (column)
        {
        case C_id:
            return "Item ID";
        }
        return {};
    }

    if (role == Qt::BackgroundRole) {
        if (!item) {
            return QColor(90, 90, 90, 100);
        }
        if (item->getId().isNull()) {
            return QColor(240, 120, 150, 200);
        }
        return {};
    }

    if (role > Qt::UserRole) {
        if (role == R_id) {
            return item ? item->getId() : QVariant();
        }
        return {};
    }

    return {};
}

Qt::ItemFlags TableSourceModel::getItemFlags(const ItemHandler &item, int column) const
{
    return Qt::ItemFlags(Qt::ItemIsSelectable |
                         Qt::ItemIsEnabled |
                         ~Qt::ItemIsEditable);
}

}