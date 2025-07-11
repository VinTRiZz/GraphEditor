#include "grapheditorform.h"
#include "ui_grapheditorform.h"

#include <QFileDialog>
#include <QFileInfo>

#include "Graph/graphobject.h"
#include "Graph/graphcommon.h"
#include "Filework/savemaster.h"

#include <QParallelAnimationGroup>
#include <QPropertyAnimation>

#include "logging.h"

#include "Testing/testgraphgenerator.h"

GraphEditorForm::GraphEditorForm(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::GraphEditorForm)
{
    ui->setupUi(this);

    ui->graphScene->init();

    m_currentGraph = new Graph::GraphExtendedObject(this);
    ui->graphScene->setCurrentGraph(m_currentGraph);

    m_currentGraph->setName("Test example graph");
    m_currentGraph->setDescription("Example description");
    m_currentGraph->setCreateTime(QDateTime::currentDateTime());
    m_currentGraph->setEditTime(QDateTime::currentDateTime());

//    *m_currentGraph = TestGraphGenerator::generateGraph(10);

    Graph::GVertex vert;
    vert.shortName = "Дебич";
    vert.name = "Дебич узел";
    vert.backgroundColor = Qt::green;
    vert.pxmap = QIcon("://DATA/images/vertexicons/vertex_person_red.png").pixmap(500, 500);
    vert.posX = 100;
    vert.posY = 100;
    m_currentGraph->addVertex(vert);

    vert.shortName = "Кр. дебич";
    vert.name = "Кр. дебич узел";
    vert.backgroundColor = Qt::red;
    vert.pxmap = QIcon("://DATA/images/vertexicons/vertex_person_red.png").pixmap(500, 500);
    vert.posX = 300;
    vert.posY = 300;
    m_currentGraph->addVertex(vert);

    vert.shortName = "Др. дебич";
    vert.name = "Др. дебич узел";
    vert.backgroundColor = Qt::green;
    vert.pxmap = QIcon("://DATA/images/vertexicons/vertex_person_green.png").pixmap(500, 500);
    vert.posX = 500;
    vert.posY = 500;
    m_currentGraph->addVertex(vert);

    vert.shortName = "Кр. хер";
    vert.name = "Кр. хер узел";
    vert.backgroundColor = Qt::red;
    vert.pxmap = QIcon("://DATA/images/vertexicons/vertex_person_red.png").pixmap(500, 500);
    vert.posX = 100;
    vert.posY = 500;
    m_currentGraph->addVertex(vert);

    Graph::GConnection con;
    con.name = "1-2";
    con.idFrom = 1;
    con.idTo = 2;
    con.lineColor = Qt::red;
    m_currentGraph->addConnection(con);

    con.name = "4-3";
    con.idFrom = 4;
    con.idTo = 3;
    con.lineColor = Qt::green;
    m_currentGraph->addConnection(con);

    con.name = "1-3";
    con.idFrom = 1;
    con.idTo = 3;
    con.lineColor = Qt::magenta;
    m_currentGraph->addConnection(con);

    setupWidget();
    setupModels();
    setupSignals();

    ui->graphScene->setCurrentGraph(m_currentGraph);
}

GraphEditorForm::~GraphEditorForm()
{
    delete ui;
}

void GraphEditorForm::startValidanceTest()
{
    LOG_WARNING("Validance test not written");
}

Graph::GraphExtendedObject* GraphEditorForm::getCurrentGraph()
{
    return m_currentGraph;
}

ButtonMatrix::HeadButton *GraphEditorForm::getOverlayButton()
{
    return m_pButtonMatrixHead;
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
        m_currentGraph->setCustomValue(m_pUserGraphInfoModel->index(row, userPropNameCol).data().toString(),
                                        m_pUserGraphInfoModel->index(row, userPropDataCol).data());
    }

    m_currentGraph->setEditTime(QDateTime::currentDateTime());

    if (QFileInfo(m_currentGraphFilePath).suffix() != "gse") {
        m_currentGraphFilePath += ".gse";
    }

    auto saveSucceed = SaveMaster::save(m_currentGraphFilePath, *m_currentGraph);
    if (!saveSucceed) {
        GraphCommon::showError("Ошибка сохранения графа");
        return;
    }
}

