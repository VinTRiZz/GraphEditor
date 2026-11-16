#include "tableitemmaster.hpp"

#include <Components/Logger/Logger.h>

#include <QSqlDatabase>
#include <QSqlQuery>
#include <QSqlRecord>
#include <QSqlResult>
#include <QSqlError>
#include <QVariant>

namespace QtCustom::Orm {

struct TableItemMaster::Impl
{
    // System
    QSqlDatabase    m_db;
    QString         m_tableName {"[ UNKNOWN TABLE ]"};
    QString         m_recordsFilter;

    // Item handling
    std::unordered_set<ItemHandler> m_temporaryItems;
    ItemContainer m_tableRows;
    ItemContainer m_filteredRows;
    selector_t  m_filter;
    RowsHandler m_dummyRowHandler { &m_filteredRows }; // For getters
};

TableItemMaster::TableItemMaster(const QSqlDatabase& db, QObject *parent) :
    QObject{parent},
    d {new Impl{.m_db = db}}
{

}

TableItemMaster::~TableItemMaster()
{
    d->m_dummyRowHandler.invalidate();
}

QSqlDatabase TableItemMaster::getDatabase() const
{
    return d->m_db;
}

bool TableItemMaster::createTable()
{
    // Must be implemented in inheritors
    return false;
}

bool TableItemMaster::isTableExist() const
{
    auto q = QSqlQuery(d->m_db);
    if (!executeQuery(q, QString("PRAGMA table_info(\"%1\")").arg(d->m_tableName))) {
        return false;
    }
    return q.next();
}

bool TableItemMaster::loadRecords()
{
    emit sig_recordsLoadStarted();
    resetRows();
    if (d->m_tableName.isEmpty()) {
        m_error.setCode(ExtraClasses::ErrorCode::DatabaseInvalidTable);
        m_error.setDetailText("Table name not set");
        COMPLOG_ERROR("TableItemMaster (", getTable().toStdString(), ") : failed to load records:", QString(getError().what()).toStdString());
        emit sig_recordsLoaded();
        return false;
    }

    auto q = QSqlQuery(d->m_db);
    if (!executeQuery(q, QString("SELECT * FROM %1 %2").arg(getTable(), QString(d->m_recordsFilter.isEmpty() ? "" : "WHERE " + d->m_recordsFilter)))) {
        emit sig_recordsLoaded();
        return false;
    }

    while (q.next()) {
        auto rec = q.record();
        RowValues row;
        row.reserve(rec.count());
        for (int i = 0; i < rec.count(); ++i) {
            row[rec.fieldName(i)] = rec.value(i);
        }

        auto itemHdl = createItem();
        connect(itemHdl.get(), &TableItemBase::sig_dataChanged,
                this, [this, itemHdl](){ processItemUpdate(itemHdl); });
        itemHdl->setRecord(row);
        d->m_tableRows.insert(itemHdl);
    }

    slot_updateFilter();

    COMPLOG_INFO("[", getTable().toStdString(), "] loaded objects:", d->m_tableRows.size(), "filtered to:", d->m_filteredRows.size(), ")");
    emit sig_recordsLoaded();
    return true;
}

void TableItemMaster::setDatabaseFilter(const QString &recordsFilter)
{
    d->m_recordsFilter = recordsFilter;
    emit sig_databaseFilterChanged();
}

void TableItemMaster::setFilter(selector_t &&selector)
{
    d->m_filter = selector;
    slot_updateFilter();
}

void TableItemMaster::setTable(const QString &tableName)
{
    emit sig_recordsLoadStarted();
    resetRows();
    d->m_tableName = tableName;
    emit sig_recordsLoaded();
}

QString TableItemMaster::getTable() const
{
    return d->m_tableName;
}

ItemHandler TableItemMaster::createTemporaryItem()
{
    auto itemHdl = createItem();
    connect(itemHdl.get(), &TableItemBase::sig_dataChanged,
            this, [this, itemHdl](){ processItemUpdate(itemHdl); });
    d->m_temporaryItems.insert(itemHdl);
    emit sig_itemAddedTemp(itemHdl);
    return itemHdl;
}

bool TableItemMaster::insertItem(ItemHandler hdl)
{
    if (!hdl || hdl->getId().isValid()) {
        m_error.setCode(ExtraClasses::ErrorCode::LogicalInvalidValue);
        m_error.setDetailText("invalid handle of item to save");
        return false;
    }

    auto rowData = hdl->getRecord();
    if (rowData.empty()) {
        COMPLOG_ERROR("TableItemMaster (", getTable().toStdString(), ") : failed convertion to values");
        return false;
    }

    auto generatedQueryText = QString("INSERT INTO %1 (%2) VALUES (%3)").arg(getTable(), recordsToColumns(rowData), recordsToValues(rowData));

    auto q = QSqlQuery(d->m_db);
    if (!executeQuery(q, generatedQueryText)) {
        return false;
    }
    hdl->setId(q.lastInsertId());
    d->m_temporaryItems.erase(hdl);
    d->m_tableRows.insert(hdl);
    if (d->m_filter) {
        if (d->m_filter(hdl)) {
            processItemAdd(hdl);
        }
    } else {
        processItemAdd(hdl);
    }
    return true;
}

bool TableItemMaster::updateItem(const ItemHandler& hdl)
{
    if (!hdl) {
        m_error.setCode(ExtraClasses::ErrorCode::LogicalInvalidValue);
        m_error.setDetailText("invalid handle of item to update");
        return false;
    }

    auto rowData = hdl->getRecord();
    if (rowData.empty()) {
        COMPLOG_ERROR("TableItemMaster (", getTable().toStdString(), ") : failed convertion to values:", hdl->getError().what());
        return false;
    }

    rowData.erase("id");
    auto generatedQueryText = QString("UPDATE %1 SET %2 WHERE id = %3").arg(getTable(), recordsToColumnValuePairs(rowData), hdl->getId().toString());

    auto q = QSqlQuery(d->m_db);
    if (!executeQuery(q, generatedQueryText)) {
        return false;
    }
    processItemUpdate(hdl);
    slot_updateFilter();
    return true;
}

bool TableItemMaster::removeItem(ItemHandler hdl)
{
    if (!hdl) {
        m_error.setCode(ExtraClasses::ErrorCode::LogicalInvalidValue);
        m_error.setDetailText("invalid handle of item to remove");
        return false;
    }

    if (!hdl->getId().isValid()) { // Item is temporary or have been removed
        auto pItem = hdl.get();
        d->m_temporaryItems.erase(hdl);
        hdl.invalidate();
        pItem->deleteLater();
        return true;
    }

    auto generatedQueryText = QString("DELETE FROM %1 WHERE id = %2").arg(getTable(), hdl->getId().toString());

    auto q = QSqlQuery(d->m_db);
    if (!executeQuery(q, generatedQueryText)) {
        return false;
    }

    auto pItem = hdl.get();
    processItemRemove(hdl);
    d->m_tableRows.erase(hdl);
    emit sig_itemDeleted(hdl);
    hdl.invalidate();
    pItem->deleteLater();
    return true;
}

ItemHandler TableItemMaster::getItem(const QVariant &id) const
{
    return findItem([&](const auto& hdl) -> bool {
        return (id == hdl->getId());
    });
}

ItemHandler TableItemMaster::getItemByRow(int itemRow) const
{
    if (itemRow < 0 || itemRow >= d->m_filteredRows.size()) { return {}; }
    auto itemIt = d->m_filteredRows.begin();
    std::advance(itemIt, itemRow);
    return *itemIt;
}

int TableItemMaster::getItemRow(const ItemHandler &hdl) const
{
    if (!hdl || hdl->getId().isNull()) {
        return -1;
    }
    auto targetIt = d->m_filteredRows.find(hdl);
    if (d->m_filteredRows.end() == targetIt) {
        return -1;
    }
    return std::distance(d->m_filteredRows.begin(), targetIt);
}

ItemHandler TableItemMaster::findItem(std::function<bool (const ItemHandler &)> &&selector, bool ignoreFilters) const
{
    for (auto& itemHdl : d->m_temporaryItems) {
        if (selector(itemHdl)) {
            return itemHdl;
        }
    }

    if (ignoreFilters) {
        for (auto& itemHdl : d->m_tableRows) {
            if (selector(itemHdl)) {
                return itemHdl;
            }
        }
    } else {
        for (auto& itemHdl : d->m_filteredRows) {
            if (selector(itemHdl)) {
                return itemHdl;
            }
        }
    }
    return {};
}

RowsHandler TableItemMaster::getAllItems() const
{
    return d->m_dummyRowHandler;
}

void TableItemMaster::slot_reloadRecords()
{
    loadRecords();
}

void TableItemMaster::resetRows()
{
    for (auto pItemHdl : d->m_temporaryItems) {
        auto pItem = pItemHdl.get();
        pItemHdl.invalidate();
        pItem->deleteLater();
    }
    d->m_temporaryItems.clear();

    d->m_filteredRows.clear();
    for (auto pItemHdl : d->m_tableRows) {
        auto pItem = pItemHdl.get();
        pItemHdl.invalidate();
        pItem->deleteLater();
    }
    d->m_tableRows.clear();
}

void TableItemMaster::slot_updateFilter()
{
    emit sig_aboutToRefilter();

    if (!d->m_filter || d->m_tableRows.empty()) {
        d->m_filteredRows = d->m_tableRows;
        emit sig_refiltered();
        return;
    }

    // Reserve delta
    std::size_t buckets = d->m_filteredRows.bucket_count();
    float maxLoad = d->m_filteredRows.max_load_factor();
    auto currentCapacity = static_cast<std::size_t>(buckets * maxLoad);
    if (currentCapacity < d->m_tableRows.size()) {
        d->m_filteredRows.reserve(d->m_tableRows.size() - currentCapacity);
    }

    // Process filters
    for (const auto& hdl : d->m_tableRows) {
        auto isCorrect = d->m_filter(hdl);
        if (!isCorrect) {
            auto removedHdlIt = d->m_filteredRows.find(hdl);
            if (d->m_filteredRows.end() != removedHdlIt) {
                processItemRemove(hdl);
            }
            continue;
        }
        auto removedHdlIt = d->m_filteredRows.find(hdl);
        if (d->m_filteredRows.end() == removedHdlIt) {
            processItemAdd(hdl);
        }
    }
    emit sig_refiltered();
}

void TableItemMaster::processItemAdd(const ItemHandler &hdl)
{
    emit sig_itemAboutToBeAdded(hdl);
    d->m_filteredRows.insert(hdl);
    emit sig_itemAdded(hdl);
}

void TableItemMaster::processItemUpdate(const ItemHandler &hdl)
{
    if (hdl && d->m_filteredRows.count(hdl)) {
        emit sig_itemUpdated(hdl);
    }
}

void TableItemMaster::processItemRemove(const ItemHandler &hdl)
{
    emit sig_itemAboutToBeRemoved(hdl);
    d->m_filteredRows.erase(hdl);
    emit sig_itemRemoved(hdl);
}

bool TableItemMaster::executeQuery(QSqlQuery &q, const QString &txt) const
{
    m_error.reset();
    if (!q.exec(txt)) {
        m_error.setCode(ExtraClasses::ErrorCode::DatabaseQueryExecutionFailed);
        m_error.setDetailText(q.lastError().text().toStdString());
        COMPLOG_ERROR("TableItemMaster (", getTable().toStdString(), ") : query failed:\n\t",
                      QString(getError().what()).toStdString());
        COMPLOG_DEBUG("Query text:\n\t", q.lastQuery().toStdString());
        return false;
    }
    // COMPLOG_DEBUG("OK Query executed:\n\t", q.lastQuery().toStdString());
    return true;
}

QString TableItemMaster::valueToString(const QVariant &val) const
{
    if (val.isNull()) {
        return "NULL";
    }
    if (val.canConvert<QString>()) {
        return QString("'%1'").arg(val.toString());
    }
    return val.toString();
}

QString TableItemMaster::recordsToColumns(const RowValues &vals) const
{
    QString res;
    for (auto& [colName, _] : vals) {
        res += colName + ',';
    }
    if (!res.isEmpty()) { res.chop(1); } // Extra ','
    return res;
}

QString TableItemMaster::recordsToValues(const RowValues &vals) const
{
    QString res;
    for (auto& [_, valueStr] : vals) {
        res += valueToString(valueStr) + ',';
    }
    if (!res.isEmpty()) { res.chop(1); } // Extra ','
    return res;
}

QString TableItemMaster::recordsToColumnValuePairs(const RowValues &vals) const
{
    QString res;
    for (auto& [colName, valueStr] : vals) {
        res += colName + '=' + valueToString(valueStr) + ',';
    }
    if (!res.isEmpty()) { res.chop(1); } // Extra ','
    return res;
}

}
