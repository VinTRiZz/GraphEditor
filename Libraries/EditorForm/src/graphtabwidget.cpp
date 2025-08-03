#include "graphtabwidget.h"

#include <Common/ApplicationSettings.h>
#include <Filework/SaveMaster.h>

#include <QMessageBox>

#include "grapheditorform.h"
#include "ui_graphtabwidget.h"

GraphTabWidget::GraphTabWidget(QWidget* parent)
    : QWidget(parent), ui(new Ui::GraphTabWidget) {
    ui->setupUi(this);

    ui->stackedWidget->setCurrentIndex(0);  // Чтобы UI редактировать спокойно

    ui->filesToolBar->setSaveAsEnabled(false);
    ui->filesToolBar->setShowPropertiesEnabled(false);
    ui->filesToolBar->setSaveEnabled(false);
    ui->filesToolBar->setLoadEnabled(false);

    ui->editorForms_tabWidget->hide();
    ui->placeholder_label->show();

    connect(ui->editorForms_tabWidget, &QTabWidget::tabCloseRequested, this,
            [this](int tabIndex) {
                auto tabTargetWidget =
                    ui->editorForms_tabWidget->widget(tabIndex);
                removeTab(static_cast<GraphEditorForm*>(tabTargetWidget)
                              ->getGraph()
                              ->getName());
            });

    connect(ui->editorForms_tabWidget, &QTabWidget::currentChanged, this,
            [this](int tabIndex) {
                disconnect(ui->filesToolBar, &GraphFilesToolbar::showProperties,
                           nullptr, nullptr);
                disconnect(ui->filesToolBar, &GraphFilesToolbar::hideProperties,
                           nullptr, nullptr);

                auto tabTargetWidget =
                    ui->editorForms_tabWidget->widget(tabIndex);
                auto pForm = static_cast<GraphEditorForm*>(tabTargetWidget);
                if (nullptr == pForm) {
                    return;
                }

                auto isSavesEnabled = pForm->getIsSavepathValid();
                ui->filesToolBar->setSaveEnabled(isSavesEnabled);
                ui->filesToolBar->setLoadEnabled(isSavesEnabled);

                connect(ui->filesToolBar, &GraphFilesToolbar::showProperties,
                        pForm, &GraphEditorForm::showGraphProperties);
                connect(ui->filesToolBar, &GraphFilesToolbar::hideProperties,
                        pForm, &GraphEditorForm::hideGraphProperties);
            });

    connect(ui->filesToolBar, &GraphFilesToolbar::createGraph, this,
            &GraphTabWidget::createGraph);

    connect(ui->filesToolBar, &GraphFilesToolbar::saveGraph, this,
            &GraphTabWidget::saveVisibleGraph);

    connect(ui->filesToolBar, &GraphFilesToolbar::loadGraph, this,
            &GraphTabWidget::loadVisibleGraph);

    auto& settingsInstance = ApplicationSettings::getInstance();
    for (auto& recentFile : settingsInstance.getRecentOpenFiles()) {
        addTab(recentFile);
    }

    connect(ui->optionsToolBar, &GraphOptionsToolbar::openSettings, this,
            [this]() {
                if (ui->stackedWidget->currentIndex() == 0) {
                    ui->stackedWidget->setCurrentIndex(1);
                } else {
                    ui->stackedWidget->setCurrentIndex(0);
                }
            });

    connect(ui->settingsForm, &GraphEditorSettingsForm::updateCanvasSize, this,
            [this]() {
                for (int i = 0; i < ui->editorForms_tabWidget->count(); ++i) {
                    auto tabTargetWidget = ui->editorForms_tabWidget->widget(i);
                    static_cast<GraphEditorForm*>(tabTargetWidget)
                        ->updateCanvasSize();
                }
            });

    m_saveTimer.connect(&m_saveTimer, &QTimer::timeout, [this]() {
        for (int i = 0; i < ui->editorForms_tabWidget->count(); ++i) {
            auto tabTargetWidget = ui->editorForms_tabWidget->widget(i);
            static_cast<GraphEditorForm*>(tabTargetWidget)->saveGraph();
        }
        auto savetimeSec =
            ApplicationSettings::getInstance().getAutoSaveInterval();
        LOG_INFO("Restarting savefile timer. Time left:", savetimeSec / 60,
                 "minutes");
        m_saveTimer.start(savetimeSec * 1000);
    });
    auto savetimeSec = ApplicationSettings::getInstance().getAutoSaveInterval();
    LOG_INFO("Restarting savefile timer. Time left:", savetimeSec / 60,
             "minutes");
    m_saveTimer.start(savetimeSec * 1000);
}

