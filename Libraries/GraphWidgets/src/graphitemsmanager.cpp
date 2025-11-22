#include "graphitemsmanager.hpp"
#include "ui_graphitemsmanager.h"

#include <PluginModule/PluginMaster.h>

GraphItemsManager::GraphItemsManager(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::GraphItemsManager)
{
    ui->setupUi(this);

    m_pluginItemsModel = new QStandardItemModel(this);
    ui->pluginItemList_listView->setModel(m_pluginItemsModel);

    m_pluginItemsModel->setColumnCount(1);
    m_pluginItemsModel->setHeaderData(0, Qt::Horizontal, "Элемент плагина", Qt::DisplayRole);

    initSignals();
    updatePluginList();
}

GraphItemsManager::~GraphItemsManager()
{
    delete ui;
}

void GraphItemsManager::updatePluginList()
{
    m_pluginItemsModel->clear();
    for (auto& pPlugin : Graph::PluginMaster::getInstance().getAllPlugins()) {
        addPlugin(pPlugin->getPluginName().c_str());
    }
}

void GraphItemsManager::initSignals()
{
  connect(ui->pluginSelector_comboBox, &QComboBox::currentTextChanged, this,
          &GraphItemsManager::loadPluginItems);
}

void GraphItemsManager::addPlugin(const QString &pluginName)
{
//    QIcon typeIcon;
//    switch (ptype) {
//    case PluginType::Unknown:
//        typeIcon = QIcon(":/images/icons/plugins/unknown.svg");
//        break;

//    case PluginType::Vertex:
//        typeIcon = QIcon(":/images/icons/plugins/vertex.svg");
//        break;

//    case PluginType::Connection:
//        typeIcon = QIcon(":/images/icons/plugins/connection.svg");
//        break;

//    default:
//        throw std::invalid_argument("Invalid plugin type");
//    }

//    ui->pluginSelector_comboBox->addItem(typeIcon, pluginName);
    ui->pluginSelector_comboBox->addItem(pluginName);
}

void GraphItemsManager::loadPluginItems(const QString &pluginName)
{
    auto& pluginMaster = Graph::PluginMaster::getInstance();
    auto plugin = pluginMaster.getPlugin(pluginName.toStdString());

    m_pluginItemsModel->clear();
    for (auto& pluginItemName : plugin->getObjectNameList()) {
        m_pluginItemsModel->appendRow(new QStandardItem(QString::fromStdString(pluginItemName)));
    }
}
