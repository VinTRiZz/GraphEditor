#include "grapheditorform.h"

#include <Common/ApplicationSettings.h>
#include <Common/CommonFunctions.h>
#include <Common/Logging.h>
#include <CustomWidgets/PasswordInsertDialog.h>
#include <Filework/SaveMaster.h>
#include <GraphObject/Maintainer.h>

#include <QFileDialog>
#include <QFileInfo>
#include <QMessageBox>
#include <QParallelAnimationGroup>
#include <QPropertyAnimation>

#include "ui_grapheditorform.h"

GraphEditorForm::GraphEditorForm(QWidget* parent)
    : QWidget(parent), ui(new Ui::GraphEditorForm) {
    ui->setupUi(this);
    ui->props_stackedWidget->setCurrentIndex(
        0);  // Чтобы редактировать UI без проблем

    ui->graphScene->startEditMode();

    m_graphMaintainer = Graph::GraphMaintainer::createInstance();
    m_graphMaintainer->setCreateTime(QDateTime::currentDateTime());

    ui->graphScene->setGraphMaintaner(m_graphMaintainer);
    ui->propertyEditForm->setCurrentGraph(m_graphMaintainer);
    ui->props_stackedWidget->hide();

    // Коннекты для редактора свойств объектов
    connect(ui->graphScene, &Graph::GraphEditView::openPropertyEditor, this,
            &GraphEditorForm::showObjectProperties);
    connect(ui->graphScene, &Graph::GraphEditView::closePropertyEditor, this,
            &GraphEditorForm::hideObjectProperties);

    updateCanvasSize();
}

GraphEditorForm::~GraphEditorForm() {
    delete ui;
}

bool GraphEditorForm::setGraph(const QString& saveFilePath) {
    return loadGraph(saveFilePath);
}

Graph::PMaintainer GraphEditorForm::getGraph() const {
    return m_graphMaintainer;
}

bool GraphEditorForm::getIsSavepathValid() const {
    if (m_lastSavePath.isEmpty()) {
        return false;
    }
    return QFileInfo(m_lastSavePath).exists();
}

QString GraphEditorForm::getSavefilePath() const {
    return m_lastSavePath;
}

bool GraphEditorForm::saveGraph(const QString& targetPath) {
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
        QMessageBox::critical(
            this, "Ошибка сохранения",
            "Возникла ошибка при сохранении данных.\nПроверьте: \nПрава "
            "доступа к директории;\nФакт её существования");
        return false;
    }

    ui->propertyEditForm->updateGraphInfo();
    return true;
}

bool GraphEditorForm::loadGraph(const QString& targetPath) {
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
        QMessageBox::critical(this, "Ошибка загрузки",
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

Graph::GraphSceneView *GraphEditorForm::getScene() const
{
    return ui->graphScene;
}

void GraphEditorForm::showGraphProperties() {
    ui->props_stackedWidget->setCurrentIndex(0);

    // Иначе будет дёргаться
    if (!ui->props_stackedWidget->isHidden()) {
        return;
    }
    CommonFunctions::showAnimatedHorizontal(ui->props_stackedWidget,
                                            m_propBarShowWidth);
}

void GraphEditorForm::hideGraphProperties() {
    ui->props_stackedWidget->setCurrentIndex(0);
    CommonFunctions::hideAnimatedHorizontal(ui->props_stackedWidget,
                                            m_propBarShowWidth);
}

void GraphEditorForm::showObjectProperties(QGraphicsItem* pTargetItem) {
    if (!dynamic_cast<ObjectViewItems::ItemBase*>(pTargetItem)) {
        throw std::invalid_argument("Invalid item to edit properties of");
    }
    ui->props_stackedWidget->setCurrentIndex(1);

    // Иначе будет дёргаться
    if (ui->props_stackedWidget->isHidden()) {
        CommonFunctions::showAnimatedHorizontal(ui->props_stackedWidget,
                                                m_propBarShowWidth);
    }
    ui->objectPropertyEditForm->setTargetItem(
        static_cast<ObjectViewItems::ItemBase*>(pTargetItem));
}

void GraphEditorForm::hideObjectProperties() {
    ui->props_stackedWidget->setCurrentIndex(1);
    CommonFunctions::hideAnimatedHorizontal(ui->props_stackedWidget,
                                            m_propBarShowWidth);
}

void GraphEditorForm::updateCanvasSize() {
    auto& appSettings = ApplicationSettings::getInstance();
    auto canvasSize = appSettings.getCanvasConfig().getCanvasSize();
    ui->graphScene->setCanvasRect(
        QRectF(0, 0, canvasSize.width(), canvasSize.height()));
}

void GraphEditorForm::updateCanvasGrid() {
    auto& appSettings = ApplicationSettings::getInstance();
    ui->graphScene->setGridEnabled(appSettings.getCanvasConfig().getIsGridEnabled());
    ui->graphScene->setGridSize(appSettings.getCanvasConfig().getGridSize());
}
