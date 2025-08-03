#ifndef GRAPHPROPERTYEDITFORM_H
#define GRAPHPROPERTYEDITFORM_H

#include <GraphObject/Maintainer.h>

#include <QStandardItemModel>
#include <QWidget>

namespace Ui {
class GraphPropertyEditForm;
}

namespace Graph {

class GraphPropertyEditForm : public QWidget {
    Q_OBJECT

public:
    explicit GraphPropertyEditForm(QWidget* parent = nullptr);
    ~GraphPropertyEditForm();

    /**
     * @brief updateGraphInfo Обновление информации о графе
     */
    void updateGraphInfo();

    /**
     * @brief setCurrentGraph   Задать мейнтейнера данных графа
     * @param pGraphMaintaner   Указатель на инстанцию мейнтейнера
     */
    void setCurrentGraph(const Graph::PMaintainer& pGraphMaintaner);

public slots:
    /**
     * @brief updateEditTime Должно вызываться после любых изменений свойств
     * графа
     */
    void updateEditTime();

private:
    Ui::GraphPropertyEditForm* ui;

    Graph::PMaintainer m_currentGraph;  //! Текущий майнтейнер графа

    enum GraphCommonPropertyRows : int {
        NAMEROW = 0,
        DESCRIPTIONROW,
        CREATEDROW,
        EDITEDROW,
    };

    bool m_isSettingGraph{false};  //! Для задания графов без удаления их
                                   //! свойств (особенности логики)
    QStandardItemModel* m_pCommonGraphInfoModel{
        nullptr};  //! Модель с общими данными по графу
    QStandardItemModel* m_pUserGraphInfoModel{
        nullptr};  //! Модель с пользовательскими данными по графу

    /**
     * @brief setupSignals Настройка сигналов виджета
     */
    void setupSignals();

    /**
     * @brief setupModels Настройка моделей отображения свойств графа
     */
    void setupModels();

    /**
     * @brief setupWidget Настройка виджетов формы
     */
    void setupWidget();
};

}  // namespace Graph

#endif  // GRAPHPROPERTYEDITFORM_H
