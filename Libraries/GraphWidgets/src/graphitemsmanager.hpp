#pragma once

#include <QWidget>
#include <QStandardItemModel>

#include <GraphObject/PluginObjectInterface.h>

namespace Ui {
class GraphItemsManager;
}

class GraphItemsManager : public QWidget
{
    Q_OBJECT

public:
    explicit GraphItemsManager(QWidget *parent = nullptr);
    ~GraphItemsManager();

public slots:
    void setCurrentPlugin(const QString& pluginName);

private:
    Ui::GraphItemsManager *ui;

    QString m_currentPluginName;
    QStandardItemModel* m_pluginItemsModel {nullptr};

    void initSignals();
    void loadPluginItems(const QString& pluginName);
};

