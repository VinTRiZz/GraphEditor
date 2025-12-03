#ifndef GRAPHTABWIDGET_H
#define GRAPHTABWIDGET_H

#include <QTimer>
#include <QWidget>

namespace Ui {
class GraphTabWidget;
}

/**
 * @brief The GraphTabWidget class  Форма вкладок с открытыми графами
 */
class GraphTabWidget : public QWidget {
    Q_OBJECT

public:
    explicit GraphTabWidget(QWidget* parent = nullptr);
    ~GraphTabWidget();

private slots:
    void addTab(const QString& filePath);
    void createGraph();

    void updatePluginList();

private:
    Ui::GraphTabWidget* ui;
};

#endif  // GRAPHTABWIDGET_H
