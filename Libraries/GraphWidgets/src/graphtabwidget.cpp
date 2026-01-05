#include "graphtabwidget.h"
#include "ui_graphtabwidget.h"

#include <Components/Logger/Logger.h>
#include <Components/Common/ApplicationSettings.h>
#include <Components/Common/DirectoryManager.h>
#include <Components/Database/SQlite.h>

#include <PluginModule/PluginMaster.h>
#include <GraphWidgets/EditView.h>

#include <QMessageBox>
#include <QFileInfo>

GraphTabWidget::GraphTabWidget(QWidget* parent)
    : QWidget(parent), ui(new Ui::GraphTabWidget) {
    ui->setupUi(this);

    ui->manip_tabWidget->setCurrentIndex(0);  // Чтобы UI редактировать спокойно

    ui->editorForms_tabWidget->hide();
    ui->placeholder_label->show();

    connect(ui->fileToolbar, &FileManagementToolbar::createGraph,
            this, &GraphTabWidget::createGraph);

    connect(ui->fileToolbar, &FileManagementToolbar::loadedGraph,
            this, [this](const QString& filePath){
        if (ui->editorForms_tabWidget->count() == 0) {
            return addTab(filePath);
        }

        auto pTargetForm = static_cast<GraphEditView*>(ui->editorForms_tabWidget->currentWidget());
        if (ui->fileToolbar->getGraph() != pTargetForm->getGraph()) {
            addTab(filePath);
        } else {
            pTargetForm->getGraph()->getObject()->synchronizeParents(pTargetForm);
        }
    });

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

    auto filesTable = getRecentFilesTable();
    for (auto& row : filesTable.getRow({"file_path"}, {}, "ID ASC")) {
        auto filePath = std::get<Database::DBCellString>(row[0]).value();
        auto recfile = QByteArray::fromHex(QByteArray::fromStdString(filePath));
        if (QFileInfo(recfile).exists()) {
            auto pGraph = Graph::GraphObjectManager::createGraphInstance();
            ui->fileToolbar->setGraph(pGraph);
            ui->fileToolbar->loadGraph(recfile);
        }
    }
}

GraphTabWidget::~GraphTabWidget() {

    // Сохраняем как временные все те, что были на вкладках
    saveTemporaryGraphs();

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

void GraphTabWidget::saveTemporaryGraphs()
{
    auto filesTable = getRecentFilesTable();
    filesTable.removeRow(); // Чистим столбцы

    for (int i = 0; i < ui->editorForms_tabWidget->count(); ++i) {
        auto pTargetForm = static_cast<GraphEditView*>(
            ui->editorForms_tabWidget->widget(i));

        auto pCurrentGraph = pTargetForm->getGraph();
        ui->fileToolbar->setGraph(pCurrentGraph);
        if (pCurrentGraph && pCurrentGraph->getObject()) {
            auto pGraph = pCurrentGraph->getObject();
            if (pGraph->getMetaInfo()->getSavepath().isEmpty()) {
                ui->fileToolbar->saveGraphAsTemporary();
            }
        }

        auto savePath = pCurrentGraph->getObject()->getMetaInfo()->getSavepath().toUtf8().toHex().toStdString();
        filesTable.addRow(std::map<std::string, Database::DBCell>{ {"file_path", savePath} });
    }
}

Database::SQLiteTable GraphTabWidget::getRecentFilesTable() const
{
    // TODO: Вынести в некий общий класс

    Database::SQLiteDatabase cacheDb;
    auto dbFilepath = Common::DirectoryManager::getDirectoryStatic(Common::DirectoryManager::DirectoryType::Temporary).absolutePath() + QDir::separator() + "cache.db";
    cacheDb.setDatabase(dbFilepath.toStdString());
    Database::SQLiteTable filesTable(cacheDb);

    filesTable.setTable("recent_files");
    if (!filesTable.isTableExist()) {
        std::list<Database::SQLiteTable::ColumnInfo> cols;

        Database::SQLiteTable::ColumnInfo col;
        col.name = "id";
        col.isPrimaryKey = true;
        col.canBeNull = false;
        col.type = Database::CT_INTEGER;
        cols.push_back(col);

        col = {};
        col.name = "file_path";
        col.canBeNull = false;
        col.type = Database::CT_TEXT;
        cols.push_back(col);

        filesTable.create(cols);
    }
    return filesTable;
}
