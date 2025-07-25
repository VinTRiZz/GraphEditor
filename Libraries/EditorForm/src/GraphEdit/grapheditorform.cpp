#include "grapheditorform.h"
#include "ui_grapheditorform.h"

#include <QFileDialog>
#include <QFileInfo>

#include <GraphObject/Object.h>
#include <Filework/SaveMaster.h>

#include <QParallelAnimationGroup>
#include <QPropertyAnimation>

#include <Common/Logging.h>

GraphEditorForm::GraphEditorForm(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::GraphEditorForm)
{
    ui->setupUi(this);
}

GraphEditorForm::~GraphEditorForm()
{
    delete ui;
}

void GraphEditorForm::init()
{
    ui->graphScene->init();
    ui->graphScene->startEditMode();

    m_currentGraph = new Graph::GraphExtendedObject(this);
    m_currentGraph->setCreateTime(QDateTime::currentDateTime());
    ui->graphScene->setCurrentGraph(m_currentGraph);

    setupWidget();
    setupSignals();

    ui->graphScene->setCurrentGraph(m_currentGraph);
    ui->propertyEditForm->setCurrentGraph(m_currentGraph);
    ui->propertyEditForm->hide();
}

void GraphEditorForm::startValidanceTest()
{
    SaveMaster::testValidance();
    LOG_WARNING("GraphEditorForm: Validance test not written");
}

bool GraphEditorForm::isGraphPathSet()
{
    if (!m_currentGraphFilePath.isEmpty() && QFileInfo(m_currentGraphFilePath).exists()) {
        return true;
    }

    // .gse --> Graph Save Extension
    m_currentGraphFilePath = QFileDialog::getSaveFileName(this, "Файл для сохранения графа", QDir::homePath(), "Файл графа (*.gse)");

    if (m_currentGraphFilePath.isEmpty()) {
        return false;
    }
    return true;
}

void GraphEditorForm::saveGraph()
{
    if (m_currentGraphFilePath.isEmpty()) {
        LOG_WARNING("Save failed: empty path");
        return;
    }

    m_currentGraph->setEditTime(QDateTime::currentDateTime());

    if (QFileInfo(m_currentGraphFilePath).suffix() != "gse") {
        m_currentGraphFilePath += ".gse";
    }

    ui->graphScene->writeChangesToGraph();

    auto saveSucceed = SaveMaster::save(m_currentGraphFilePath, m_currentGraph);
    if (!saveSucceed) {
        GraphCommon::showError("Ошибка сохранения графа");
        return;
    }

    ui->propertyEditForm->updateGraphInfo();
}

void GraphEditorForm::loadGraph()
{
    if (m_currentGraphFilePath.isEmpty()) {
        LOG_WARNING("Load failed: empty path");
        return;
    }

    auto loadSucceed = SaveMaster::load(m_currentGraphFilePath, m_currentGraph);
    if (!loadSucceed) {
        GraphCommon::showError("Ошибка загрузки графа");
        return;
    }

    ui->propertyEditForm->updateGraphInfo();
    ui->graphScene->updateGraph();
}

void GraphEditorForm::setupSignals()
{
    connect(ui->buttonToolbar, &GraphToolbar::showProperties,
            ui->propertyEditForm, &Graph::GraphPropertyEditForm::showAnimated);

    connect(ui->buttonToolbar, &GraphToolbar::hideProperties,
            ui->propertyEditForm, &Graph::GraphPropertyEditForm::hideAnimated);

    connect(ui->buttonToolbar, &GraphToolbar::saveGraph,
            this, [this](const QString& filePath){
        if (!filePath.isEmpty()) { // Негласное соглашение
            m_currentGraphFilePath = filePath;
        }
        saveGraph();
    });

    connect(ui->buttonToolbar, &GraphToolbar::loadGraph,
            this, [this](const QString& filePath){
        if (!filePath.isEmpty()) { // Негласное соглашение
            m_currentGraphFilePath = filePath;
        }
        loadGraph();
    });

    connect(ui->buttonToolbar, &GraphToolbar::changeMode,
            ui->graphScene, [this](){
        if (ui->graphScene->isEditMode()) {
            ui->graphScene->startViewMode();
        } else if (ui->graphScene->isViewMode()) {
            ui->graphScene->startEditMode();
        }
    });
}

void GraphEditorForm::setupWidget()
{

}
