#pragma once

#include <QObject>

#include <unordered_set>

#include <Components/ExtraClasses/Containers/Handler.h>

#include "tableitembase.hpp"

class QSqlDatabase;
class QSqlQuery;

template <>
struct std::hash<QtCustom::Orm::ItemHandler>
{
    std::size_t operator()(const QtCustom::Orm::ItemHandler& hdl) const {
        return std::hash<const QtCustom::Orm::ItemHandler::value_t*>{}(hdl.get());
    }
};

namespace QtCustom::Orm {

using ItemContainer = std::unordered_set<ItemHandler>;
using RowsHandler = ExtraClasses::Handler<const ItemContainer>;

/**
 * @brief The TableItemMaster class Handle for a table, providing objects, according to Active Record pattern
 */
class TableItemMaster : public QObject,
                        public ExtraClasses::ErrorUserBase<ExtraClasses::ErrorObject>
{
    Q_OBJECT
public:
    TableItemMaster(const QSqlDatabase& db, QObject *parent = nullptr);
    ~TableItemMaster();

    QSqlDatabase getDatabase() const;

    using selector_t = std::function<bool(const ItemHandler&)>;

    virtual bool createTable();
    bool isTableExist() const;
    bool loadRecords(); // TODO: Work with dynamic load, if even needed
    virtual void setDatabaseFilter(const QString& recordsFilter);
    virtual void setFilter(selector_t&& selector);

    void setTable(const QString& tableName);
    QString getTable() const;

    // CRUD of rows
    ItemHandler createTemporaryItem();              // Creates temporary item to save manually, will have invalid ID
    bool        insertItem(ItemHandler hdl);        // Inserts row into table (saves created temporary item)
    bool        updateItem(const ItemHandler &hdl); // Updates row in table
    bool        removeItem(ItemHandler hdl);        // Removes item with id. Also invalidates handlers

    // Search not only in visible, but in temporary items too. Ignore filters flag is mostly for item saving
    ItemHandler findItem(selector_t&& selector, bool ignoreFilters = false) const;

    ItemHandler getItem(const QVariant& id) const;
    ItemHandler getItemByRow(int itemRow) const;
    int         getItemRow(const ItemHandler& hdl) const;
    RowsHandler getAllItems() const;

signals:
    void sig_recordsLoadStarted();
    void sig_recordsLoaded();
    void sig_databaseFilterChanged();

    // For dynamic filtering
    void sig_aboutToRefilter();
    void sig_refiltered();

    // All ADD / UPDATE / REMOVE signals have sense ONLY in table context
    // DB can be modified before "about" signals call (to be less complex)
    void sig_itemAddedTemp(const ItemHandler& tempItem);

    // Important: signals are designed for Qt's item models
    void sig_itemAboutToBeAdded(const ItemHandler& addedItem);
    void sig_itemAdded(const ItemHandler& addedItem);
    void sig_itemUpdated(const ItemHandler& updatedItem);

    // Does not emitted for temporary items
    // Important: signals are designed for Qt's item models
    void sig_itemAboutToBeRemoved(const ItemHandler& removedItem);
    void sig_itemRemoved(const ItemHandler& removedItem);

    // For REAL removing item, not just by filters
    void sig_itemDeleted(const ItemHandler& removedItem);

public slots:
    void slot_reloadRecords();
    void slot_updateFilter();

private:
    struct Impl;
    std::unique_ptr<Impl> d;

    void processItemAdd(const ItemHandler& hdl);
    void processItemUpdate(const ItemHandler& hdl);
    void processItemRemove(const ItemHandler& hdl);

    void resetRows();

protected:
    // Executes query using item and writes error code + may have extra processing
    virtual bool executeQuery(QSqlQuery& q, const QString& txt) const;

    // IMPORTANT: Value types are not detected, so quotes and others must be in item field setting methods
    QString valueToString(const QVariant& val) const;
    QString recordsToColumns(const RowValues& vals) const;
    QString recordsToValues(const RowValues& vals) const;
    QString recordsToColumnValuePairs(const RowValues& vals) const;

    virtual ItemHandler createItem() = 0;
};

}