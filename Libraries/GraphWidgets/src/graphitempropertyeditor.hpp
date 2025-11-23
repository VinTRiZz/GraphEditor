#pragma once

#include <QWidget>

#include <GraphItems/PluginObjectInterface.h>

namespace Ui {
class GraphItemPropertyEditor;
}

class GraphItemPropertyEditor : public QWidget
{
    Q_OBJECT

public:
    explicit GraphItemPropertyEditor(QWidget *parent = nullptr);
    ~GraphItemPropertyEditor();

public slots:
    void setTargetItem(Graph::PluginObjectInterface* pTarget);

private:
    Ui::GraphItemPropertyEditor *ui;

    Graph::PluginObjectInterface* m_targetItem {nullptr};
};

