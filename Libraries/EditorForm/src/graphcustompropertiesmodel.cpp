#include "graphcustompropertiesmodel.h"

GraphCustomPropertiesModel::GraphCustomPropertiesModel(QObject *parent)
    : QAbstractTableModel(parent) {}

void GraphCustomPropertiesModel::setGraph(Graph::PMaintainer pMaintainer) {
  if (m_pMaintainer) {
    disconnect(m_pMaintainer.get(), nullptr, this, nullptr);
  }
  m_pMaintainer = pMaintainer;
  if (m_pMaintainer) {
    connect(m_pMaintainer.get(), &Graph::GraphMaintainer::changedCustomProperty,
            this, [this]() {
              beginResetModel();
              endResetModel();
            });
  }
  beginResetModel();
  endResetModel();
}

void GraphCustomPropertiesModel::addProperty() {
  // TODO: Сделать сложную логику по поиску точки вставки (нет большой
  // необходимости)
  beginInsertRows({}, 0, rowCount() + 1);
  m_pMaintainer->setCustomValue("Моё свойство", "Моё значение");
  endInsertRows();
}

void GraphCustomPropertiesModel::removeProperty(const QString &propertyName) {
  // TODO: Сделать сложную логику по поиску точки вставки (нет большой
  // необходимости)
  beginRemoveRows({}, 0, rowCount() + 1);
  m_pMaintainer->removeCustomValue(propertyName);
  endInsertRows();
}

QString GraphCustomPropertiesModel::getPropertyName(int rowNo) const {
  // Так нельзя делать вообще-то
  if (rowNo < 0) [[unlikely]] {
    return {};
  }

  auto &customValues = m_pMaintainer->getCustomValueMapRef();

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

int GraphCustomPropertiesModel::rowCount(const QModelIndex &parent) const {
  if (parent.isValid() || !m_pMaintainer)
    return 0;
  return m_pMaintainer->getCustomValueMapRef().size();
}

int GraphCustomPropertiesModel::columnCount(const QModelIndex &parent) const {
  if (parent.isValid())
    return 0;
  return 2;
}

QVariant GraphCustomPropertiesModel::data(const QModelIndex &index,
                                          int role) const {
  if (!index.isValid() || !m_pMaintainer)
    return QVariant();

  if (role != Qt::DisplayRole && role != Qt::EditRole) {
    return {};
  }

  if (index.column() == 0) {
    auto &customValues = m_pMaintainer->getCustomValueMapRef();

    // Быть такого не должно, но всё же
    if (index.row() >= customValues.size()) [[unlikely]] {
      return {};
    }

    auto begIt = customValues.begin();
    std::advance(begIt, index.row());
    return begIt->first;
  } else if (index.column() == 1) {
    auto &customValues = m_pMaintainer->getCustomValueMapRef();

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

bool GraphCustomPropertiesModel::setData(const QModelIndex &index,
                                         const QVariant &value, int role) {
  if (role != Qt::EditRole || !m_pMaintainer) {
    return QAbstractTableModel::setData(index, value, role);
  }

  if (index.column() == 0) {
    // Если редактируется поле слева (название), логика сложная
    auto customValueKey = index.data(Qt::DisplayRole).toString();
    auto prevValue = m_pMaintainer->getCustomValue(customValueKey);
    m_pMaintainer->removeCustomValue(customValueKey);
    m_pMaintainer->setCustomValue(value.toString(), prevValue);
    return true;

  } else if (index.column() == 1) {
    // Если редактируется поле справа (значение), логика простая
    auto customValueKey =
        index.siblingAtColumn(0).data(Qt::DisplayRole).toString();
    m_pMaintainer->setCustomValue(customValueKey, value.toString());
    return true;
  }
  return false;
}

Qt::ItemFlags
GraphCustomPropertiesModel::flags(const QModelIndex &index) const {
  return (QAbstractTableModel::flags(index) | Qt::ItemIsEditable);
}
