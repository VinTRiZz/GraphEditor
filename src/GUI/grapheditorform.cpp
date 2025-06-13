#include "grapheditorform.h"
#include "ui_grapheditorform.h"

#include <QFileDialog>
#include <QFileInfo>

#include "objectsceneconstants.h"
#include "graphobject.h"
#include "savemaster.h"
#include "graphcommon.h"
#include "graphconversion.h"

#include "logging.h"

GraphEditorForm::GraphEditorForm(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::GraphEditorForm)
{
    ui->setupUi(this);

    ui->graphScene->init();
    ui->graphScene->resizeScene(QSize(1000, 1000));
    ui->graphScene->scale(0.5, 0.5);

    setupSignals();
    setupModels();
}

GraphEditorForm::~GraphEditorForm()
{
    delete ui;
}

void GraphEditorForm::startValidanceTest()
{
    LOG_WARNING("Validance test not written");
}

Graph::GraphObject *GraphEditorForm::getCurrentGraph()
{
    return &m_currentGraph;
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
    // Обновить данные по пользовательским данным
    const int userPropNameCol = 0;
    const int userPropDataCol = 1;
    for (int row = 0; row < m_pUserGraphInfoModel->rowCount(); ++row) {
        m_currentGraph.setCustomValue(m_pUserGraphInfoModel->index(row, userPropNameCol).data().toString(),
                                        m_pUserGraphInfoModel->index(row, userPropDataCol).data());
    }

    m_currentGraph.setEditTime(QDateTime::currentDateTime());

    if (QFileInfo(m_currentGraphFilePath).suffix() != "gse") {
        m_currentGraphFilePath += ".gse";
    }

    auto saveSucceed = SaveMaster::save(m_currentGraphFilePath, m_currentGraph);
    if (!saveSucceed) {
        GraphCommon::showError("Ошибка сохранения графа");
        return;
    }
    ui->propertyLoad_pushButton->setEnabled(true);
}

void GraphEditorForm::loadGraph()
{
    auto loadSucceed = SaveMaster::load(m_currentGraphFilePath, m_currentGraph);
    if (!loadSucceed) {
        GraphCommon::showError("Ошибка загрузки графа");
        return;
    }
    m_currentGraph.setIdGenerator(ui->graphScene->getIdGenerator());

    if (m_currentGraphItem != nullptr) {
        ui->graphScene->removeObject(m_currentGraphItem->data(ObjectSceneConstants::OBJECTFIELD_ID).toUInt());
    }
    m_currentGraphItem = GraphConversion::toItem(m_currentGraph);
    ui->graphScene->addObject(m_currentGraphItem);
    updateGraphInfo();
}

void GraphEditorForm::setupSignals()
{
    connect(ui->graphProps_toggle_pushButton, &QPushButton::clicked,
            this, [this](){
        if (ui->graphProps_groupBox->isHidden()) {
            ui->graphProps_groupBox->show();
            ui->graphProps_toggle_pushButton->setText("Скрыть свойства графа");
            return;
        }

        ui->graphProps_groupBox->hide();
        ui->graphProps_toggle_pushButton->setText("Показать свойства графа");
    });
    ui->graphProps_toggle_pushButton->click(); // Чтобы UI текст заменился

    connect(ui->propertyAdd_pushButton, &QPushButton::clicked,
            this, [this]() {
        auto pItem = new QStandardItem("Моё свойство");
        pItem->setColumnCount(2);
        pItem->setChild(1, new QStandardItem("Моё значение"));
        m_pUserGraphInfoModel->appendRow(pItem);
    });

    connect(ui->propertySave_pushButton, &QPushButton::clicked,
            this, [this]() {
        if (!isGraphPathSet()) {
            return;
        }
        saveGraph();
        updateGraphInfo();
    });

    connect(ui->propertySaveAs_pushButton, &QPushButton::clicked,
            this, [this](){
        m_currentGraphFilePath = QFileDialog::getSaveFileName(this, "Файл для сохранения графа", QDir::homePath(), "Файл графа (*.gse)");
        if (m_currentGraphFilePath.isEmpty()) {
            return;
        }
        saveGraph();
        updateGraphInfo();
    });

    connect(ui->propertyLoad_pushButton, &QPushButton::clicked,
            this, [this]() {
        if (!isGraphPathSet()) {
            return;
        }
        loadGraph();
    });
    ui->propertyLoad_pushButton->setEnabled(false); // Нечего загружать на старте

    connect(ui->propertyLoadAs_pushButton, &QPushButton::clicked,
            this, [this]() {
        m_currentGraphFilePath = QFileDialog::getOpenFileName(this, "Файл сохранённого графа", QDir::homePath(), "Файл графа (*.gse)");
        if (m_currentGraphFilePath.isEmpty()) {
            return;
        }
        loadGraph();
    });
}

