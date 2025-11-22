#pragma once

#include <GraphObject/Maintainer.h>

#include <QStandardItemModel>
#include <QWidget>

namespace Ui {
class GraphPropertyEditForm;
}

namespace Graph {

class GraphCommonPropertiesModel;
class GraphCustomPropertiesModel;

/**
 * @brief The GraphPropertyEditForm class   Форма редактирования метаинформации
 * о графе
 */
class GraphPropertyEditForm :
        public QWidget,
        public MaintainerUserDecorator {
    Q_OBJECT

public:
    explicit GraphPropertyEditForm(QWidget* parent = nullptr);
    ~GraphPropertyEditForm();

private:
    Ui::GraphPropertyEditForm* ui;

    void processGraphChange() override;

    enum GraphCommonPropertyRows : int {
        NAMEROW = 0,
        DESCRIPTIONROW,
        CREATEDROW,
        EDITEDROW,
    };

    bool m_isSettingGraph{false};  //! Для задания графов без удаления их
                                   //! свойств (особенности логики)
    GraphCommonPropertiesModel* m_pCommonPropertiesModel{
        nullptr};  //! Модель с общими данными по графу
    GraphCustomPropertiesModel* m_pCustomPropertiesModel{
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
