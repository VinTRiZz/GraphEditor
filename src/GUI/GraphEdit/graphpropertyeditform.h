#ifndef GRAPHPROPERTYEDITFORM_H
#define GRAPHPROPERTYEDITFORM_H

#include <QWidget>
#include <QStandardItemModel>

#include "graphextendedobject.h"

namespace Ui {
class GraphPropertyEditForm;
}

namespace Graph
{

class GraphPropertyEditForm : public QWidget
{
    Q_OBJECT

public:
    explicit GraphPropertyEditForm(QWidget *parent = nullptr);
    ~GraphPropertyEditForm();

    /**
     * @brief updateGraphInfo Обновление информации о графе
     */
    void updateGraphInfo();

    void setCurrentGraph(Graph::GraphExtendedObject* pGraph);

private:
    Ui::GraphPropertyEditForm *ui;

    Graph::GraphExtendedObject* m_currentGraph; //! Текущий граф

    QStandardItemModel* m_pCommonGraphInfoModel     {nullptr};  //! Модель с общими данными по графу
    QStandardItemModel* m_pUserGraphInfoModel       {nullptr};  //! Модель с пользовательскими данными по графу

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

}

#endif // GRAPHPROPERTYEDITFORM_H
