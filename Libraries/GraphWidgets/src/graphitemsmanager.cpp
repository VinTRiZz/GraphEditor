#include "graphitemsmanager.hpp"
#include "ui_graphitemsmanager.h"

GraphItemsManager::GraphItemsManager(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::GraphItemsManager)
{
    ui->setupUi(this);

    ui->main_toolBox->setCurrentIndex(0);
}

GraphItemsManager::~GraphItemsManager()
{
    delete ui;
}
