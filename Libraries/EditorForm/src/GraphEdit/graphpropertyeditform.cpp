#include "graphpropertyeditform.h"
#include "ui_graphpropertyeditform.h"

#include <Common/Logging.h>

#include <GraphObject/Object.h>

#include <QPropertyAnimation>

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
    pProperyItem->setEditable(false);
    m_pCommonGraphInfoModel->appendRow({pItem, pProperyItem});

    pItem = new QStandardItem("Изменён");
    pProperyItem = new QStandardItem(m_currentGraph->getEditTime().toString(GraphCommon::DATE_DISPLAY_CONVERSION_FORMAT));
    pItem->setEditable(false);
    pProperyItem->setEditable(false);
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

void GraphPropertyEditForm::showAnimated()
{
    show();

    QPropertyAnimation* animation = new QPropertyAnimation(this, "minimumWidth");
    animation->setDuration(150);

    setMaximumWidth(m_showWidth);
    setMinimumWidth(0);
    setFixedWidth(0);
    animation->setStartValue(0);
    animation->setEndValue(m_showWidth);
    animation->start(QPropertyAnimation::DeleteWhenStopped);
}

void GraphPropertyEditForm::hideAnimated()
{
    QPropertyAnimation* animation = new QPropertyAnimation(this, "maximumWidth");
    animation->setDuration(150);

    setMaximumWidth(m_showWidth);
    setMinimumWidth(0);
    setFixedWidth(0);
    animation->setStartValue(m_showWidth);
    animation->setEndValue(0);
    animation->start(QPropertyAnimation::DeleteWhenStopped);

    connect(animation, &QPropertyAnimation::finished,
            this, &GraphPropertyEditForm::hide);
}

void GraphPropertyEditForm::updateEditTime()
{
    auto pItem = m_pCommonGraphInfoModel->item(EDITEDROW, 1);
    if (pItem != nullptr) {
        m_currentGraph->setEditTime(QDateTime::currentDateTime());
        pItem->setData(m_currentGraph->getEditTime().toString(GraphCommon::DATE_DISPLAY_CONVERSION_FORMAT), Qt::DisplayRole);
    }
}

void GraphPropertyEditForm::setupSignals()
{
    connect(ui->propertyAdd_pushButton, &QPushButton::clicked,
            this, [this]() {
        auto pItem = new QStandardItem("Моё свойство");
        pItem->setColumnCount(2);
        pItem->setChild(1, new QStandardItem("Моё значение"));
        m_pUserGraphInfoModel->appendRow(pItem);
        updateEditTime();
    });

    connect(ui->propertyRemove_pushButton, &QPushButton::clicked,
            this, [this]() {
        auto selectedItems = ui->propertyUser_tableView->selectionModel()->selection();

        auto firstRow = selectedItems.front().top();
        auto rowCount = selectedItems.front().height();

        m_pUserGraphInfoModel->removeRows(firstRow, rowCount);
        updateEditTime();
    });

    ui->propertyRemove_pushButton->setDisabled(true);
    connect(ui->propertyUser_tableView->selectionModel(), &QItemSelectionModel::selectionChanged,
            this, [this](const QItemSelection &selected, const QItemSelection &) {
        ui->propertyRemove_pushButton->setDisabled(selected.isEmpty());
    });

    connect(m_pCommonGraphInfoModel, &QAbstractItemModel::dataChanged,
            this, [this](const QModelIndex &topLeft, const QModelIndex &) {
        auto changedString = topLeft.data(Qt::DisplayRole).toString();

        switch (topLeft.row())
        {
        case NAMEROW:
            m_currentGraph->setName(changedString);
            break;

        case DESCRIPTIONROW:
            m_currentGraph->setDescription(changedString);
            break;
        }
        updateEditTime();
    });

    connect(m_pUserGraphInfoModel, &QAbstractItemModel::dataChanged,
            this, [this](const QModelIndex &, const QModelIndex &) {
        const int userPropNameCol = 0;
        const int userPropDataCol = 1;
        for (int row = 0; row < m_pUserGraphInfoModel->rowCount(); ++row) {
            m_currentGraph->setCustomValue(m_pUserGraphInfoModel->index(row, userPropNameCol).data().toString(),
                                            m_pUserGraphInfoModel->index(row, userPropDataCol).data());
        }
        updateEditTime();
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
