#ifndef GRAPHEDITORFORM_H
#define GRAPHEDITORFORM_H

#include <CustomWidgets/ButtonMatrix.h>
#include <GraphObject/ExtendedObject.h>
#include <GraphObject/Maintainer.h>

#include <QStandardItemModel>
#include <QWidget>

#include <GraphScene/EditView.h>

namespace Ui {
class GraphEditorForm;
}

/**
 * @brief The GraphEditorForm class Форма для редактирования графов и информации
 * о них
 */
class GraphEditorForm : public QWidget {
    Q_OBJECT

public:
    /**
     * @brief GraphEditorForm
     * @param parent
     */
    explicit GraphEditorForm(QWidget* parent = nullptr);
    ~GraphEditorForm();

    /**
     * @brief setGraph      Задать граф
     * @param saveFilePath  Путь до файла графа
     * @retval              true при успешном открытии графа
     */
    bool setGraph(const QString& saveFilePath);

    /**
     * @brief getGraph  Получить заданный граф
     * @return          Указатель на мейнтейнер графов
     */
    Graph::PMaintainer getGraph() const;

    /**
     * @brief getIsSavepathValid    Проверка, существует ли файл по целевому
     * пути для сохранения и загрузки
     * @param targetPath            Целевой путь
     * @return                      true если граф может быть сохранён
     */
    bool getIsSavepathValid() const;

    /**
     * @brief getSavefilePath   Получить текущий путь до файла графа
     * @return                  Значение пути. может быть NULL если ещё не
     * задано
     */
    QString getSavefilePath() const;

    /**
     * @brief saveGraph     Сохранение текущего графа в файл
     * @param targetPath    Путь к файлу
     * @return              true если файл был успешно сохранён
     */
    bool saveGraph(const QString& targetPath = {});

    /**
     * @brief loadGraph     Выгрузить граф из файла
     * @param targetPath    Путь к файлу
     * @return              true если граф был успешно загружен
     */
    bool loadGraph(const QString& targetPath = {});

    void setEditMode();
    void setViewMode();
    void setAlgoMode();
    void setCrimeMode();

    Graph::GraphSceneView* getScene() const;

private:
    Ui::GraphEditorForm* ui;
    Graph::PMaintainer m_graphMaintainer;  //! Текущий граф
    QString m_lastSavePath;  //! Путь до файла с последним сохранённым графом
    unsigned m_propBarShowWidth{
        400};  //! Значение ширины, которой будет окно свойств после появления.
               //! Нужно для анимаций

public slots:
    void showGraphProperties();
    void hideGraphProperties();

    void showObjectProperties(QGraphicsItem* pTargetItem);
    void hideObjectProperties();

    void updateCanvasSize();
    void updateCanvasGrid();
};

#endif  // GRAPHEDITORFORM_H
