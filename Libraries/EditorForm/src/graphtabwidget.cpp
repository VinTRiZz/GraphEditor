#include "graphtabwidget.h"
#include "ui_graphtabwidget.h"

#include "grapheditorform.h"

#include <Filework/SaveMaster.h>
#include <Common/ApplicationSettings.h>

#include <QMessageBox>

GraphTabWidget::GraphTabWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::GraphTabWidget)
{
    ui->setupUi(this);

    ui->buttonToolbar->setSaveAsEnabled(false);
    ui->buttonToolbar->setShowPropertiesEnabled(false);
    ui->buttonToolbar->setSaveEnabled(false);
    ui->buttonToolbar->setLoadEnabled(false);

    ui->editorForms_tabWidget->hide();
    ui->placeholder_label->show();

    connect(ui->editorForms_tabWidget, &QTabWidget::tabCloseRequested,
            this, [this](int tabIndex){
        auto tabTargetWidget = ui->editorForms_tabWidget->widget(tabIndex);
        removeTab(static_cast<GraphEditorForm*>(tabTargetWidget)->getGraph()->getName());
    });

    connect(ui->editorForms_tabWidget, &QTabWidget::currentChanged,
            this, [this](int tabIndex){
        disconnect(ui->buttonToolbar, &GraphToolbar::showProperties, nullptr, nullptr);
        disconnect(ui->buttonToolbar, &GraphToolbar::hideProperties, nullptr, nullptr);

        auto tabTargetWidget = ui->editorForms_tabWidget->widget(tabIndex);
        auto pForm = static_cast<GraphEditorForm*>(tabTargetWidget);
        if (nullptr == pForm) {
            return;
        }

        auto isSavesEnabled = pForm->getIsSavepathValid();
        ui->buttonToolbar->setSaveEnabled(isSavesEnabled);
        ui->buttonToolbar->setLoadEnabled(isSavesEnabled);

        connect(ui->buttonToolbar, &GraphToolbar::showProperties, pForm, &GraphEditorForm::showProperties);
        connect(ui->buttonToolbar, &GraphToolbar::hideProperties, pForm, &GraphEditorForm::hideProperties);
    });

    connect(ui->buttonToolbar, &GraphToolbar::createGraph,
            this, &GraphTabWidget::createGraph);

    connect(ui->buttonToolbar, &GraphToolbar::saveGraph,
            this, &GraphTabWidget::saveVisibleGraph);

    connect(ui->buttonToolbar, &GraphToolbar::loadGraph,
            this, &GraphTabWidget::loadVisibleGraph);

    auto& settingsInstance = ApplicationSettings::getInstance();
    for (auto& recentFile : settingsInstance.getRecentFiles()) {
        addTab(recentFile);
    }
}

GraphTabWidget::~GraphTabWidget()
{
    delete ui;
}

void GraphTabWidget::addTab(const QString &filePath)
{
    auto pEditorForm = new GraphEditorForm(this);
    if (!pEditorForm->setGraph(filePath)) {
        pEditorForm->deleteLater();
        return;
    }
    ui->editorForms_tabWidget->addTab(pEditorForm, pEditorForm->getGraph()->getName());
    ui->editorForms_tabWidget->setCurrentIndex(ui->editorForms_tabWidget->count() - 1);

    connect(pEditorForm->getGraph().get(), &Graph::GraphMaintainer::changedCommonProperty,
            this, [this, pEditorForm](){
        for (int i = 0; i < ui->editorForms_tabWidget->count(); ++i) {
            if (ui->editorForms_tabWidget->widget(i) == pEditorForm) {
                ui->editorForms_tabWidget->setTabText(i, pEditorForm->getGraph()->getName());
                break;
            }
        }
    });

    ui->buttonToolbar->setShowPropertiesEnabled(true);
    ui->buttonToolbar->setSaveAsEnabled(true);

    if (ui->editorForms_tabWidget->isHidden()) {
        ui->editorForms_tabWidget->show();
        ui->placeholder_label->hide();
    }

    ApplicationSettings::getInstance().addRecentFile(filePath);
}

