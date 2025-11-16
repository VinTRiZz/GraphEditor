#pragma once

#include <QWidget>
#include <QStandardItemModel>

#include <GraphItems/VertexItem.h>

namespace Ui {
class GraphItemsManager;
}

class GraphItemsManager : public QWidget
{
    Q_OBJECT

public:
    explicit GraphItemsManager(QWidget *parent = nullptr);
    ~GraphItemsManager();

    enum class PluginType : int {
        Unknown = -1,
        Vertex = 0,
        Connection,
    };

signals:
    void addVertex(Graph::VertexItem* pVertex);
    void addConnection(Graph::VertexConnectionLine* pLine);

private slots:
    void loadPluginItems(const QString& pluginName);

private:
    Ui::GraphItemsManager *ui;

    QStandardItemModel* m_pluginItemsModel {nullptr};

    void initSignals();

    void initVertexItems();
    void initConnectionItems();

    void addPlugin(PluginType ptype, const QString& pluginName);
};

