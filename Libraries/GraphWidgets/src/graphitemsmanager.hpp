#pragma once

#include <QWidget>
#include <QStandardItemModel>

#include <GraphItems/PluginObjectInterface.h>

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
    void updatePluginList();

signals:
    void addObject(Graph::PluginObjectInterface* pVertex);

private slots:
    void loadPluginItems(const QString& pluginName);

private:
    Ui::GraphItemsManager *ui;

    QStandardItemModel* m_pluginItemsModel {nullptr};

    void initSignals();

    void addPlugin(const QString& pluginName);
};