void GraphTabWidget::removeTab(const QString &graphName)
{
    GraphEditorForm* pTargetForm {nullptr};
    int targetIndex {0};

    for (int i = 0; i < ui->editorForms_tabWidget->count(); ++i) {
        auto tabTargetWidget = ui->editorForms_tabWidget->widget(i);
        if (static_cast<GraphEditorForm*>(tabTargetWidget)->getGraph()->getName() == graphName) {
            targetIndex = i;
            pTargetForm = static_cast<GraphEditorForm*>(tabTargetWidget);
            break;
        }
    }

    if (nullptr == pTargetForm) {
        LOG_ERROR("Did not found target text to remove tab!");
        return;
    }

    auto userResponse = QMessageBox::warning(this, "Внимание!", "Сохранить перед закрытием?", QMessageBox::StandardButton::Yes | QMessageBox::StandardButton::No);
    if (userResponse == QMessageBox::StandardButton::Yes) {
        if (!pTargetForm->getIsSavepathValid()) {
            auto targetPath = SaveMaster::getSavePath();
            pTargetForm->saveGraph(targetPath);
        } else {
            pTargetForm->saveGraph();
        }
    }
    ui->editorForms_tabWidget->removeTab(targetIndex);
    ApplicationSettings::getInstance().removeRecentFile(pTargetForm->getSavefilePath());
    delete pTargetForm;

    if (ui->editorForms_tabWidget->count() == 0) {
        ui->editorForms_tabWidget->hide();
        ui->placeholder_label->show();
        ui->buttonToolbar->setSaveAsEnabled(false);
        ui->buttonToolbar->setSaveEnabled(false);
        ui->buttonToolbar->setLoadEnabled(false);
        ui->buttonToolbar->setShowPropertiesEnabled(false);
    }
}

void GraphTabWidget::createGraph()
{
    auto pEditorForm = new GraphEditorForm(this);
    pEditorForm->getGraph()->setName("Новый граф");
    pEditorForm->getGraph()->setCreateTime(QDateTime::currentDateTime());

    ui->editorForms_tabWidget->addTab(pEditorForm, pEditorForm->getGraph()->getName());
    ui->editorForms_tabWidget->setCurrentIndex(ui->editorForms_tabWidget->count() - 1);

    connect(pEditorForm->getGraph().get(), &Graph::GraphMaintainer::changedCommonProperty,
            this, [this, pEditorForm](){
        for (int i = 0; i < ui->editorForms_tabWidget->count(); ++i) {
            if (ui->editorForms_tabWidget->widget(i) == pEditorForm) {
                ui->editorForms_tabWidget->setTabText(i, pEditorForm->getGraph()->getName());
                break;
            }
        }
    });

    ui->buttonToolbar->setShowPropertiesEnabled(true);
    ui->buttonToolbar->setSaveAsEnabled(true);

    if (ui->editorForms_tabWidget->isHidden()) {
        ui->editorForms_tabWidget->show();
        ui->placeholder_label->hide();
    }
}

void GraphTabWidget::saveVisibleGraph(const QString &filePath)
{
    auto tabTargetWidget = ui->editorForms_tabWidget->currentWidget();
    auto pForm = static_cast<GraphEditorForm*>(tabTargetWidget);
    pForm->saveGraph(filePath);

    ui->buttonToolbar->setLoadEnabled(true);
    ui->buttonToolbar->setSaveEnabled(true);
}

void GraphTabWidget::loadVisibleGraph(const QString &filePath)
{
    // Соглашение
    if (!filePath.isEmpty()) {
        addTab(filePath);
        return;
    }

    auto tabTargetWidget = ui->editorForms_tabWidget->currentWidget();
    auto pForm = static_cast<GraphEditorForm*>(tabTargetWidget);
    pForm->loadGraph(filePath);
}

