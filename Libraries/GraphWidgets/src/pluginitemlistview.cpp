#include "pluginitemlistview.hpp"

#include <QDrag>
#include <QMimeData>

#include "graphwidgetconstants.hpp"

#include <Components/Logger/Logger.h>

PluginItemListView::PluginItemListView(QWidget* parent) :
    QListView(parent)
{
    setDragEnabled(true);
    setDragDropMode(DragDropMode::DragOnly);
}

void PluginItemListView::setPluginName(const QString &pluginName)
{
    m_pluginName = pluginName;
}

void PluginItemListView::startDrag(Qt::DropActions supportedActions) {
    QModelIndexList indexes = selectedIndexes();
    if (indexes.isEmpty()) return;

    auto moveData = new QMimeData;
    QString pluginItemInfo;
    pluginItemInfo += m_pluginName + "\n";
    pluginItemInfo += indexes.front().data(Qt::DisplayRole).toString();
    moveData->setData(Graph::GraphWidgetConstants::MIMETYPE_PLUGINOBJECT, pluginItemInfo.toUtf8());

    QDrag* drag = new QDrag(this);
    drag->setMimeData(moveData);
    drag->exec(supportedActions, Qt::MoveAction);
}
