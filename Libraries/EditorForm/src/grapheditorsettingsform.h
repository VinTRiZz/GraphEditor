#ifndef GRAPHEDITORSETTINGSFORM_H
#define GRAPHEDITORSETTINGSFORM_H

#include <QWidget>

namespace Ui {
class GraphEditorSettingsForm;
}

class GraphEditorSettingsForm : public QWidget {
    Q_OBJECT

public:
    explicit GraphEditorSettingsForm(QWidget* parent = nullptr);
    ~GraphEditorSettingsForm();

signals:
    void updateCanvasSize();

    void updateSceneBrush(const QBrush& sceneBrush);

    void updateCanvasBrush(const QBrush& canvasBrush);
    void updateCanvasOpacity(double opac);

    void updateGridColor(const QColor& gColor);

public slots:
    void loadSettings();
    void applySettings();

private:
    Ui::GraphEditorSettingsForm* ui;

    void showEvent(QShowEvent* e) override;
};

#endif  // GRAPHEDITORSETTINGSFORM_H
