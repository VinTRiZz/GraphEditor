#include "gobjectitem.hpp"

#include "gobjectconnectionitem.hpp"

#include <Components/Logger/Logger.h>

#include <QPainterPathStroker>

using namespace ObjectItems;

namespace Graph {

void GObjectItem::updateSelectionPathItem()
{
    QPainterPath selectionShape;

    auto selfBRect = toVertexBoundingRect(getSize());
    QTransform scaleTr;
    scaleTr.scale(1.2, 1.2);
    selfBRect = scaleTr.mapRect(selfBRect);
    selfBRect.moveCenter(toVertexBoundingRect(getSize()).center());
    selectionShape.addRoundedRect(selfBRect, 10, 10);

    QPainterPathStroker strk;
    selectionShape = strk.createStroke(selectionShape);

    m_selectionPathItem->setPath(selectionShape);
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
    m_nameItem->setFlag(ItemIsMovable, false);

    createSubitem<false>(m_selectionPathItem);
    m_selectionPathItem->setZValue(100);
    m_selectionPathItem->setPen(Colors::DEFAULT_COLOR_VERTEX_SEL);
    m_selectionPathItem->setBrush(Colors::DEFAULT_COLOR_VERTEX_SEL);
    updateSelectionPathItem();
    m_selectionPathItem->setFlag(ItemClipsToShape, true);
    m_selectionPathItem->hide();

    setFlag(ItemIsSelectable, true);
    setFlag(ItemIsMovable, true);

    setLinePen(Colors::DEFAULT_COLOR_VERTEX_LINE);
    setBackgroundBrush(Colors::DEFAULT_COLOR_VERTEX_BGR);

    setZValue(Layers::VERTEX_LAYER);

    connect(this, &BasicItem::displayNameChanged,
            m_nameItem, [this](){
        if (m_isTextEditedByUser) { return; }
        m_nameItem->setDisplayName(getDisplayName());
        updateLabelPosition();
    });

    connect(m_nameItem, &BasicItem::displayNameChanged,
            this, [this](){
        m_isTextEditedByUser = true;
        setDisplayName(m_nameItem->getDisplayName());
        m_isTextEditedByUser = false;
    });

    connect(this, &BasicItem::idChanged,
            this, [this](){ setPluginObjectId(getItemId()); });

    connect(this, &BasicItem::itemSelected,
            this, [this](){
        updateSelectionPathItem();
        m_selectionPathItem->setVisible(true);
    });
    connect(this, &BasicItem::itemDeselected,
            this, [this](){
        m_selectionPathItem->setVisible(false);
    });

    setDisplayName("Объект без названия");
    m_nameItem->setEditableByUser(true);
}

GObjectItem::~GObjectItem()
{

}

QJsonObject GObjectItem::toJson() const
{
    auto res = PluginObjectInterface::toJson();

    QJsonObject vObj;
    vObj["isVisible"] = isVisible();
    vObj["pos"] = QString("%0;%1").arg(QString::number(pos().x()), QString::number(pos().y()));
    vObj["size"] = static_cast<int>(getSize());
    vObj["name"] = getDisplayName().toUtf8().toHex().data();
    vObj["description"] = getDescription().toUtf8().toHex().data();

    res["GObjectItem"] = vObj;
    return res;
}

bool GObjectItem::fromJson(const QJsonObject &jsonObj)
{
    auto res = PluginObjectInterface::fromJson(jsonObj);
    if (!res) { return res; }
    auto vObj = jsonObj["GObjectItem"].toObject();

    setItemId(getPluginObjectId()); // Он уже получен уровнем выше

    setDisplayName(QByteArray::fromHex(vObj["name"].toString().toUtf8()));
    setDescription(QByteArray::fromHex(vObj["description"].toString().toUtf8()));

    setVisible(vObj["isVisible"].toBool());
    auto serializedPos = vObj["pos"].toString().split(";");
    setPos(QPointF(serializedPos[0].toDouble(), serializedPos[1].toDouble()));

    setSize(static_cast<VertexSizeType>(vObj["size"].toInt(VertexSizeType::VST_Medium)));

    return res;
}

void GObjectItem::setSize(VertexSizeType vst)
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
    updateSelectionPathItem();

    processSizeTypeChange(toVertexBoundingRect(vst));
    emit sizeChanged(prevSizeT, m_vertexSizeType);
}

VertexSizeType GObjectItem::getSize() const
{
    return m_vertexSizeType;
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

void GObjectItem::slotProcessEvent(ConnectionEvent *cEvent)
{
    processConnectionEvent(cEvent);
}

void GObjectItem::updateLabelPosition()
{
    auto shapeBRect = toVertexBoundingRect(getSize());
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

void GObjectItem::processSizeTypeChange(const QRectF &newSize) {
    updateSelectionPathItem();
}

void GObjectItem::processConnectionEvent(ConnectionEvent *pEvent)
{

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
