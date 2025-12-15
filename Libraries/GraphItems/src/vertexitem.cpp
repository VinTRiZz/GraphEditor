#include "vertexitem.hpp"

#include "constants.hpp"
#include "vertexconnectionitem.hpp"

#include <Components/Logger/Logger.h>

using namespace ObjectItems;

namespace Graph {

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

void VertexItem::fromGObject(const GObject &vert)
{
    setItemId(vert.getId().value());

    auto itemPos = vert.getPos();
    if (parentItem()) {
        itemPos = parentItem()->mapFromScene(itemPos);
    }
    setPos(itemPos);

    setDisplayName(vert.getName());

    auto commonVertexItemData = vert.getCommonData();
    auto getCommonData = [&commonVertexItemData](const auto& fieldName) -> QJsonValue {
        if (commonVertexItemData.contains(fieldName)) {
            return commonVertexItemData[fieldName];
        }
        return QJsonValue();
    };

    setPluginName(getCommonData(GObjectValueName::COMMON_PLUGIN_NAME).toString());
    setPluginObjectName(getCommonData(GObjectValueName::COMMON_PLUGIN_OBJECTNAME).toString());

    setToolTip(getCommonData(GObjectValueName::COMMON_TOOLTIP).toString());
    setDescription(getCommonData(GObjectValueName::COMMON_DESCRIPTION).toString());

    setLinePen(qvariant_cast<QPen>(getCommonData(GObjectValueName::COMMON_LINEPEN).toVariant()));
    setBackgroundBrush(qvariant_cast<QBrush>(getCommonData(GObjectValueName::COMMON_BGRBRUSH).toVariant()));
    setTitlePosition(VertexTitlePosition(getCommonData(GObjectValueName::COMMON_TITLEPOS).toInt()));
}

GObject VertexItem::toGObject() const
{
    GObject graphVertex;

    graphVertex.setId(getItemId());
    graphVertex.setPos(scenePos());
    graphVertex.setName(getDisplayName());

    QJsonObject commonVertexItemData;

    auto setCommonData = [&commonVertexItemData](const auto& fieldName, const auto& value) -> void {
        commonVertexItemData[fieldName] = value;
    };

    setCommonData(GObjectValueName::COMMON_PLUGIN_NAME, getPluginName());
    setCommonData(GObjectValueName::COMMON_PLUGIN_OBJECTNAME, getPluginObjectName());

    setCommonData(GObjectValueName::COMMON_TOOLTIP, toolTip());
    setCommonData(GObjectValueName::COMMON_DESCRIPTION, getDescription());
    setCommonData(GObjectValueName::COMMON_LINEPEN, QJsonValue::fromVariant(QVariant::fromValue(getLinePen())));
    setCommonData(GObjectValueName::COMMON_BGRBRUSH, QJsonValue::fromVariant(QVariant::fromValue(getBackgroundBrush())));

    setCommonData(GObjectValueName::COMMON_TITLEPOS, getTitlePosition());
    graphVertex.setCommonData(commonVertexItemData);

    for (auto* pConnection : m_connectionsFromThis) {
        if (nullptr == pConnection->getVertexTo()) {
            LOG_WARNING("Invalid connection got (no VERTEX-TO set)");
            continue;
        }
        graphVertex.addConnection(pConnection->getVertexTo()->getItemId());
    }

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
    connect(pLine, &QObject::destroyed,
            this, [this, pLine](){
        m_connectionsFromThis.erase(pLine);
    });
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
    connect(pLine, &QObject::destroyed,
            this, [this, pLine](){
        m_connectionsToThis.erase(pLine);
    });
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
