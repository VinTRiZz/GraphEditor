#pragma once

#include <QWidget>

namespace Ui {
class GraphItemPropertyEditor;
}

class GraphItemPropertyEditor : public QWidget
{
    Q_OBJECT

public:
    explicit GraphItemPropertyEditor(QWidget *parent = nullptr);
    ~GraphItemPropertyEditor();

private:
    Ui::GraphItemPropertyEditor *ui;
};

