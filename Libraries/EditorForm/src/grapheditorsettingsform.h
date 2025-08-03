#ifndef GRAPHEDITORSETTINGSFORM_H
#define GRAPHEDITORSETTINGSFORM_H

#include <QWidget>

namespace Ui {
class GraphEditorSettingsForm;
}

class GraphEditorSettingsForm : public QWidget
{
    Q_OBJECT

public:
    explicit GraphEditorSettingsForm(QWidget *parent = nullptr);
    ~GraphEditorSettingsForm();

private:
    Ui::GraphEditorSettingsForm *ui;
};

#endif // GRAPHEDITORSETTINGSFORM_H
