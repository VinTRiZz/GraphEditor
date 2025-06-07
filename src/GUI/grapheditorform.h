#ifndef GRAPHEDITORFORM_H
#define GRAPHEDITORFORM_H

#include <QWidget>

namespace Ui {
class GraphEditorForm;
}

class GraphEditorForm : public QWidget
{
    Q_OBJECT

public:
    explicit GraphEditorForm(QWidget *parent = nullptr);
    ~GraphEditorForm();

private:
    Ui::GraphEditorForm *ui;

    void setupSignals();
};

#endif // GRAPHEDITORFORM_H
