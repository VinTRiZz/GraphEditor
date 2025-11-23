#include "graphitemsmanager.hpp"
#include "ui_graphitemsmanager.h"

#include <PluginModule/PluginMaster.h>

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
        addPlugin(pPlugin->getPluginName().c_str());
    }
}

void GraphItemsManager::initSignals()
{
  connect(ui->pluginSelector_comboBox, &QComboBox::currentTextChanged, this,
          &GraphItemsManager::loadPluginItems);

  connect(ui->pluginItemList_listView, &QListView::doubleClicked,
          this, [this](auto& clickIdx){
      auto& pluginMaster = Graph::PluginMaster::getInstance();
      auto targetItem = pluginMaster.getPlugin(ui->pluginSelector_comboBox->currentText().toStdString())->createObject(clickIdx.data(Qt::DisplayRole).toString().toStdString());
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
    auto plugin = pluginMaster.getPlugin(pluginName.toStdString());

    m_pluginItemsModel->clear();
    QIcon typeIcon;
    for (auto& pluginItemName : plugin->getObjectNameList()) {
        auto pRow = new QStandardItem(QString::fromStdString(pluginItemName));

        auto itemType = plugin->getObjectType(pluginItemName);
        if (itemType == "vertex") {
            typeIcon = QIcon(":/images/icons/plugins/vertex.svg");
        } else if (itemType == "connection") {
            typeIcon = QIcon(":/images/icons/plugins/connection.svg");
        } else {
            typeIcon = QIcon(":/images/icons/plugins/unknown.svg");
        }
        pRow->setIcon(typeIcon);
        m_pluginItemsModel->appendRow(pRow);
    }
}