GraphTabWidget::~GraphTabWidget() {
    delete ui;
}

void GraphTabWidget::addTab(const QString& filePath) {
    auto pEditorForm = new GraphEditorForm(this);
    if (!pEditorForm->setGraph(filePath)) {
        pEditorForm->deleteLater();
        return;
    }
    ui->editorForms_tabWidget->addTab(pEditorForm,
                                      pEditorForm->getGraph()->getName());
    ui->editorForms_tabWidget->setCurrentIndex(
        ui->editorForms_tabWidget->count() - 1);

    connect(pEditorForm->getGraph().get(),
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

    ui->filesToolBar->setShowPropertiesEnabled(true);
    ui->filesToolBar->setSaveAsEnabled(true);

    if (ui->editorForms_tabWidget->isHidden()) {
        ui->editorForms_tabWidget->show();
        ui->placeholder_label->hide();
    }

    ApplicationSettings::getInstance().addRecentFile(filePath);
}

void GraphTabWidget::removeTab(const QString& graphName) {
    GraphEditorForm* pTargetForm{nullptr};
    int targetIndex{0};

    for (int i = 0; i < ui->editorForms_tabWidget->count(); ++i) {
        auto tabTargetWidget = ui->editorForms_tabWidget->widget(i);
        if (static_cast<GraphEditorForm*>(tabTargetWidget)
                ->getGraph()
                ->getName() == graphName) {
            targetIndex = i;
            pTargetForm = static_cast<GraphEditorForm*>(tabTargetWidget);
            break;
        }
    }

    if (nullptr == pTargetForm) {
        LOG_ERROR("Did not found target text to remove tab!");
        return;
    }

    if (ApplicationSettings::getInstance().getNeedConfirmDeletion() &&
        pTargetForm->getIsSavepathValid()) {
        auto userResponse = QMessageBox::warning(
            this, "Внимание!", "Сохранить перед закрытием?",
            QMessageBox::StandardButton::Yes | QMessageBox::StandardButton::No);
        if (userResponse == QMessageBox::StandardButton::Yes) {
            pTargetForm->saveGraph();
        }
    }
    ui->editorForms_tabWidget->removeTab(targetIndex);
    ApplicationSettings::getInstance().removeRecentFile(
        pTargetForm->getSavefilePath());
    delete pTargetForm;

    if (ui->editorForms_tabWidget->count() == 0) {
        ui->editorForms_tabWidget->hide();
        ui->placeholder_label->show();
        ui->filesToolBar->setSaveAsEnabled(false);
        ui->filesToolBar->setSaveEnabled(false);
        ui->filesToolBar->setLoadEnabled(false);
        ui->filesToolBar->setShowPropertiesEnabled(false);
    }
}

void GraphTabWidget::createGraph() {
    auto pEditorForm = new GraphEditorForm(this);
    pEditorForm->getGraph()->setName("Новый граф");
    pEditorForm->getGraph()->setCreateTime(QDateTime::currentDateTime());

    ui->editorForms_tabWidget->addTab(pEditorForm,
                                      pEditorForm->getGraph()->getName());
    ui->editorForms_tabWidget->setCurrentIndex(
        ui->editorForms_tabWidget->count() - 1);

    connect(pEditorForm->getGraph().get(),
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

    ui->filesToolBar->setShowPropertiesEnabled(true);
    ui->filesToolBar->setSaveAsEnabled(true);

    if (ui->editorForms_tabWidget->isHidden()) {
        ui->editorForms_tabWidget->show();
        ui->placeholder_label->hide();
    }
}

void GraphTabWidget::saveVisibleGraph(const QString& filePath) {
    auto tabTargetWidget = ui->editorForms_tabWidget->currentWidget();
    auto pForm = static_cast<GraphEditorForm*>(tabTargetWidget);
    pForm->saveGraph(filePath);

    ui->filesToolBar->setLoadEnabled(true);
    ui->filesToolBar->setSaveEnabled(true);
}

void GraphTabWidget::loadVisibleGraph(const QString& filePath) {
    // Соглашение
    if (!filePath.isEmpty()) {
        addTab(filePath);
        return;
    }

    auto tabTargetWidget = ui->editorForms_tabWidget->currentWidget();
    auto pForm = static_cast<GraphEditorForm*>(tabTargetWidget);
    pForm->loadGraph(filePath);
}
