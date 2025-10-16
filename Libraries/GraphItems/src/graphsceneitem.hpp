#pragma once

#include <Components/CustomQt/ObjectScene/Constants.h>
#include <Components/CustomQt/ObjectScene/ItemBase.h>

#include <QJsonObject>

#include <QGraphicsItem>

namespace Graph {

const int OBJECTTYPE_VERTEX {ObjectViewItems::OBJECTFIELD_USERTYPE + 10};
const int OBJECTTYPE_CONNECTION {OBJECTTYPE_VERTEX + 1};

namespace Layers {
const int VERTEX_LAYER {100};
const int CONNECTION_LAYER {50};
}

namespace Sizes {
const int VERTEX_RADIUS {50};
}

/**
 * @brief The GraphSceneItem class Интерфейс элемента графа
 */
class GraphSceneItem :  public ObjectViewItems::ItemBase
{
public:
    explicit GraphSceneItem(QGraphicsItem* parent = nullptr);
};


}
