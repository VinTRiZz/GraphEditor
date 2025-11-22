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

    void setTargetItem(Graph::PluginObjectItnterface* pTarget);

private:
    Ui::GraphItemPropertyEditor *ui;

    Graph::PluginObjectItnterface* m_targetItem {nullptr};
};

