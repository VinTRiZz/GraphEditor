#ifndef GRAPHEDITORFORM_H
#define GRAPHEDITORFORM_H

#include <QWidget>
#include <QStandardItemModel>

#include "graphextendedobject.h"
#include "GUI/CustomWidgets/buttonmatrix.h"

namespace Ui {
class GraphEditorForm;
}

/**
 * @brief The GraphEditorForm class Форма для редактирования графов и информации о них
 */
class GraphEditorForm : public QWidget
{
    Q_OBJECT

public:
    /**
     * @brief GraphEditorForm
     * @param parent
     */
    explicit GraphEditorForm(QWidget *parent = nullptr);
    ~GraphEditorForm();

    /**
     * @brief init Инициализация формы. Необходимо вызвать до использования для корректной работы
     */
    void init();

    /**
     * @brief startValidanceTest Запуск тестирования формы на валидность работы
     */
    void startValidanceTest();

private:
    Ui::GraphEditorForm *ui;

    Graph::GraphExtendedObject* m_currentGraph;         //! Текущий граф
    QString                     m_currentGraphFilePath; //! Путь для сохранения графа в файл

    /**
     * @brief isGraphPathSet Проверить, задан ли путь для сохранения/загрузки графа. Вызовет диалог и задаст путь, если он пуст или не валидный
     * @return true если путь корректен после ввода пользователем
     */
    bool isGraphPathSet();

    /**
     * @brief saveGraph Сохранение текущего графа в файл m_currentGraphFilePath
     */
    void saveGraph();

    /**
     * @brief loadGraph Выгрузить граф из выбранного в m_currentGraphFilePath пути
     */
    void loadGraph();

    /**
     * @brief setupSignals Настройка сигналов виджета
     */
    void setupSignals();

    /**
     * @brief setupWidget Настройка виджетов формы
     */
    void setupWidget();
};

#endif // GRAPHEDITORFORM_H
