#pragma once

#include <QWidget>

#include <GraphObject/GraphObject.h>

namespace Ui {
class FileManagementToolbar;
}

class FileManagementToolbar :
        public QWidget,
        public Graph::GraphObjectUser
{
    Q_OBJECT

public:
    explicit FileManagementToolbar(QWidget *parent = nullptr);
    ~FileManagementToolbar();

    bool saveGraph(const QString& savePath = {});
    bool saveGraphAsTemporary();
    bool loadGraph(const QString& savePath = {});

signals:
    void createGraph();
    void loadedGraph(const QString& loadPath);

private:
    Ui::FileManagementToolbar *ui;

    void processGraphChange() override;
};
