#include "tableitembase.hpp"

#include "tableitemmaster.hpp"

#include <QVariant>

namespace QtCustom::Orm {

TableItemBase::TableItemBase(TableItemMaster *pTableMaster)
    : QObject{pTableMaster}
{
    m_tableName = pTableMaster->getTable();

}

QString TableItemBase::getTableName() const
{
    return m_tableName;
}

void TableItemBase::setId(const QVariant &id)
{
    setField("id", id);
    emit sig_idChanged(id);
}

QVariant TableItemBase::getId() const
{
    return getField("id");
}

void TableItemBase::saveItem()
{
    auto parentTable = static_cast<TableItemMaster*>(parent());

    m_rowData.commit();

    bool isSucceed {false};
    if (getId().isNull()) {
        auto hdl = parentTable->findItem([this](const auto& hdl) -> bool {
            return (hdl == this);
        });
        isSucceed = parentTable->insertItem(hdl);
    } else {
        const auto id = getId();
        auto hdl = parentTable->findItem([id](const auto& hdl) -> bool {
            return (id == hdl->getId());
        }, true);
        isSucceed = parentTable->updateItem(hdl);
    }

    if (isSucceed) {
        emit sig_saved();
    }
}

void TableItemBase::restoreSaved()
{
    m_rowData.revert();
}

void TableItemBase::removeItem()
{
    auto parentTable = static_cast<TableItemMaster*>(parent());
    auto hdl = parentTable->findItem([this](const auto& hdl) -> bool {
        return (this == hdl.get());
    });
    parentTable->removeItem(hdl);
}

void TableItemBase::setRecord(const RowValues &values)
{
    m_rowData = values;
    m_rowData.commit();
    emit sig_dataChanged();
}

RowValues TableItemBase::getRecord() const
{
    return m_rowData;
}

bool TableItemBase::operator==(const TableItemBase &_oItem) const {
    return  (m_tableName == _oItem.m_tableName) &&
           (m_rowData.value() == _oItem.m_rowData.value());
}

bool TableItemBase::operator!=(const TableItemBase &_oItem) const {
    return !(*this == _oItem);
}

QVariant TableItemBase::getField(const QString &fieldName) const {
    auto targetField = m_rowData->find(fieldName);
    if (targetField == m_rowData->end()) {
        return {};
    }
    return targetField->second;
}

void TableItemBase::setField(const QString &fieldName, const QVariant &fieldValue)
{
    (*m_rowData)[fieldName] = fieldValue;
    emit sig_dataChanged();
}

}
