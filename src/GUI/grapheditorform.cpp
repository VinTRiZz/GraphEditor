#include "grapheditorform.h"
#include "ui_grapheditorform.h"

#include <QFileDialog>
#include <QFileInfo>

#include "Graph/graphobject.h"
#include "Graph/graphcommon.h"
#include "Internal/savemaster.h"

#include <QParallelAnimationGroup>
#include <QPropertyAnimation>

#include "logging.h"

GraphEditorForm::GraphEditorForm(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::GraphEditorForm)
{
    ui->setupUi(this);

    ui->graphScene->init();
    ui->graphScene->resizeScene(QSize(1000, 1000));
    ui->graphScene->scale(0.5, 0.5);

    m_currentGraph.setName("Test example graph");
    m_currentGraph.setDescription("Example description");
    m_currentGraph.setCreateTime(QDateTime::currentDateTime());
    m_currentGraph.setEditTime(QDateTime::currentDateTime());

    m_currentGraph.setIdGenerator(ui->graphScene->getIdGenerator());

    Graph::GVertex vert;
    vert.shortName = "Дебич";
    vert.name = "Дебич узел";
    vert.backgroundColor = Qt::green;
    vert.pxmap = QIcon("://DATA/images/vertexicons/vertex_person_red.png").pixmap(500, 500);
    vert.posX = 100;
    vert.posY = 100;
    m_currentGraph.addVertex(vert);

    vert.shortName = "Кр. дебич";
    vert.name = "Кр. дебич узел";
    vert.backgroundColor = Qt::red;
    vert.pxmap = QIcon("://DATA/images/vertexicons/vertex_person_red.png").pixmap(500, 500);
    vert.posX = 300;
    vert.posY = 300;
    m_currentGraph.addVertex(vert);

    vert.shortName = "Др. дебич";
    vert.name = "Др. дебич узел";
    vert.backgroundColor = Qt::green;
    vert.pxmap = QIcon("://DATA/images/vertexicons/vertex_person_green.png").pixmap(500, 500);
    vert.posX = 500;
    vert.posY = 500;
    m_currentGraph.addVertex(vert);

    vert.shortName = "Кр. хер";
    vert.name = "Кр. хер узел";
    vert.backgroundColor = Qt::red;
    vert.pxmap = QIcon("://DATA/images/vertexicons/vertex_person_red.png").pixmap(500, 500);
    vert.posX = 100;
    vert.posY = 500;
    m_currentGraph.addVertex(vert);

    Graph::GConnection con;
    con.name = "1-2";
    con.idFrom = 1;
    con.idTo = 2;
    con.lineColor = Qt::red;
    m_currentGraph.addConnection(con);

    con.name = "4-3";
    con.idFrom = 4;
    con.idTo = 3;
    con.lineColor = Qt::green;
    m_currentGraph.addConnection(con);

    con.name = "1-3";
    con.idFrom = 1;
    con.idTo = 3;
    con.lineColor = Qt::magenta;
    m_currentGraph.addConnection(con);

    setupWidget();
    setupModels();
    setupSignals();

    m_graphDrawer.updateGraph();
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

OverlayButtonList *GraphEditorForm::getOverlayButton()
{
    return m_pOverlayButton;
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
}

void GraphEditorForm::loadGraph()
{
    auto loadSucceed = SaveMaster::load(m_currentGraphFilePath, m_currentGraph);
    if (!loadSucceed) {
        GraphCommon::showError("Ошибка загрузки графа");
        return;
    }
    m_currentGraph.setIdGenerator(ui->graphScene->getIdGenerator());

    updateGraphInfo();
    m_graphDrawer.updateGraph();
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
    m_pOverlayButton = new OverlayButtonList(ui->graphScene);

    m_pOverlayButton->setToolTip("Инструменты");
    m_pOverlayButton->setWidgetPadding(OverlayButtonList::Right,    40);
    m_pOverlayButton->setWidgetPadding(OverlayButtonList::Down,     40);
    m_pOverlayButton->setWidgetPadding(OverlayButtonList::Up,       -1);
    m_pOverlayButton->setWidgetPadding(OverlayButtonList::Left,     -1);

    m_pOverlayButton->setOpenDirection(OverlayButtonList::ButtonOpenDirection( OverlayButtonList::ButtonOpenDirection::Up | OverlayButtonList::ButtonOpenDirection::Left));
    m_pOverlayButton->setMaxButtonCount(OverlayButtonList::ButtonOpenDirection::Left, 5);
    m_pOverlayButton->setMaxButtonCount(OverlayButtonList::ButtonOpenDirection::Up, 5);

    m_pOverlayButton->setAnimationSpeed(1.5);
    m_pOverlayButton->setButtonSize(QSize(50, 50));
    m_pOverlayButton->setHideOnClick(false);

    m_pOverlayButton->setOpenedIcon(QIcon("://DATA/images/icons/toolbox_hide.png"));
    m_pOverlayButton->setClosedIcon(QIcon("://DATA/images/icons/toolbox_show.png"));


    OverlayButtonList::ButtonInfo buttonInfo;
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
                m_pOverlayButton->fixPosition();
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
    m_pOverlayButton->addButton(buttonInfo);

    buttonInfo.icon = QIcon("://DATA/images/icons/cancel_changes.png");
    buttonInfo.tooltip = "Отменить изменения";
    buttonInfo.action = [this](QPushButton*) {
        if (!isGraphPathSet()) {
            return;
        }
        loadGraph();
    };
    auto cancelButtonIndex = m_pOverlayButton->addButton(buttonInfo);
    m_pOverlayButton->getButton(cancelButtonIndex)->setEnabled(false);


    buttonInfo.icon = QIcon("://DATA/images/icons/save.png");
    buttonInfo.tooltip = "Сохранить";
    buttonInfo.action = [this, cancelButtonIndex](QPushButton*) {
        if (!isGraphPathSet()) {
            return;
        }
        saveGraph();
        updateGraphInfo();
        m_pOverlayButton->getButton(cancelButtonIndex)->setEnabled(true);
    };
    m_pOverlayButton->addButton(buttonInfo);


    buttonInfo.icon = QIcon("://DATA/images/icons/open_graph.png");
    buttonInfo.tooltip = "Открыть файл графа";
    buttonInfo.action = [this, cancelButtonIndex](QPushButton*) {
        m_currentGraphFilePath = QFileDialog::getOpenFileName(this, "Файл сохранённого графа", QDir::homePath(), "Файл графа (*.gse)");
        if (m_currentGraphFilePath.isEmpty()) {
            return;
        }
        loadGraph();
        m_pOverlayButton->getButton(cancelButtonIndex)->setEnabled(true);
    };
    m_pOverlayButton->addButton(buttonInfo);


    buttonInfo.icon = QIcon("://DATA/images/icons/save_as.png");
    buttonInfo.tooltip = "Сохранить как...";
    buttonInfo.action = [this, cancelButtonIndex](QPushButton*) {
        m_currentGraphFilePath = QFileDialog::getSaveFileName(this, "Файл для сохранения графа", QDir::homePath(), "Файл графа (*.gse)");
        if (m_currentGraphFilePath.isEmpty()) {
            return;
        }
        saveGraph();
        updateGraphInfo();
        m_pOverlayButton->getButton(cancelButtonIndex)->setEnabled(true);
    };
    m_pOverlayButton->addButton(buttonInfo);

    buttonInfo.icon = QIcon("://DATA/images/icons/mode_none.png");
    buttonInfo.tooltip = "Сменить режим работы";
    buttonInfo.action = [this](QPushButton* pButton) {
        auto currentDrawerMode = m_graphDrawer.getCurrentMode();

        if (currentDrawerMode == GraphDrawer::CurrentDrawerMode::Edit) {
            pButton->setIcon(QIcon("://DATA/images/icons/mode_view.png"));
            m_graphDrawer.startViewMode();
            return;
        }

        if (currentDrawerMode == GraphDrawer::CurrentDrawerMode::View) {
            pButton->setIcon(QIcon("://DATA/images/icons/mode_none.png"));
            m_graphDrawer.stopMode();
            return;
        }

        pButton->setIcon(QIcon("://DATA/images/icons/mode_edit.png"));
        m_graphDrawer.startEditMode();
    };
    m_pOverlayButton->addButton(buttonInfo);

    // Настройка мастера редактирования графа
    m_graphDrawer.setOverlayButtonList(m_pOverlayButton);
    m_graphDrawer.setCurrentGraph(&m_currentGraph);
    m_graphDrawer.setScene(ui->graphScene);
    m_graphDrawer.init();
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
