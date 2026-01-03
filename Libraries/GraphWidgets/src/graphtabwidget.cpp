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

                auto pGraph = pTargetForm->getGraph()->getObject();
                auto lastSaveDate = pGraph->getMetaInfo()->getEditTime();
                auto deltaDate = (QDateTime::currentDateTime().toMSecsSinceEpoch() - lastSaveDate.toMSecsSinceEpoch());
                if (deltaDate > 300'000) { // More than 5 min between saves
                    auto userResponse = QMessageBox::question(this,
                                                              "Требуется действие",
                                                              QString("Сохранить граф\n\"%0\"\nперед закрытием?").arg(pGraph->getMetaInfo()->getName()),
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
                auto GraphObjectManager = pForm->getGraph();
                ui->fileToolbar->setGraph(GraphObjectManager);
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

    pGraph->getObject()->synchronizeParents(pEditorForm);

    ui->editorForms_tabWidget->addTab(pEditorForm,
                                      pGraph->getObject()->getMetaInfo()->getName());
    ui->editorForms_tabWidget->setCurrentIndex(
        ui->editorForms_tabWidget->count() - 1);

    if (ui->editorForms_tabWidget->isHidden()) {
        ui->editorForms_tabWidget->show();
        ui->placeholder_label->hide();
    }

    auto metaInfo = pGraph->getObject()->getMetaInfo();
    connect(metaInfo,
            &Graph::GraphMetaInformation::dataChanged, this,
            [this, pEditorForm, metaInfo]() {
                for (int i = 0; i < ui->editorForms_tabWidget->count(); ++i) {
                    if (ui->editorForms_tabWidget->widget(i) == pEditorForm) {
                        ui->editorForms_tabWidget->setTabText(
                            i, metaInfo->getName());
                        break;
                    }
                }
            });
}

void GraphTabWidget::createGraph() {
    auto pEditorForm = new GraphEditView(this);
    auto pGraph = Graph::GraphObjectManager::createGraphInstance();
    pEditorForm->setGraph(pGraph);

    auto metaInfo = pGraph->getObject()->getMetaInfo();

    metaInfo->setName("Новый граф");
    metaInfo->setCreateTime(QDateTime::currentDateTime());

    ui->editorForms_tabWidget->addTab(pEditorForm,
                                      metaInfo->getName());
    ui->editorForms_tabWidget->setCurrentIndex(
        ui->editorForms_tabWidget->count() - 1);

    if (ui->editorForms_tabWidget->isHidden()) {
        ui->editorForms_tabWidget->show();
        ui->placeholder_label->hide();
    }

    connect(metaInfo,
            &Graph::GraphMetaInformation::dataChanged, this,
            [this, pEditorForm, metaInfo]() {
                for (int i = 0; i < ui->editorForms_tabWidget->count(); ++i) {
                    if (ui->editorForms_tabWidget->widget(i) == pEditorForm) {
                        ui->editorForms_tabWidget->setTabText(
                            i, metaInfo->getName());
                        break;
                    }
                }
            });

    ui->fileToolbar->setGraph(pGraph);
}

void GraphTabWidget::updatePluginList()
{
    for (auto& pPlugin : Graph::PluginMaster::getInstance().getAllPlugins()) {
        ui->pluginSelector_comboBox->addItem(pPlugin->getPluginName());
    }
}
