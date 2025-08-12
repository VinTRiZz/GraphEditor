#include "graphcustompropertiesmodel.h"

GraphCustomPropertiesModel::GraphCustomPropertiesModel(QObject *parent)
    : QAbstractItemModel(parent)
{
}

QVariant GraphCustomPropertiesModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    // FIXME: Implement me!
}

QModelIndex GraphCustomPropertiesModel::index(int row, int column, const QModelIndex &parent) const
{
    // FIXME: Implement me!
}

QModelIndex GraphCustomPropertiesModel::parent(const QModelIndex &index) const
{
    // FIXME: Implement me!
}

int GraphCustomPropertiesModel::rowCount(const QModelIndex &parent) const
{
    if (!parent.isValid())
        return 0;

    // FIXME: Implement me!
}

int GraphCustomPropertiesModel::columnCount(const QModelIndex &parent) const
{
    if (!parent.isValid())
        return 0;

    // FIXME: Implement me!
}

QVariant GraphCustomPropertiesModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid())
        return QVariant();

    // FIXME: Implement me!
    return QVariant();
}
