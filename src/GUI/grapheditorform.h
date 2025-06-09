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
     * @brief setCurrentGraph Выбрать граф в качестве текущего
     * @param pGraph Указатель на граф, с которым будет проводиться работа
     */
    void setCurrentGraph(Graph::GraphObject* pGraph);

    /**
     * @brief getCurrentGraph Получить текущий граф
     * @return Указатель на текущий граф. nullptr если не задан
     */
    Graph::GraphObject* getCurrentGraph();

private:
    Ui::GraphEditorForm *ui;

    Graph::GraphObject* m_currentGraph              {nullptr}; //! Указатель на текущий граф во избежание множественного копирования данных
    QString             m_currentGraphFilePath;                //! Путь для сохранения графа в файл

    QStandardItemModel* m_pCommonGraphInfoModel     {nullptr}; //! Модель с общими данными по графу
    QStandardItemModel* m_pUserGraphInfoModel       {nullptr}; //! Модель с пользовательскими данными по графу

    /**
     * @brief saveGraph Сохранение текущего графа в файл
     */
    void saveGraph();

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
