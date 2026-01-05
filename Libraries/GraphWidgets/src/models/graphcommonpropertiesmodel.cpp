#include "graphcommonpropertiesmodel.h"


namespace Graph {

void GraphPropertiesModel::processGraphChange(const GraphObjectManagerPtr &pPrevGraph)
{
    beginResetModel();
    endResetModel();
}

QVariant GraphPropertiesModel::headerData(int section,
                                          Qt::Orientation orientation,
                                          int role) const {
    if (role != Qt::DisplayRole) {
        return {};
    }

    switch (section) {
    case 0:
        return "Свойство";
    case 1:
        return "Значение";
    }

    return {};
}

int GraphPropertiesModel::rowCount(const QModelIndex& parent) const {
    if (parent.isValid())
        return 0;
    return 4;  // При добавлении -- изменяем.
}

int GraphPropertiesModel::columnCount(const QModelIndex& parent) const {
    if (parent.isValid())
        return 0;
    return 2;
}

QVariant GraphPropertiesModel::data(const QModelIndex& index,
                                          int role) const {
    if (!index.isValid())
        return QVariant();

    if (role != Qt::DisplayRole && role != Qt::EditRole) {
        return {};
    }

    if (index.column() == 0) {
        switch (index.row()) {
            case CPR_NAME:
                return "Название";
            case CPR_DESCRIPTION:
                return "Описание";
            case CPR_CREATED_BY:
                return "Создан";
            case CPR_EDITED_BY:
                return "Изменён";
        }
    } else if (index.column() == 1) {
        auto pGraphObject = getGraph()->getObject();
        if (!pGraphObject) {
            return {};
        }

        switch (index.row()) {
            case CPR_NAME:
                return pGraphObject->getMetaInfo()->getName();
            case CPR_DESCRIPTION:
                return pGraphObject->getMetaInfo()->getDescription();
            case CPR_CREATED_BY:
                return pGraphObject->getMetaInfo()->getCreateTime().toString(
                    Graph::DATE_DISPLAY_CONVERSION_FORMAT);
            case CPR_EDITED_BY:
                return pGraphObject->getMetaInfo()->getEditTime().toString(
                    Graph::DATE_DISPLAY_CONVERSION_FORMAT);
        }
    }
    return QVariant();
}

bool GraphPropertiesModel::setData(const QModelIndex& index,
                                         const QVariant& value, int role) {
    if (role != Qt::EditRole) {
        return QAbstractTableModel::setData(index, value, role);
    }

    if (index.column() == 1) {
        auto pGraphObject = getGraph()->getObject();
        switch (index.row()) {
            case CPR_NAME:
                pGraphObject->getMetaInfo()->setName(value.toString());
                return true;

            case CPR_DESCRIPTION:
                pGraphObject->getMetaInfo()->setDescription(value.toString());
                return true;
        }
    }
    return false;
}

Qt::ItemFlags GraphPropertiesModel::flags(
    const QModelIndex& index) const {
    auto retFlags = QAbstractTableModel::flags(index);
    if (index.column() == 0 || (index.row() == CPR_CREATED_BY) ||
        (index.row() == CPR_EDITED_BY)) {
        return (retFlags & ~Qt::ItemIsEditable);
    }
    return (retFlags | Qt::ItemIsEditable);
}

}
