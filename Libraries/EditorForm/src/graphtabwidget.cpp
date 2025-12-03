#include "graphtabwidget.h"
#include "ui_graphtabwidget.h"

#include <Components/Common/ApplicationSettings.h>
#include <PluginModule/PluginMaster.h>
#include <QMessageBox>

#include <GraphWidgets/EditView.h>

GraphTabWidget::GraphTabWidget(QWidget* parent)
    : QWidget(parent), ui(new Ui::GraphTabWidget) {
    ui->setupUi(this);

    ui->manip_tabWidget->setCurrentIndex(0);  // Чтобы UI редактировать спокойно

    ui->editorForms_tabWidget->hide();
    ui->placeholder_label->show();

    connect(ui->fileToolbar, &FileManagementToolbar::createGraph,
            this, &GraphTabWidget::createGraph);

    connect(ui->fileToolbar, &FileManagementToolbar::loadedGraph,
            this, &GraphTabWidget::addTab);

    connect(ui->editorForms_tabWidget, &QTabWidget::tabCloseRequested, this,
            [this](int tabIndex) {
                auto pTargetForm = static_cast<GraphEditView*>(
                    ui->editorForms_tabWidget->widget(tabIndex));

                assert(nullptr != pTargetForm);

                auto pGraph = pTargetForm->getGraph();
                auto lastSaveDate = pGraph->getEditTime();
                auto deltaDate = (QDateTime::currentDateTime().toMSecsSinceEpoch() - lastSaveDate.toMSecsSinceEpoch());
                if (deltaDate > 300'000) { // More than 5 min between saves
                    auto userResponse = QMessageBox::question(this,
                                                              "Требуется действие",
                                                              QString("Сохранить граф\n\"%0\"\nперед закрытием?").arg(pGraph->getName()),
                                                              QMessageBox::StandardButton::Yes,
                                                              QMessageBox::StandardButton::No);
                    if (userResponse == QMessageBox::StandardButton::Yes) {

                        ui->fileToolbar->saveGraph();
                    }
                }

                ui->editorForms_tabWidget->removeTab(tabIndex);
                pTargetForm->deleteLater();

                if (ui->editorForms_tabWidget->count() == 0) {
                    ui->editorForms_tabWidget->hide();
                    ui->placeholder_label->show();
                    ui->fileToolbar->setGraph({});
                }
            });

    connect(ui->editorForms_tabWidget, &QTabWidget::currentChanged, this,
            [this](int tabIndex) {
                if (tabIndex == -1) { // Пустой виджет
                    return;
                }

                auto tabTargetWidget =
                    ui->editorForms_tabWidget->widget(tabIndex);

                assert((ui->editorForms_tabWidget->count() > 0) &&
                       (nullptr != tabTargetWidget));

                auto pForm = static_cast<GraphEditView*>(tabTargetWidget);
                auto pMaintainer = pForm->getGraph();
                ui->fileToolbar->setGraph(pMaintainer);
            });

    connect(ui->pluginSelector_comboBox, &QComboBox::currentTextChanged,
            this, [this](auto& curPluginName){
        ui->pluginInteractor->setCurrentPlugin(curPluginName);
        ui->graphItemsManager->setCurrentPlugin(curPluginName);
    });
    updatePluginList();
}

GraphTabWidget::~GraphTabWidget() {
    delete ui;
}

void GraphTabWidget::addTab(const QString& filePath) {
    auto pGraph = ui->fileToolbar->getGraph();

    auto pEditorForm = new GraphEditView(this);
    pEditorForm->setGraph(pGraph);

    ui->editorForms_tabWidget->addTab(pEditorForm,
                                      pGraph->getName());
    ui->editorForms_tabWidget->setCurrentIndex(
        ui->editorForms_tabWidget->count() - 1);

    if (ui->editorForms_tabWidget->isHidden()) {
        ui->editorForms_tabWidget->show();
        ui->placeholder_label->hide();
    }

    connect(pGraph.get(),
            &Graph::GraphMaintainer::changedCommonProperty, this,
            [this, pEditorForm]() {
                for (int i = 0; i < ui->editorForms_tabWidget->count(); ++i) {
                    if (ui->editorForms_tabWidget->widget(i) == pEditorForm) {
                        ui->editorForms_tabWidget->setTabText(
                            i, pEditorForm->getGraph()->getName());
                        break;
                    }
                }
            });
}

void GraphTabWidget::createGraph() {
    auto pEditorForm = new GraphEditView(this);
    auto pGraph = Graph::GraphMaintainer::createInstance();
    pEditorForm->setGraph(pGraph);

    pGraph->setToolTip("Новый граф");
    pGraph->setCreateTime(QDateTime::currentDateTime());

    ui->editorForms_tabWidget->addTab(pEditorForm,
                                      pGraph->getName());
    ui->editorForms_tabWidget->setCurrentIndex(
        ui->editorForms_tabWidget->count() - 1);

    if (ui->editorForms_tabWidget->isHidden()) {
        ui->editorForms_tabWidget->show();
        ui->placeholder_label->hide();
    }

    connect(pGraph.get(),
            &Graph::GraphMaintainer::changedCommonProperty, this,
            [this, pEditorForm]() {
                for (int i = 0; i < ui->editorForms_tabWidget->count(); ++i) {
                    if (ui->editorForms_tabWidget->widget(i) == pEditorForm) {
                        ui->editorForms_tabWidget->setTabText(
                            i, pEditorForm->getGraph()->getName());
                        break;
                    }
                }
    });
}

void GraphTabWidget::updatePluginList()
{
    for (auto& pPlugin : Graph::PluginMaster::getInstance().getAllPlugins()) {
        ui->pluginSelector_comboBox->addItem(pPlugin->getPluginName());
    }
}
