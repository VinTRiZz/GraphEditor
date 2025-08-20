#include "graphpropertyeditform.h"

#include <Common/Logging.h>
#include <GraphObject/Object.h>

#include <QPropertyAnimation>

#include "ui_graphpropertyeditform.h"

namespace Graph {

GraphPropertyEditForm::GraphPropertyEditForm(QWidget *parent)
    : QWidget(parent), ui(new Ui::GraphPropertyEditForm) {
  ui->setupUi(this);

  setupWidget();
  setupModels();
  setupSignals();
}

GraphPropertyEditForm::~GraphPropertyEditForm() { delete ui; }

void GraphPropertyEditForm::setCurrentGraph(
    const PMaintainer &pGraphMaintaner) {
  m_currentGraph = pGraphMaintaner;
  m_pCommonPropertiesModel->setGraph(m_currentGraph);
  m_pCustomPropertiesModel->setGraph(m_currentGraph);
}

void GraphPropertyEditForm::setupSignals() {
  connect(ui->propertyAdd_pushButton, &QPushButton::clicked,
          m_pCustomPropertiesModel, &GraphCustomPropertiesModel::addProperty);

  connect(ui->propertyRemove_pushButton, &QPushButton::clicked, this, [this]() {
    auto selectedItems =
        ui->propertyUser_tableView->selectionModel()->selection();

    std::list<QString> propertiesToRemove;
    auto currentRow = selectedItems.front().top();
    auto targetRow = selectedItems.front().height() + currentRow;
    while (currentRow <= targetRow) {
      propertiesToRemove.push_back(
          m_pCustomPropertiesModel->getPropertyName(currentRow));
      ++currentRow;
    }

    for (auto &prop : propertiesToRemove) {
      m_pCustomPropertiesModel->removeProperty(prop);
    }
  });

  ui->propertyRemove_pushButton->setDisabled(true);
  connect(ui->propertyUser_tableView->selectionModel(),
          &QItemSelectionModel::selectionChanged, this,
          [this](const QItemSelection &selected, const QItemSelection &) {
            ui->propertyRemove_pushButton->setDisabled(selected.isEmpty());
          });
}

void GraphPropertyEditForm::setupModels() {
  if (m_pCommonPropertiesModel == nullptr) {
    m_pCommonPropertiesModel = new GraphCommonPropertiesModel;

    ui->propertyCommon_tableView->setModel(m_pCommonPropertiesModel);
    ui->propertyCommon_tableView->verticalHeader()->hide();
    ui->propertyCommon_tableView->horizontalHeader()->setStretchLastSection(
        true);
    ui->propertyCommon_tableView->horizontalHeader()->setSizeAdjustPolicy(
        QHeaderView::AdjustToContents);
  }

  if (m_pCustomPropertiesModel == nullptr) {
    m_pCustomPropertiesModel = new GraphCustomPropertiesModel(this);

    ui->propertyUser_tableView->setModel(m_pCustomPropertiesModel);
    ui->propertyUser_tableView->verticalHeader()->hide();
    ui->propertyUser_tableView->horizontalHeader()->setStretchLastSection(true);
    ui->propertyUser_tableView->horizontalHeader()->setSizeAdjustPolicy(
        QHeaderView::AdjustToContents);
  }
}

void GraphPropertyEditForm::setupWidget() {}

} // namespace Graph
