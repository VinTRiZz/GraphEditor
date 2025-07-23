#ifndef GRAPHPROPERTYEDITFORM_H
#define GRAPHPROPERTYEDITFORM_H

#include <QWidget>
#include <QStandardItemModel>

#include <GraphObject/ExtendedObject.h>

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

public slots:
    void showAnimated();
    void hideAnimated();

    /**
     * @brief updateEditTime Должно вызываться после любых изменений свойств графа
     */
    void updateEditTime();

private:
    Ui::GraphPropertyEditForm *ui;

    unsigned m_showWidth {400}; //! Значение ширины, которой будет окно после появления. Нужно для анимаций

    Graph::GraphExtendedObject* m_currentGraph; //! Текущий граф

    enum GraphCommonPropertyRows : int
    {
        NAMEROW = 0,
        DESCRIPTIONROW,
        CREATEDROW,
        EDITEDROW,
    };

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
