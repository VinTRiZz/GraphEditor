#include "graphpropertyeditform.h"
#include "ui_graphpropertyeditform.h"

#include "logging.h"

#include "Graph/graphcommon.h"

namespace Graph
{

GraphPropertyEditForm::GraphPropertyEditForm(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::GraphPropertyEditForm)
{
    ui->setupUi(this);

    setupWidget();
    setupModels();
    setupSignals();
}

GraphPropertyEditForm::~GraphPropertyEditForm()
{
    delete ui;
}

void GraphPropertyEditForm::updateGraphInfo()
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

void GraphPropertyEditForm::setCurrentGraph(GraphExtendedObject *pGraph)
{
    m_currentGraph = pGraph;
    updateGraphInfo();
}

void GraphPropertyEditForm::setupSignals()
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

void GraphPropertyEditForm::setupModels()
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

void GraphPropertyEditForm::setupWidget()
{

}

}
