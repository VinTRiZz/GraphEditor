#ifndef GRAPHEDITORFORM_H
#define GRAPHEDITORFORM_H

#include <QWidget>
#include <QStandardItemModel>

#include "graphobject.h"

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
     * @brief startValidanceTest Запуск тестирования формы на валидность работы
     */
    void startValidanceTest();

    /**
     * @brief getCurrentGraph Получить текущий граф
     * @return Указатель на текущий граф (объект графа существует вместе с формой)
     */
    Graph::GraphObject* getCurrentGraph();

private:
    Ui::GraphEditorForm *ui;

    QGraphicsItem*      m_currentGraphItem          {nullptr}; //! Указатель на объект сцены, соответствующий графу
    Graph::GraphObject  m_currentGraph;                        //! Текущий граф
    QString             m_currentGraphFilePath;                //! Путь для сохранения графа в файл

    QStandardItemModel* m_pCommonGraphInfoModel     {nullptr}; //! Модель с общими данными по графу
    QStandardItemModel* m_pUserGraphInfoModel       {nullptr}; //! Модель с пользовательскими данными по графу

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
     * @brief setupModels Настройка моделей отображения свойств графа
     */
    void setupModels();

    /**
     * @brief updateGraphInfo Обновление информации о графе
     */
    void updateGraphInfo();
};

#endif // GRAPHEDITORFORM_H
