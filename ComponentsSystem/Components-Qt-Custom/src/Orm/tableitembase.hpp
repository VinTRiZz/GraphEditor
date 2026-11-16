#pragma once

#include <QObject>
#include <QVariant>

#include <unordered_map>

#include <Components/ExtraClasses/Containers/Handler.h>
#include <Components/ExtraClasses/Containers/CommitableObject.h>
#include <Components/ExtraClasses/Error.h>

namespace QtCustom::Orm {

class TableItemMaster;
using RowValues = std::unordered_map<QString, QVariant>;


/**
 * @brief The TableItemBase class Base of a row from database, according to Active Record pattern
 * @note deleted item object = removed row from db
 */
class TableItemBase : public QObject, public ExtraClasses::ErrorUserBase<ExtraClasses::ErrorObject>
{
    Q_OBJECT
public:
    explicit TableItemBase(TableItemMaster *pTableMaster = nullptr);
    QString getTableName() const;

    void setId(const QVariant &id);
    QVariant getId() const;

    /**
     * @brief saveItem Add or update item's associated record in DB
     */
    virtual void saveItem();

    /**
     * @brief restoreSaved Reverse changes to previous saved / loaded state. Does not reverse ID changes
     * @note In base class does nothing, but in inheried must restore fields
     */
    virtual void restoreSaved();

    /**
     * @brief removeItem Remove item from DB
     */
    virtual void removeItem();

    void setRecord(const RowValues& values);
    RowValues getRecord() const;

    bool operator==(const TableItemBase& _oItem) const;
    bool operator!=(const TableItemBase& _oItem) const;

signals:
    void sig_idChanged(const QVariant& nextId);

    // Universal for inherited classes
    void sig_dataChanged();

    // DB CRUD
    void sig_saved();
    void sig_restored();

private:
    QString m_tableName {"========"}; // Used to determine if item is valid
    ExtraClasses::CommitableObject<RowValues> m_rowData;

protected:
    QVariant getField(const QString& fieldName) const;
    void    setField(const QString& fieldName, const QVariant& fieldValue);
};
using ItemHandler = ExtraClasses::Handler<TableItemBase>;

}