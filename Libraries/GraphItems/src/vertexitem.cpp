#include "vertexitem.hpp"

#include "constants.hpp"
#include "vertexconnectionitem.hpp"

using namespace ObjectItems;

namespace Graph {

namespace {
const QString EXTRADATA_VALUE_TITLEPOS {"titlePosition"};
}


bool VertexItem::isLineSubscribed(VertexConnectionItem* pLine) {
    // Нет смысла проверять исходящие, т.к. нельзя регистрировать вершину саму
    // на себя
    for (auto pLineTo : m_connectionsToThis) {
        if (pLineTo->getVertexFrom()->getItemId() ==
            pLine->getVertexFrom()->getItemId()) {
            return true;
        }
    }
    return false;
}

VertexItem::VertexItem(QGraphicsItem *parent) :
    ObjectItems::BasicItem(parent)
{
    setObjectType(OBJECTTYPE_VERTEX);
    setSystemName("Vertex");
    createSubitem(m_nameItem);
    m_nameItem->setLinePen({Qt::black, 1, Qt::SolidLine, Qt::RoundCap});
    m_nameItem->setZValue(100);
    m_nameItem->setObjectType(OBJECTTYPE_VERTEX);

    setLinePen(Colors::DEFAULT_COLOR_VERTEX_LINE);
    setBackgroundBrush(Colors::DEFAULT_COLOR_VERTEX_BGR);

    setZValue(Layers::VERTEX_LAYER);

    connect(this, &BasicItem::displayNameChanged,
            m_nameItem, [this](){
        m_nameItem->setDisplayName(getDisplayName());
        updateLabelPosition();
    });

    connect(this, &BasicItem::itemMoved,
            this, &VertexItem::updateConnectionLines);
}

VertexItem::~VertexItem()
{
    for (auto pLine : m_connectionsToThis) {
        pLine->setVertexTo(nullptr);
    }

    for (auto pLine : m_connectionsFromThis) {
        pLine->setVertexFrom(nullptr);
    }
}

void VertexItem::fromVertex(const GVertex &vert)
{
    setItemId(vert.id);
    setPos(vert.posX, vert.posY);

    setDisplayName(vert.displayName);
    setToolTip(vert.name);
    setDescription(vert.description);

    setLinePen(vert.lineColor);
    setBackgroundBrush(vert.backgroundColor);

    if (!vert.vertexExtraData.contains(EXTRADATA_VALUE_TITLEPOS)) {
        LOG_WARNING("VertexItem: Failed to find required extra data");
        return;
    }
    setTitlePosition(VertexTitlePosition(vert.vertexExtraData[EXTRADATA_VALUE_TITLEPOS].toInt()));
}

GVertex VertexItem::toVertex() const
{
    GVertex graphVertex;
    graphVertex.id = getItemId();
    graphVertex.posX = x();
    graphVertex.posY = y();

    graphVertex.displayName = getDisplayName();
    graphVertex.name = toolTip();
    graphVertex.description = getDescription();

    graphVertex.lineColor = getLinePen().color();
    graphVertex.backgroundColor = getBackgroundBrush().color();

    graphVertex.vertexExtraData[EXTRADATA_VALUE_TITLEPOS] = m_shapeTitlePos;

    return graphVertex;
}

void VertexItem::setVertexSizeType(VertexSizeType vst)
{
    auto prevSizeT = m_vertexSizeType;
    m_vertexSizeType = vst;

    double currentFontSizePt {0};
    switch (vst)
    {
    case VertexSizeType::VST_UltraSmall:
        currentFontSizePt = 6;
        break;

    case VertexSizeType::VST_Small:
        currentFontSizePt = 6;
        break;

    case VertexSizeType::VST_Medium:
        currentFontSizePt = 14;
        break;

    case VertexSizeType::VST_Big:
        currentFontSizePt = 16;
        break;

    case VertexSizeType::VST_Huge:
        currentFontSizePt = 20;
        break;

    default:
        throw std::invalid_argument(std::string("Invalid vertex size type: ") + std::to_string(vst));
    }
    getLabel()->setTextSizePt(currentFontSizePt);
    updateLabelPosition();

    processSizeTypeChange(toVertexBoundingRect(vst));
    emit sizeChanged(prevSizeT, m_vertexSizeType);
}

VertexSizeType VertexItem::getVertexSizeType() const
{
    return m_vertexSizeType;
}

void VertexItem::subscribeAsConnectionFrom(VertexConnectionItem* pLine) {
    if (this == pLine->getVertexTo()) {
        return;
    }

    if (nullptr != pLine->getVertexFrom()) {
        pLine->getVertexFrom()->unsubscribeConnectionFrom(pLine);
    }

    pLine->setVertexFrom(this);
    m_connectionsFromThis.emplace(pLine);
    updateConnectionLines();
}

void VertexItem::unsubscribeConnectionFrom(VertexConnectionItem* pLine) {
    pLine->setVertexFrom(nullptr);
    m_connectionsFromThis.erase(pLine);
}

void VertexItem::subscribeAsConnectionTo(VertexConnectionItem* pLine) {
    if (this == pLine->getVertexFrom()) {
        return;
    }

    if (nullptr != pLine->getVertexTo()) {
        pLine->getVertexTo()->unsubscribeConnectionTo(pLine);
    }

    pLine->setVertexTo(this);
    m_connectionsToThis.emplace(pLine);
    updateConnectionLines();
}

void VertexItem::unsubscribeConnectionTo(VertexConnectionItem* pLine) {
    pLine->setVertexTo(nullptr);
    m_connectionsToThis.erase(pLine);
}

void VertexItem::updateConnectionLines() {
    unsigned connectionNumber{0};
    auto vertexRadius = static_cast<double>(boundingRect().width()) / 2.0;

    for (auto pConFrom : m_connectionsFromThis) {
        auto fromPos =
            QPointF(x() + vertexRadius,
                    y() + 2 * vertexRadius + pConFrom->getLineItem()->getArrowHeight().height() +
                        m_nameItem->boundingRect().height() * 0.7);

        pConFrom->getLineItem()->setPositionFrom(fromPos);
        connectionNumber++;
    }

    connectionNumber = 0;
    for (auto pConTo : m_connectionsToThis) {
        auto toPos = QPointF(x() + vertexRadius, y() - pConTo->getLineItem()->getArrowHeight().height());
        pConTo->getLineItem()->setPositionTo(toPos);
        connectionNumber++;
    }
}

void VertexItem::setTitlePosition(VertexTitlePosition vtp)
{
    m_shapeTitlePos = vtp;
    updateLabelPosition();
}

VertexTitlePosition VertexItem::getTitlePosition() const
{
    return m_shapeTitlePos;
}

void VertexItem::updateLabelPosition()
{
    auto shapeBRect = toVertexBoundingRect(getVertexSizeType());
    QPointF targetPos;

    auto labelBRect = getLabel()->shape().boundingRect();

    switch (m_shapeTitlePos)
    {
    case VertexTitlePosition::VTP_Center:
        targetPos = shapeBRect.center();
        targetPos -= labelBRect.center();
        break;

    case VertexTitlePosition::VTP_Bottom:
        targetPos = shapeBRect.bottomLeft();
        targetPos.setX(targetPos.x() + (shapeBRect.width() - labelBRect.width()) / 2.0);
        break;

    case VertexTitlePosition::VTP_Top:
        targetPos = shapeBRect.center();
        targetPos.setY(- labelBRect.height());
        targetPos.setX(targetPos.x() - labelBRect.width() / 2.0);
        break;

    case VertexTitlePosition::VTP_RightBottom:
        targetPos = shapeBRect.bottomRight();
        targetPos += QPointF(-10, -10);
        break;
    }

    getLabel()->setPos(targetPos);
}

TextLabel *VertexItem::getLabel() const
{
    return m_nameItem;
}

QRectF toVertexBoundingRect(VertexSizeType vst)
{
    QRectF resultRect {0, 0, 150, 100};
    QTransform scaleTransform;
    switch (vst)
    {
    case VertexSizeType::VST_UltraSmall:
        scaleTransform.scale(0.2, 0.2);
        break;

    case VertexSizeType::VST_Small:
        scaleTransform.scale(0.5, 0.5);
        break;

    case VertexSizeType::VST_Medium:
        // Do nothing (normal size)
        break;

    case VertexSizeType::VST_Big:
        scaleTransform.scale(1.5, 1.5);
        break;

    case VertexSizeType::VST_Huge:
        scaleTransform.scale(3, 3);
        break;

    default:
        throw std::invalid_argument(std::string("Invalid vertex size type: ") + std::to_string(vst));
    }
    return scaleTransform.mapRect(resultRect);
}

}
