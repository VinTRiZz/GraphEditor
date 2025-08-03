#ifndef GRAPHTABWIDGET_H
#define GRAPHTABWIDGET_H

#include <QTimer>
#include <QWidget>

namespace Ui {
class GraphTabWidget;
}

class GraphTabWidget : public QWidget {
    Q_OBJECT

public:
    explicit GraphTabWidget(QWidget* parent = nullptr);
    ~GraphTabWidget();

public slots:
    void addTab(const QString& filePath);
    void removeTab(const QString& graphName);

    void createGraph();
    void saveVisibleGraph(const QString& filePath);
    void loadVisibleGraph(const QString& filePath);

private:
    Ui::GraphTabWidget* ui;
    QTimer m_saveTimer;
};

#endif  // GRAPHTABWIDGET_H
