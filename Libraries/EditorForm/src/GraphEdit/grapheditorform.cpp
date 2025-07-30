#include "grapheditorform.h"
#include "ui_grapheditorform.h"

#include <QFileDialog>
#include <QFileInfo>
#include <QMessageBox>

#include <GraphObject/Maintainer.h>
#include <Filework/SaveMaster.h>

#include <QParallelAnimationGroup>
#include <QPropertyAnimation>

#include <Common/Logging.h>

#include <CustomWidgets/PasswordInsertDialog.h>

GraphEditorForm::GraphEditorForm(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::GraphEditorForm)
{
    ui->setupUi(this);

    ui->graphScene->init();
    ui->graphScene->startEditMode();

    m_graphMaintainer = Graph::GraphMaintainer::createInstance();

    m_graphMaintainer->setCreateTime(QDateTime::currentDateTime());

    ui->graphScene->setGraphMaintaner(m_graphMaintainer);
    ui->propertyEditForm->setCurrentGraph(m_graphMaintainer);
    ui->propertyEditForm->hide();
}

GraphEditorForm::~GraphEditorForm()
{
    delete ui;
}

bool GraphEditorForm::setGraph(const QString &saveFilePath)
{
    return loadGraph(saveFilePath);
}

Graph::PMaintainer GraphEditorForm::getGraph() const
{
    return m_graphMaintainer;
}

bool GraphEditorForm::getIsSavepathValid() const
{
    if (m_lastSavePath.isEmpty()) {
        return false;
    }
    return QFileInfo(m_lastSavePath).exists();
}

bool GraphEditorForm::saveGraph(const QString& targetPath)
{
    // СОГЛАШЕНИЕ: Если путь не задан, то сохранить по последнему заданному
    if (!targetPath.isEmpty()) {
        m_lastSavePath = targetPath;
    }
    if (!QFileInfo(targetPath).dir().exists()) {
        LOG_ERROR("Can not saving file with path:", m_lastSavePath);
        return false;
    }

    m_graphMaintainer->setEditTime(QDateTime::currentDateTime());

    ui->graphScene->writeChangesToGraph();

    SaveMaster saveMaster;
    auto saveSucceed = saveMaster.save(m_lastSavePath, m_graphMaintainer);
    if (!saveSucceed) {
        QMessageBox::critical(this, "Ошибка сохранения", "Возникла ошибка при сохранении данных.\nПроверьте: \nПрава доступа к директории;\nФакт её существования");
        return false;
    }

    ui->propertyEditForm->updateGraphInfo();
    return true;
}

bool GraphEditorForm::loadGraph(const QString& targetPath)
{
    // СОГЛАШЕНИЕ: Если путь не задан, то загрузить по последнему заданному
    if (!targetPath.isEmpty()) {
        m_lastSavePath = targetPath;
    }
    if (!getIsSavepathValid()) {
        LOG_ERROR("Can not loading file with path:", m_lastSavePath);
        return false;
    }

    SaveMaster saveMaster;
    auto loadSucceed = saveMaster.load(m_lastSavePath, m_graphMaintainer);
    if (!loadSucceed) {
        QMessageBox::critical(
                    this,
                    "Ошибка загрузки",
R"(Возникла ошибка при сохранении данных.
Проверьте:
Права доступа к директории;
Факт её существования;
Корректность формата файла;
Введённый пароль (если это зашифрованный формат) )");
        return false;
    }

    ui->propertyEditForm->updateGraphInfo();
    ui->graphScene->updateGraph();
    return true;
}

void GraphEditorForm::setEditMode()
{
    ui->graphScene->startEditMode();
}

void GraphEditorForm::setViewMode()
{
    ui->graphScene->startViewMode();
}

void GraphEditorForm::setAlgoMode()
{

}

void GraphEditorForm::setCrimeMode()
{

}

void GraphEditorForm::showProperties()
{
    ui->propertyEditForm->showAnimated();
}

void GraphEditorForm::hideProperties()
{
    ui->propertyEditForm->hideAnimated();
}