void GraphEditorForm::setupModels()
{
    if (m_pCommonGraphInfoModel == nullptr) {
        m_pCommonGraphInfoModel = new QStandardItemModel;
        m_pCommonGraphInfoModel->setColumnCount(2);
        m_pCommonGraphInfoModel->setHeaderData(0, Qt::Horizontal, "Свойство");
        m_pCommonGraphInfoModel->setHeaderData(1, Qt::Horizontal, "Значение");

        ui->propertyCommon_tableView->setModel(m_pCommonGraphInfoModel);
        ui->propertyCommon_tableView->verticalHeader()->hide();
        ui->propertyCommon_tableView->horizontalHeader()->setStretchLastSection(true);
        ui->propertyCommon_tableView->horizontalHeader()->setSizeAdjustPolicy(QHeaderView::AdjustToContents);
    }

    if (m_pUserGraphInfoModel == nullptr) {
        m_pUserGraphInfoModel = new QStandardItemModel;
        m_pUserGraphInfoModel->setColumnCount(2);
        m_pUserGraphInfoModel->setHeaderData(0, Qt::Horizontal, "Свойство");
        m_pUserGraphInfoModel->setHeaderData(1, Qt::Horizontal, "Значение");

        ui->propertyUser_tableView->setModel(m_pUserGraphInfoModel);
        ui->propertyUser_tableView->verticalHeader()->hide();
        ui->propertyUser_tableView->horizontalHeader()->setStretchLastSection(true);
        ui->propertyUser_tableView->horizontalHeader()->setSizeAdjustPolicy(QHeaderView::AdjustToContents);
    }
}

void GraphEditorForm::updateGraphInfo()
{
    // Очистка таким образом, чтобы не сбрасывать VIEW
    m_pCommonGraphInfoModel->removeRows(0, m_pCommonGraphInfoModel->rowCount());
    m_pUserGraphInfoModel->removeRows(0, m_pUserGraphInfoModel->rowCount());

    // Полу-хардкод
    auto pItem = new QStandardItem("Название");
    auto pProperyItem = new QStandardItem(m_currentGraph.getName());
    pItem->setEditable(false);
    m_pCommonGraphInfoModel->appendRow({pItem, pProperyItem});

    pItem = new QStandardItem("Описание");
    pProperyItem = new QStandardItem(m_currentGraph.getDescription());
    pItem->setEditable(false);
    m_pCommonGraphInfoModel->appendRow({pItem, pProperyItem});

    pItem = new QStandardItem("Создан");
    pProperyItem = new QStandardItem(m_currentGraph.getCreateTime().toString(GraphCommon::DATE_DISPLAY_CONVERSION_FORMAT));
    pItem->setEditable(false);
    m_pCommonGraphInfoModel->appendRow({pItem, pProperyItem});

    pItem = new QStandardItem("Изменён");
    pProperyItem = new QStandardItem(m_currentGraph.getEditTime().toString(GraphCommon::DATE_DISPLAY_CONVERSION_FORMAT));
    pItem->setEditable(false);
    m_pCommonGraphInfoModel->appendRow({pItem, pProperyItem});

    for (auto& [key, value] : m_currentGraph.getCustomValueMap()) {
        pItem = new QStandardItem(key);
        pProperyItem = new QStandardItem(value.toString());
        m_pUserGraphInfoModel->appendRow({pItem, pProperyItem});
    }

    LOG_INFO("Current graph data update");
}








