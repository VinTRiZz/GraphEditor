#include "graphsceneitem.hpp"

#include <QGraphicsItem>

using namespace ObjectViewItems;

namespace Graph {

GraphSceneItem::GraphSceneItem(QGraphicsItem *parent) :
    ItemBase(parent)
{
    setSystemName("Объект сцены");
}

}
