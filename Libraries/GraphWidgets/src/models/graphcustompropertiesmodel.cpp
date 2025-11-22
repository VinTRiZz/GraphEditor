#include "graphcustompropertiesmodel.h"


namespace Graph {

GraphCustomPropertiesModel::GraphCustomPropertiesModel(QObject* parent)
    : QAbstractTableModel(parent) {}

void GraphCustomPropertiesModel::addProperty() {
    auto pMaintainer = getGraph();
    beginInsertRows({}, 0, rowCount() + 1);
    pMaintainer->setCustomValue("Моё свойство", "Моё значение");
    endInsertRows();
}

void GraphCustomPropertiesModel::removeProperty(const QString& propertyName) {
    auto pMaintainer = getGraph();
    beginRemoveRows({}, 0, rowCount() + 1);
    pMaintainer->removeCustomValue(propertyName);
    endInsertRows();
}

QString GraphCustomPropertiesModel::getPropertyName(int rowNo) const {
    assert(rowNo >= 0);

    auto pMaintainer = getGraph();
    auto& customValues = pMaintainer->getCustomValueMapRef();

    // Быть такого не должно, но всё же
    if (rowNo >= customValues.size()) [[unlikely]] {
        return {};
    }

    auto begIt = customValues.begin();
    std::advance(begIt, rowNo);
    return begIt->first;
}

QVariant GraphCustomPropertiesModel::headerData(int section,
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

int GraphCustomPropertiesModel::rowCount(const QModelIndex& parent) const {
    auto pMaintainer = getGraph();
    if (parent.isValid() || !pMaintainer)
        return 0;
    return pMaintainer->getCustomValueMapRef().size();
}

int GraphCustomPropertiesModel::columnCount(const QModelIndex& parent) const {
    if (parent.isValid())
        return 0;
    return 2;
}

QVariant GraphCustomPropertiesModel::data(const QModelIndex& index,
                                          int role) const {
    auto pMaintainer = getGraph();
    if (!index.isValid() || !pMaintainer)
        return QVariant();

    if (role != Qt::DisplayRole && role != Qt::EditRole) {
        return {};
    }

    if (index.column() == 0) {
        auto& customValues = pMaintainer->getCustomValueMapRef();

        // Быть такого не должно, но всё же
        if (index.row() >= customValues.size()) [[unlikely]] {
            return {};
        }

        auto begIt = customValues.begin();
        std::advance(begIt, index.row());
        return begIt->first;
    } else if (index.column() == 1) {
        auto& customValues = pMaintainer->getCustomValueMapRef();

        // Быть такого не должно, но всё же
        if (index.row() >= customValues.size()) [[unlikely]] {
            return {};
        }

        auto begIt = customValues.begin();
        std::advance(begIt, index.row());
        return begIt->second;
    }
    return QVariant();
}

bool GraphCustomPropertiesModel::setData(const QModelIndex& index,
                                         const QVariant& value, int role) {
    auto pMaintainer = getGraph();
    if (role != Qt::EditRole || !pMaintainer) {
        return QAbstractTableModel::setData(index, value, role);
    }

    if (index.column() == 0) {
        // Если редактируется поле слева (название), логика сложная
        auto customValueKey = index.data(Qt::DisplayRole).toString();
        auto prevValue = pMaintainer->getCustomValue(customValueKey);
        pMaintainer->removeCustomValue(customValueKey);
        pMaintainer->setCustomValue(value.toString(), prevValue);
        return true;

    } else if (index.column() == 1) {
        // Если редактируется поле справа (значение), логика простая
        auto customValueKey =
            index.siblingAtColumn(0).data(Qt::DisplayRole).toString();
        pMaintainer->setCustomValue(customValueKey, value.toString());
        return true;
    }
    return false;
}

Qt::ItemFlags GraphCustomPropertiesModel::flags(
    const QModelIndex& index) const {
    return (QAbstractTableModel::flags(index) | Qt::ItemIsEditable);
}

void GraphCustomPropertiesModel::processGraphChange()
{
    auto pMaintainer = getGraph();
    if (pMaintainer) {
        disconnect(pMaintainer.get(), nullptr, this, nullptr);
    }
    pMaintainer = pMaintainer;
    if (pMaintainer) {
        connect(pMaintainer.get(),
                &Graph::GraphMaintainer::changedCustomProperty, this, [this]() {
                    beginResetModel();
                    endResetModel();
                });
    }
    beginResetModel();
    endResetModel();
}

}
