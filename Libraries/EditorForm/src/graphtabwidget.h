#ifndef GRAPHTABWIDGET_H
#define GRAPHTABWIDGET_H

#include <QTimer>
#include <QWidget>

class GraphEditorForm;

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

public slots:
    /**
     * @brief addTab    Добавить граф по пути до файла с ним
     * @param filePath
     */
    void addTab(const QString& filePath);

    /**
     * @brief removeTab Удалить граф по его названию
     * @param graphName
     */
    void removeTab(const QString& graphName);

    /**
     * @brief createGraph   Создать граф (добавить вкладку с новым)
     */
    void createGraph();

    /**
     * @brief saveVisibleGraph  Сохранить граф на активной вкладке
     * @param filePath
     */
    void saveVisibleGraph(const QString& filePath);

    /**
     * @brief loadVisibleGraph  Загрузить граф на активной вкладке
     * @param filePath
     */
    void loadVisibleGraph(const QString& filePath);

private:
    Ui::GraphTabWidget* ui;
    QTimer m_saveTimer;

    void setupEditorForm(GraphEditorForm* pEditorForm);
};

#endif  // GRAPHTABWIDGET_H
