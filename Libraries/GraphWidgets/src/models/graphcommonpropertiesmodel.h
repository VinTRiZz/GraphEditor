#pragma once

#include <GraphObject/GraphObject.h>
#include <QAbstractTableModel>

namespace Graph {

/**
 * @brief The GraphPropertiesModel class  Модель для представления
 * основных свойств графа
 */
class GraphPropertiesModel :
        public QAbstractTableModel,
        public Graph::GraphObjectUser {
    Q_OBJECT
public:
    using QAbstractTableModel::QAbstractTableModel;


    /**
     * @brief The CommonPropertyRows enum   Строки в модели
     */
    enum CommonPropertyRows {
        CPR_NAME,
        CPR_DESCRIPTION,
        CPR_CREATED_BY,
        CPR_EDITED_BY
    };

    // ИНТЕРФЕЙС QItemModel ДЛЯ GUI
    QVariant headerData(int section, Qt::Orientation orientation,
                        int role = Qt::DisplayRole) const override;
    int rowCount(const QModelIndex& parent = QModelIndex()) const override;
    int columnCount(const QModelIndex& parent = QModelIndex()) const override;
    QVariant data(const QModelIndex& index,
                  int role = Qt::DisplayRole) const override;
    bool setData(const QModelIndex& index, const QVariant& value,
                 int role) override;
    Qt::ItemFlags flags(const QModelIndex& index) const override;

private:
    void processGraphChange() override;
};

}
