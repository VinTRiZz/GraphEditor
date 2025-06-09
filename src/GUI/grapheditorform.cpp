#include "grapheditorform.h"
#include "ui_grapheditorform.h"

#include <QFileDialog>

#include "graphobject.h"
#include "savemaster.h"
#include "graphcommon.h"

#include "logging.h"

Graph::GraphObject tmpGraph;

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

    tmpGraph.setIdGenerator(ui->graphScene->getIdGenerator());

    tmpGraph.setName("Мой тестовый граф");
    tmpGraph.setDescription("Описание для теста");
    tmpGraph.setCreateTime(QDateTime::currentDateTime());
    tmpGraph.setEditTime(QDateTime::currentDateTime());

    Graph::GVertex vert;
    vert.shortName = "Vertex 1";
    vert.backgroundColor = Qt::red;
    vert.posX = 100;
    vert.posY = 100;
    tmpGraph.addVertex(vert);

    vert.shortName = "Vertex 2";
    vert.borderColor = Qt::magenta;
    vert.backgroundColor = Qt::green;
    vert.posX = 100;
    vert.posY = 300;
    tmpGraph.addVertex(vert);

    vert.shortName = "Vertex 3";
    vert.backgroundColor = Qt::red;
    vert.posX = 300;
    vert.posY = 100;
    tmpGraph.addVertex(vert);

    vert.shortName = "Vertex 4";
    vert.borderColor = Qt::magenta;
    vert.backgroundColor = Qt::green;
    vert.posX = 300;
    vert.posY = 300;
    tmpGraph.addVertex(vert);

    Graph::GConnection con;
    con.name = "Connection 1";
    con.idFrom = 1;
    con.idTo = 2;
    tmpGraph.addConnection(con);

    con.name = "Connection 2";
    con.idFrom = 2;
    con.idTo = 3;
    tmpGraph.addConnection(con);

    con.name = "Connection 3";
    con.idFrom = 1;
    con.idTo = 3;
    tmpGraph.addConnection(con);

    con.name = "Connection 4";
    con.idFrom = 1;
    con.idTo = 4;
    tmpGraph.addConnection(con);

    con.name = "Connection 5";
    con.idFrom = 3;
    con.idTo = 1;
    tmpGraph.addConnection(con);
    setCurrentGraph(&tmpGraph);
}

GraphEditorForm::~GraphEditorForm()
{
    delete ui;
}

void GraphEditorForm::setCurrentGraph(Graph::GraphObject *pGraph)
{
    m_currentGraph = pGraph;

    if (pGraph != nullptr) {
        ui->graphScene->addObject(m_currentGraph->toItem());
        updateGraphInfo();
    }
}

Graph::GraphObject *GraphEditorForm::getCurrentGraph()
{
    return m_currentGraph;
}

void GraphEditorForm::saveGraph()
{
    // TODO: Save data
    m_currentGraph->setEditTime(QDateTime::currentDateTime());

    if (QFileInfo(m_currentGraphFilePath).suffix() != ".gse") {
        m_currentGraphFilePath += ".gse";
    }

    auto saveSucceed = SaveMaster::save(m_currentGraphFilePath, *m_currentGraph);
    if (!saveSucceed) {
        GraphCommon::showError("Ошибка сохранения графа");
    }
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
        if (m_currentGraphFilePath.isEmpty()) {
            // .gse --> Graph Save Extension
            m_currentGraphFilePath = QFileDialog::getSaveFileName(this, "Файл для сохранения графа", QDir::homePath(), "Файл графа (*.gse)");
        }

        if (m_currentGraphFilePath.isEmpty()) {
            GraphCommon::showWarning("Вы не выбрали путь.\nГраф не был сохранён");
            return;
        }
        saveGraph();
        updateGraphInfo();
    });

    connect(ui->propertySaveAs_pushButton, &QPushButton::clicked,
            this, [this](){
        // .gse --> Graph Save Extension
        m_currentGraphFilePath = QFileDialog::getSaveFileName(this, "Файл для сохранения графа", QDir::homePath(), "Файл графа (*.gse)");

        if (m_currentGraphFilePath.isEmpty()) {
            GraphCommon::showWarning("Вы не выбрали путь.\nГраф не был сохранён");
            return;
        }
        saveGraph();
        updateGraphInfo();
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

    if (m_currentGraph == nullptr) {
        return;
    }

    // Полу-хардкод
    auto pItem = new QStandardItem("Название");
    auto pProperyItem = new QStandardItem(m_currentGraph->getName());
    pItem->setEditable(false);
    m_pCommonGraphInfoModel->appendRow({pItem, pProperyItem});

    pItem = new QStandardItem("Описание");
    pProperyItem = new QStandardItem(m_currentGraph->getDescription());
    pItem->setEditable(false);
    m_pCommonGraphInfoModel->appendRow({pItem, pProperyItem});

    pItem = new QStandardItem("Создан");
    pProperyItem = new QStandardItem(m_currentGraph->getCreateTime().toString(GraphCommon::DATE_DISPLAY_CONVERSION_FORMAT));
    pItem->setEditable(false);
    m_pCommonGraphInfoModel->appendRow({pItem, pProperyItem});

    pItem = new QStandardItem("Изменён");
    pProperyItem = new QStandardItem(m_currentGraph->getEditTime().toString(GraphCommon::DATE_DISPLAY_CONVERSION_FORMAT));
    pItem->setEditable(false);
    m_pCommonGraphInfoModel->appendRow({pItem, pProperyItem});

    for (auto& [key, value] : m_currentGraph->getCustomValueMap()) {
        pItem = new QStandardItem;
        pItem->setData(key, Qt::DisplayRole);
        pItem->setColumnCount(2);
        pItem->setChild(1, new QStandardItem(value.toString()));
        m_pUserGraphInfoModel->appendRow(pItem);
    }

    LOG_INFO("Current graph data update");
}








