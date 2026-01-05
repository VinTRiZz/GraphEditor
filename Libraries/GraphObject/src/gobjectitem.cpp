#include "gobjectitem.hpp"

#include "gobjectconnectionitem.hpp"

#include <Components/Logger/Logger.h>

using namespace ObjectItems;

namespace Graph {

bool GObjectItem::isLineSubscribed(GObjectConnectionItem* pLine) {
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

GObjectItem::GObjectItem(QGraphicsItem *parent) :
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
            this, &GObjectItem::updateConnectionLines);
    connect(this, &BasicItem::idChanged,
            this, [this](){ setPluginObjectId(getItemId()); });

    setDisplayName("Объект без названия");
}

GObjectItem::~GObjectItem()
{
    for (auto pLine : m_connectionsToThis) {
        pLine->setVertexTo(nullptr);
    }

    for (auto pLine : m_connectionsFromThis) {
        pLine->setVertexFrom(nullptr);
    }
}

QJsonObject GObjectItem::toJson() const
{
    auto res = PluginObjectInterface::toJson();

    QJsonObject vObj;
    vObj["isVisible"] = isVisible();
    vObj["pos"] = QString("%0;%1").arg(QString::number(pos().x()), QString::number(pos().y()));
    vObj["size"] = static_cast<int>(getVertexSizeType());

    res["GObjectItem"] = vObj;
    return res;
}

bool GObjectItem::fromJson(const QJsonObject &jsonObj)
{
    auto res = PluginObjectInterface::fromJson(jsonObj);
    if (!res) { return res; }
    auto vObj = jsonObj["GObjectItem"].toObject();

    setItemId(getPluginObjectId()); // Он уже получен уровнем выше

    setVisible(vObj["isVisible"].toBool());
    auto serializedPos = vObj["pos"].toString().split(";");
    setPos(QPointF(serializedPos[0].toDouble(), serializedPos[1].toDouble()));

    setVertexSizeType(static_cast<VertexSizeType>(vObj["size"].toInt(VertexSizeType::VST_Medium)));

    return res;
}

void GObjectItem::setItemNotFound()
{
    QGraphicsRectItem* pInvalidRect {nullptr};
    createSubitem(pInvalidRect);
    pInvalidRect->setBrush(QBrush(Qt::darkMagenta, Qt::DiagCrossPattern));

    QGraphicsSimpleTextItem* pInvalidText {nullptr};
    createSubitem(pInvalidText);
    pInvalidText->setText(QString("Not found:\n%0::%1").arg(getPluginName(), getPluginObjectName()));

    m_isItemFound = false;
}

bool GObjectItem::isItemFound() const
{
    return m_isItemFound;
}

void GObjectItem::setVertexSizeType(VertexSizeType vst)
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

VertexSizeType GObjectItem::getVertexSizeType() const
{
    return m_vertexSizeType;
}

void GObjectItem::subscribeAsConnectionFrom(GObjectConnectionItem* pLine) {
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

void GObjectItem::unsubscribeConnectionFrom(GObjectConnectionItem* pLine) {
    pLine->setVertexFrom(nullptr);
    m_connectionsFromThis.erase(pLine);
}

void GObjectItem::subscribeAsConnectionTo(GObjectConnectionItem* pLine) {
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

void GObjectItem::unsubscribeConnectionTo(GObjectConnectionItem* pLine) {
    pLine->setVertexTo(nullptr);
    m_connectionsToThis.erase(pLine);
}

void GObjectItem::updateConnectionLines() {
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

void GObjectItem::setTitlePosition(VertexTitlePosition vtp)
{
    m_shapeTitlePos = vtp;
    updateLabelPosition();
}

VertexTitlePosition GObjectItem::getTitlePosition() const
{
    return m_shapeTitlePos;
}

void GObjectItem::updateLabelPosition()
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

TextLabel *GObjectItem::getLabel() const
{
    return m_nameItem;
}

void GObjectItem::processSizeTypeChange(const QRectF &newSize) {}

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