void GraphEditorForm::loadGraph()
{
    auto loadSucceed = SaveMaster::load(m_currentGraphFilePath, *m_currentGraph);
    if (!loadSucceed) {
        GraphCommon::showError("Ошибка загрузки графа");
        return;
    }

    updateGraphInfo();
}

void GraphEditorForm::setupSignals()
{
    connect(ui->propertyAdd_pushButton, &QPushButton::clicked,
            this, [this]() {
        auto pItem = new QStandardItem("Моё свойство");
        pItem->setColumnCount(2);
        pItem->setChild(1, new QStandardItem("Моё значение"));
        m_pUserGraphInfoModel->appendRow(pItem);
    });

    connect(ui->propertyRemove_pushButton, &QPushButton::clicked,
            this, [this]() {
        auto selectedItems = ui->propertyUser_tableView->selectionModel()->selection();

        auto firstRow = selectedItems.front().top();
        auto rowCount = selectedItems.front().height();

        m_pUserGraphInfoModel->removeRows(firstRow, rowCount);
    });

    ui->propertyRemove_pushButton->setDisabled(true);
    connect(ui->propertyUser_tableView->selectionModel(), &QItemSelectionModel::selectionChanged,
            this, [this](const QItemSelection &selected, const QItemSelection &) {
        ui->propertyRemove_pushButton->setDisabled(selected.isEmpty());
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
        m_pUserGraphInfoModel = new QStandardItemModel(this);
        m_pUserGraphInfoModel->setColumnCount(2);
        m_pUserGraphInfoModel->setHeaderData(0, Qt::Horizontal, "Свойство");
        m_pUserGraphInfoModel->setHeaderData(1, Qt::Horizontal, "Значение");

        ui->propertyUser_tableView->setModel(m_pUserGraphInfoModel);
        ui->propertyUser_tableView->verticalHeader()->hide();
        ui->propertyUser_tableView->horizontalHeader()->setStretchLastSection(true);
        ui->propertyUser_tableView->horizontalHeader()->setSizeAdjustPolicy(QHeaderView::AdjustToContents);
    }
}

void GraphEditorForm::setupWidget()
{
    m_pButtonMatrixHead = new ButtonMatrix::HeadButton(ui->graphScene);

    m_pButtonMatrixHead->setToolTip("Инструменты");
    m_pButtonMatrixHead->setButtonPadding(0, 40, 0, 40);
    m_pButtonMatrixHead->setButtonMatrix(-5, 0, 0, 5);

    m_pButtonMatrixHead->setAnimationSpeed(1);
    m_pButtonMatrixHead->setSize(QSize(50, 50));

    m_pButtonMatrixHead->setIcons(QIcon("://DATA/images/icons/toolbox_hide.png"),
                               QIcon("://DATA/images/icons/toolbox_show.png"));

    ButtonMatrix::ButtonConfig buttonInfo;

    buttonInfo.positionX = -1;
    buttonInfo.positionY = 0;
    buttonInfo.icon = QIcon("://DATA/images/icons/edit.png");
    buttonInfo.tooltip = "Показать свойства графа";
    buttonInfo.action = [this](QPushButton* pSender) {
        if (ui->graphProps_groupBox->isHidden()) {
            ui->graphProps_groupBox->show();

            QPropertyAnimation* animation = new QPropertyAnimation(ui->graphProps_groupBox, "maximumWidth");
            animation->setDuration(150);

            animation->setStartValue(0);
            animation->setEndValue(ui->graphProps_groupBox->width());
            animation->start();

            connect(animation, &QPropertyAnimation::finished,
                    this, [this, animation]() {
                delete animation;
            });

            pSender->setToolTip("Скрыть свойства графа");
            return;
        }

        QPropertyAnimation* animation = new QPropertyAnimation(ui->graphProps_groupBox, "minimumWidth");
        animation->setDuration(150);

        animation->setStartValue(ui->graphProps_groupBox->width());
        animation->setEndValue(0);
        animation->start();

        connect(animation, &QPropertyAnimation::finished,
                this, [this, animation]() {
            delete animation;
            ui->graphProps_groupBox->hide();
        });

        pSender->setToolTip("Показать свойства графа");
    };
    ui->graphProps_groupBox->hide();
    m_pButtonMatrixHead->addButton(buttonInfo);


    buttonInfo.positionX = -2;
    buttonInfo.icon = QIcon("://DATA/images/icons/cancel_changes.png");
    buttonInfo.tooltip = "Отменить изменения";
    buttonInfo.action = [this](QPushButton*) {
        if (!isGraphPathSet()) {
            return;
        }
        loadGraph();
    };
    m_pButtonMatrixHead->addButton(buttonInfo);
    m_pButtonMatrixHead->getButton(buttonInfo.positionX, buttonInfo.positionY)->setEnabled(false);


    buttonInfo.positionX = -3;
    buttonInfo.icon = QIcon("://DATA/images/icons/save.png");
    buttonInfo.tooltip = "Сохранить";
    buttonInfo.action = [this, buttonInfo](QPushButton*) {
        if (!isGraphPathSet()) {
            return;
        }
        saveGraph();
        updateGraphInfo();
        m_pButtonMatrixHead->getButton(buttonInfo.positionX, buttonInfo.positionY)->setEnabled(true);
    };
    m_pButtonMatrixHead->addButton(buttonInfo);


    buttonInfo.positionX = -4;
    buttonInfo.icon = QIcon("://DATA/images/icons/open_graph.png");
    buttonInfo.tooltip = "Открыть файл графа";
    buttonInfo.action = [this, buttonInfo](QPushButton*) {
        m_currentGraphFilePath = QFileDialog::getOpenFileName(this, "Файл сохранённого графа", QDir::homePath(), "Файл графа (*.gse)");
        if (m_currentGraphFilePath.isEmpty()) {
            return;
        }
        loadGraph();
        m_pButtonMatrixHead->getButton(buttonInfo.positionX, buttonInfo.positionY)->setEnabled(true);
    };
    m_pButtonMatrixHead->addButton(buttonInfo);


    buttonInfo.positionX = -5;
    buttonInfo.icon = QIcon("://DATA/images/icons/save_as.png");
    buttonInfo.tooltip = "Сохранить как...";
    buttonInfo.action = [this, buttonInfo](QPushButton*) {
        m_currentGraphFilePath = QFileDialog::getSaveFileName(this, "Файл для сохранения графа", QDir::homePath(), "Файл графа (*.gse)");
        if (m_currentGraphFilePath.isEmpty()) {
            return;
        }
        saveGraph();
        updateGraphInfo();
        m_pButtonMatrixHead->getButton(buttonInfo.positionX, buttonInfo.positionY)->setEnabled(true);
    };
    m_pButtonMatrixHead->addButton(buttonInfo);

    buttonInfo.positionX = 0;
    buttonInfo.positionY = 1;
    buttonInfo.icon = QIcon("://DATA/images/icons/mode_none.png");
    buttonInfo.tooltip = "Сменить режим работы";
    buttonInfo.action = [this](QPushButton* pButton) {
    };
    m_pButtonMatrixHead->addButton(buttonInfo);
}

void GraphEditorForm::updateGraphInfo()
{
    // Очистка таким образом, чтобы не сбрасывать VIEW
    m_pCommonGraphInfoModel->removeRows(0, m_pCommonGraphInfoModel->rowCount());
    m_pUserGraphInfoModel->removeRows(0, m_pUserGraphInfoModel->rowCount());

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
        pItem = new QStandardItem(key);
        pProperyItem = new QStandardItem(value.toString());
        m_pUserGraphInfoModel->appendRow({pItem, pProperyItem});
    }

    LOG_INFO("Current graph data update");
}
