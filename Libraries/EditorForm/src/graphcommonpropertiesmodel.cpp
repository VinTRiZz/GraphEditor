#include "graphcommonpropertiesmodel.h"

GraphCommonPropertiesModel::GraphCommonPropertiesModel(QObject *parent)
    : QAbstractTableModel(parent)
{
}

void GraphCommonPropertiesModel::setGraph(Graph::PMaintainer pMaintainer)
{
    if (m_pMaintainer) {
        disconnect(m_pMaintainer.get(), nullptr, this, nullptr);
    }
    m_pMaintainer = pMaintainer;
    if (m_pMaintainer) {
        connect(m_pMaintainer.get(), &Graph::GraphMaintainer::changedCommonProperty, this, [this](){
            beginResetModel();
            endResetModel();
        });
    }
    beginResetModel();
    endResetModel();
}

QVariant GraphCommonPropertiesModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if (role != Qt::DisplayRole) {
        return {};
    }

    switch (section)
    {
    case 0: return "Свойство";
    case 1: return "Значение";
    }

    return {};
}

int GraphCommonPropertiesModel::rowCount(const QModelIndex &parent) const
{
    if (parent.isValid())
        return 0;
    return 4; // При добавлении -- изменяем.
}

int GraphCommonPropertiesModel::columnCount(const QModelIndex &parent) const
{
    if (parent.isValid())
        return 0;
    return 2;
}

QVariant GraphCommonPropertiesModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid())
        return QVariant();

    if (role != Qt::DisplayRole && role != Qt::EditRole) {
        return {};
    }

    if (index.column() == 0) {
        switch (index.row())
        {
        case CPR_NAME:          return "Название";
        case CPR_DESCRIPTION:   return "Описание";
        case CPR_CREATED_BY:    return "Создан";
        case CPR_EDITED_BY:     return "Изменён";
        }
    } else if (index.column() == 1) {
        switch (index.row())
        {
        case CPR_NAME:          return m_pMaintainer->getName();
        case CPR_DESCRIPTION:   return m_pMaintainer->getDescription();
        case CPR_CREATED_BY:    return m_pMaintainer->getCreateTime().toString(GraphCommon::DATE_DISPLAY_CONVERSION_FORMAT);
        case CPR_EDITED_BY:     return m_pMaintainer->getEditTime().toString(GraphCommon::DATE_DISPLAY_CONVERSION_FORMAT);
        }
    }
    return QVariant();
}

bool GraphCommonPropertiesModel::setData(const QModelIndex &index, const QVariant &value, int role)
{
    if (role != Qt::EditRole) {
        return QAbstractTableModel::setData(index, value, role);
    }

    if (index.column() == 1) {
        switch (index.row())
        {
        case CPR_NAME:
            m_pMaintainer->setName(value.toString());
            return true;

        case CPR_DESCRIPTION:
            m_pMaintainer->setDescription(value.toString());
            return true;
        }
    }
    return false;
}

Qt::ItemFlags GraphCommonPropertiesModel::flags(const QModelIndex &index) const
{
    auto retFlags = QAbstractTableModel::flags(index);
    if (index.column() == 0 ||
            (index.row() == CPR_CREATED_BY) ||
            (index.row() == CPR_EDITED_BY)) {
        return (retFlags & ~Qt::ItemIsEditable);
    }
    return (retFlags | Qt::ItemIsEditable);
}
