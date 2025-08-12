#ifndef GRAPHCOMMONPROPERTIESMODEL_H
#define GRAPHCOMMONPROPERTIESMODEL_H

#include <QAbstractTableModel>

#include <GraphObject/Maintainer.h>

class GraphCommonPropertiesModel : public QAbstractTableModel
{
    Q_OBJECT
public:
    explicit GraphCommonPropertiesModel(QObject *parent = nullptr);

    enum CommonPropertyRows
    {
        CPR_NAME,
        CPR_DESCRIPTION,
        CPR_CREATED_BY,
        CPR_EDITED_BY
    };

    void setGraph(Graph::PMaintainer pMaintainer);

    // Header:
    QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const override;

    // Basic functionality:
    int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    int columnCount(const QModelIndex &parent = QModelIndex()) const override;

    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;

    bool setData(const QModelIndex &index, const QVariant &value, int role) override;
    Qt::ItemFlags flags(const QModelIndex &index) const override;

private:
    Graph::PMaintainer m_pMaintainer;

};

#endif // GRAPHCOMMONPROPERTIESMODEL_H
