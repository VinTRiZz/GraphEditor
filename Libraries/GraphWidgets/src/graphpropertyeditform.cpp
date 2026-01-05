#include "graphpropertyeditform.h"

#include "models/graphcommonpropertiesmodel.h"

#include <Components/Logger/Logger.h>
#include <GraphObject/GraphObject.h>

#include <QPropertyAnimation>

#include "ui_graphpropertyeditform.h"

namespace Graph {

GraphPropertyEditForm::GraphPropertyEditForm(QWidget* parent)
    : QWidget(parent), ui(new Ui::GraphPropertyEditForm) {
    ui->setupUi(this);

    setupWidget();
    setupModels();
    setupSignals();
}

GraphPropertyEditForm::~GraphPropertyEditForm() {
    delete ui;
}

void GraphPropertyEditForm::processGraphChange(const GraphObjectManagerPtr &pPrevGraph)
{
    auto GraphObjectManager = getGraph();
    m_pCommonPropertiesModel->setGraph(GraphObjectManager);
}

void GraphPropertyEditForm::setupSignals() {

}

void GraphPropertyEditForm::setupModels() {
    if (m_pCommonPropertiesModel == nullptr) {
        m_pCommonPropertiesModel = new GraphPropertiesModel;

        ui->propertyCommon_tableView->setModel(m_pCommonPropertiesModel);
        ui->propertyCommon_tableView->verticalHeader()->hide();
        ui->propertyCommon_tableView->horizontalHeader()->setStretchLastSection(
            true);
        ui->propertyCommon_tableView->horizontalHeader()->setSizeAdjustPolicy(
            QHeaderView::AdjustToContents);
    }
}

void GraphPropertyEditForm::setupWidget() {}

}  // namespace Graph
