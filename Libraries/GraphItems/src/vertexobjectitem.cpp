#include "vertexobjectitem.hpp"

#include <Components/Common/ApplicationSettings.h>
#include <Components/Logger/Logger.h>
#include <GraphObject/Object.h>
#include <Components/CustomQt/ObjectView/ObjectItems.h>

#include <QBuffer>
#include <QFileInfo>
#include <QGraphicsSceneMouseEvent>
#include <QLabel>
#include <QTextOption>

#include "connectionlineitem.hpp"
#include "constants.hpp"

using namespace ObjectItems;

namespace Graph {

VertexObjectItem::VertexObjectItem(QGraphicsItem* parent) : VertexItem(parent) {
    setSystemName("Вершина");

    setFlag(QGraphicsItem::ItemIsSelectable, true);
    setFlag(QGraphicsItem::ItemClipsToShape, true);
    setFlag(QGraphicsItem::ItemSendsScenePositionChanges, true);

    createSubitem(m_vertexImage);
}

VertexObjectItem::~VertexObjectItem() {
    for (auto pLine : m_connectionsToThis) {
        pLine->setVertexTo(nullptr);
    }

    for (auto pLine : m_connectionsFromThis) {
        pLine->setVertexFrom(nullptr);
    }
}

void VertexObjectItem::fromVertex(const GVertex &vert)
{
    setItemId(vert.id);
    setPos(vert.posX, vert.posY);

    setDisplayName(vert.displayName);
    setToolTip(vert.name);
    setDescription(vert.description);

    setLineColor(vert.lineColor);
    setBackgroundColor(vert.backgroundColor);

    if (!vert.image.isNull()) {
        auto pxmap = QPixmap::fromImage(vert.image);
        m_vertexImage->setPixmap(pxmap.scaled(QSize(Sizes::VERTEX_RADIUS * 2, Sizes::VERTEX_RADIUS * 2)));
    }
}

GVertex VertexObjectItem::toVertex() const
{
    GVertex graphVertex;
    graphVertex.id = getItemId();
    graphVertex.posX = x();
    graphVertex.posY = y();

    graphVertex.displayName = getDisplayName();
    graphVertex.name = toolTip();
    graphVertex.description = getDescription();

    graphVertex.lineColor = getLineColor();
    graphVertex.backgroundColor = getBackgroundColor();

    graphVertex.image = m_vertexImage->pixmap().toImage();

    return graphVertex;
}

}  // namespace ObjectItems
