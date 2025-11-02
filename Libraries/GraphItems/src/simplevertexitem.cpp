#include "simplevertexitem.hpp"

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

#include "vertexshapeprovider.hpp"

using namespace ObjectItems;

namespace Graph {

namespace {
const QString EXTRADATA_VALUE_SHAPETYPE {"shapeType"};
}

SimpleVertexItem::SimpleVertexItem(QGraphicsItem* parent) :
    VertexItem(parent) {
    setSystemName("Вершина");

    setFlag(QGraphicsItem::ItemIsSelectable, true);
    setFlag(QGraphicsItem::ItemClipsToShape, true);
    setFlag(QGraphicsItem::ItemSendsScenePositionChanges, true);

    getLabel()->setContrastRectVisible(false);

    m_vertexShapeItem = VertexShapeProvider::createShape(m_shapeType, getVertexSizeType());
    initShapeItem();

    connect(this, &ObjectItems::BasicItem::graphicalDataChanged,
            this, [this](){
        m_vertexShapeItem->setPen(getLinePen());
        m_vertexShapeItem->setBrush(getBackgroundBrush());
    });
}

SimpleVertexItem::~SimpleVertexItem() {

}

void SimpleVertexItem::fromVertex(const GVertex &vert)
{
    VertexItem::fromVertex(vert);

    if (!vert.vertexExtraData.contains(EXTRADATA_VALUE_SHAPETYPE)) {
        LOG_WARNING("SimpleVertexItem: Failed to find required extra data");
        return;
    }
    setShapeType(SimpleVertexShapeType(vert.vertexExtraData[EXTRADATA_VALUE_SHAPETYPE].toInt()));
}

GVertex SimpleVertexItem::toVertex() const
{
    auto graphVertex = VertexItem::toVertex();
    graphVertex.vertexExtraData[EXTRADATA_VALUE_SHAPETYPE] = m_shapeType;
    return graphVertex;
}

void SimpleVertexItem::setShapeType(SimpleVertexShapeType vst)
{
    if (m_shapeType == vst) {
        return;
    }

    delete m_vertexShapeItem;
    m_vertexShapeItem = VertexShapeProvider::createShape(vst, getVertexSizeType());
    initShapeItem();
    m_shapeType = vst;
}

SimpleVertexShapeType SimpleVertexItem::getShapeType() const
{
    return m_shapeType;
}

void SimpleVertexItem::initShapeItem()
{
    registerSubitem(m_vertexShapeItem);

    m_vertexShapeItem->setPen(getLinePen());
    m_vertexShapeItem->setBrush(getBackgroundBrush());

    m_vertexShapeItem->setZValue(1);
    m_vertexShapeItem->show();
}

void SimpleVertexItem::processSizeTypeChange([[maybe_unused]] const QRectF &newSize)
{
    VertexShapeProvider::scaleShape(m_vertexShapeItem, getVertexSizeType());
}

}  // namespace ObjectItems
