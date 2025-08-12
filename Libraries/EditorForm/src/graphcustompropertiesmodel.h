#ifndef GRAPHCUSTOMPROPERTIESMODEL_H
#define GRAPHCUSTOMPROPERTIESMODEL_H

#include <QAbstractTableModel>
#include <GraphObject/Maintainer.h>

/**
 * @brief The GraphCustomPropertiesModel class  Модель для представления кастомных свойств графа
 */
class GraphCustomPropertiesModel : public QAbstractTableModel
{
    Q_OBJECT
public:
    explicit GraphCustomPropertiesModel(QObject *parent = nullptr);

    /**
     * @brief setGraph      Задать мейнтейнер графа
     * @param pMaintainer   Указатель на мейнтейнер
     */
    void setGraph(Graph::PMaintainer pMaintainer);

    /**
     * @brief addProperty   Добавить свойство
     */
    void addProperty();

    /**
     * @brief removeProperty    Удалить свойство из модели и графа
     * @param propertyName      Название свойства
     */
    void removeProperty(const QString& propertyName);

    /**
     * @brief getPropertyName   Более быстрый и удобный аналог получения названия свойства на строке
     * @param rowNo             Номер строки в модели
     * @return                  Название свойства или NULL QString, если не верная строка
     */
    QString getPropertyName(int rowNo) const;

    // ИНТЕРФЕЙС QItemModel ДЛЯ GUI
    QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const override;
    int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    int columnCount(const QModelIndex &parent = QModelIndex()) const override;
    bool setData(const QModelIndex &index, const QVariant &value, int role) override;
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;
    Qt::ItemFlags flags(const QModelIndex &index) const override;

private:
    Graph::PMaintainer m_pMaintainer;
};

#endif // GRAPHCUSTOMPROPERTIESMODEL_H
