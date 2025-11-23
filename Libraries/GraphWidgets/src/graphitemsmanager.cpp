#include "graphitemsmanager.hpp"
#include "ui_graphitemsmanager.h"

#include <PluginModule/PluginMaster.h>
#include <PluginCoreInterface/Core.h>

#include <QSortFilterProxyModel>

GraphItemsManager::GraphItemsManager(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::GraphItemsManager)
{
    ui->setupUi(this);

    m_pluginItemsModel = new QStandardItemModel(this);

    auto pFiltrator = new QSortFilterProxyModel;
    pFiltrator->setSourceModel(m_pluginItemsModel);
    pFiltrator->sort(0);
    pFiltrator->setFilterKeyColumn(0);
    pFiltrator->setFilterCaseSensitivity(Qt::CaseInsensitive);
    ui->pluginItemList_listView->setModel(pFiltrator);

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
        addPlugin(pPlugin->getPluginName());
    }
}

void GraphItemsManager::initSignals()
{
  connect(ui->pluginSelector_comboBox, &QComboBox::currentTextChanged, this,
          &GraphItemsManager::loadPluginItems);

  connect(ui->pluginItemList_listView, &QListView::doubleClicked,
          this, [this](auto& clickIdx){
      auto& pluginMaster = Graph::PluginMaster::getInstance();
      auto pPluginCore = pluginMaster.getPlugin(ui->pluginSelector_comboBox->currentText())->getPluginCore();
      auto targetItem = pPluginCore->createObject(clickIdx.data(Qt::DisplayRole).toString());
      if (targetItem != nullptr) {
          emit addObject(targetItem);
      }
  });

  connect(ui->itemSearch_lineEdit, &QLineEdit::textChanged,
          this, [this](auto& txt){
      static_cast<QSortFilterProxyModel*>(ui->pluginItemList_listView->model())->setFilterWildcard("*" + txt + "*");
  });
}

void GraphItemsManager::addPlugin(const QString &pluginName)
{
    ui->pluginSelector_comboBox->addItem(pluginName);
}

void GraphItemsManager::loadPluginItems(const QString &pluginName)
{
    auto& pluginMaster = Graph::PluginMaster::getInstance();
    auto pluginCore = pluginMaster.getPlugin(pluginName)->getPluginCore();

    m_pluginItemsModel->clear();
    QIcon typeIcon;
    for (auto& pluginItemName : pluginCore->getObjectNameList()) {
        auto pRow = new QStandardItem(pluginItemName);

        auto itemType = pluginCore->getObjectType(pluginItemName);
        switch (itemType)
        {
        case Graph::AbstractPluginCore::PluginObjectType::Vertex:
            typeIcon = QIcon(":/images/icons/plugins/vertex.svg");
            break;

        case Graph::AbstractPluginCore::PluginObjectType::Connection:
            typeIcon = QIcon(":/images/icons/plugins/connection.svg");
            break;

        case Graph::AbstractPluginCore::PluginObjectType::Unknown:
            typeIcon = QIcon(":/images/icons/plugins/unknown.svg");
            break;

        default:
            throw std::invalid_argument("GraphItemsManager: Unknown type of plugin item (failed to load icon)");
        }
        pRow->setIcon(typeIcon);
        m_pluginItemsModel->appendRow(pRow);
    }
}
