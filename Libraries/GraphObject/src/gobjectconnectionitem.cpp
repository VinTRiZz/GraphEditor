#include "gobjectconnectionitem.hpp"

#include <Components/Common/ApplicationSettings.h>
#include <Components/Logger/Logger.h>
#include <GraphObject/GraphObject.h>

#include "gobjectitem.hpp"

#include <math.h>

#include <QLineF>
#include <QRectF>
#include <QPointF>
#include <optional>

using namespace ObjectItems;

namespace Graph {

GObjectConnectionItem::GObjectConnectionItem(QGraphicsItem* parent)
    : BasicItem(parent) {
    setSystemName("Соединение вершин");
    setObjectType(OBJECTTYPE_CONNECTION);

    setDisplayName("Соединение");

    setZValue(Layers::CONNECTION_LAYER);

    // TODO: Подумать на этот счёт, выглядит как костыль из-за логики с плагинами
    setPluginName("CommonPlugin");
    setPluginObjectName("Connection line");

    connect(this, &BasicItem::idChanged,
            this, [this](){ setPluginObjectId(getItemId()); });
}

GObjectConnectionItem::~GObjectConnectionItem() {

}

QJsonObject GObjectConnectionItem::toJson() const
{
    auto resJson = PluginObjectInterface::toJson();

    QJsonObject conInfoJ;
    if (auto pCon = dynamic_cast<PluginObjectInterface*>(m_connectionLine); nullptr != pCon) {
        conInfoJ["lineData"] = pCon->toJson();
    } else {
        conInfoJ["lineData"] = {};
    }

    conInfoJ["lineType"] = m_isStraightLine; // TODO: Продумать для других типов соединений?
    conInfoJ["idFrom"] = (nullptr == m_fromVertex ? 0 : m_fromVertex->getItemId());
    conInfoJ["idTo"] = (nullptr == m_toVertex ? 0 : m_toVertex->getItemId());

    resJson["GObjectConnectionItem"] = conInfoJ;
    return resJson;
}

bool GObjectConnectionItem::fromJson(const QJsonObject &arr)
{
    auto res = PluginObjectInterface::fromJson(arr);

    auto conInfoJ = arr["GObjectConnectionItem"].toObject();
    setItemId(getPluginObjectId()); // Он уже получен уровнем выше

    if (conInfoJ["lineType"].toBool()) {
        setLineItem(new ConnectionLineBase<ObjectItems::ArrowedConnectionLine>(this));
    } else {
        setLineItem(new ConnectionLineBase<ObjectItems::ElegantConnectionLine>(this));
    }

    dynamic_cast<PluginObjectInterface*>(m_connectionLine)->fromJson(conInfoJ["lineData"].toObject());
    m_loadedFromId = conInfoJ["idFrom"].toInt();
    m_loadedToId = conInfoJ["idTo"].toInt();

    return res;
}

void GObjectConnectionItem::setVertexFrom(GObjectItem* pVertexFrom) {
    if (nullptr != m_fromVertex) {
        disconnect(m_fromVertex, nullptr, this, nullptr);
    }

    m_fromVertex = pVertexFrom;
    if (nullptr != m_fromVertex) {
        connect(m_fromVertex, &QObject::destroyed,
                this, [this](){ m_fromVertex = nullptr; });
        connect(m_fromVertex, &BasicItem::itemMoved,
                this, &GObjectConnectionItem::updateLine);
    }
    updateLine();
}

GObjectItem* GObjectConnectionItem::getVertexFrom() const {
    return m_fromVertex;
}

void GObjectConnectionItem::setVertexTo(GObjectItem* pVertexTo) {
    if (nullptr != m_toVertex) {
        disconnect(m_toVertex, nullptr, this, nullptr);
    }

    m_toVertex = pVertexTo;
    if (nullptr != m_toVertex) {
        connect(m_toVertex, &QObject::destroyed,
                this, [this](){ m_toVertex = nullptr; });
        connect(m_toVertex, &BasicItem::itemMoved,
                this, &GObjectConnectionItem::updateLine);
    }
    updateLine();
}

GObjectItem* GObjectConnectionItem::getVertexTo() const {
    return m_toVertex;
}

std::pair<graphId_t, graphId_t> GObjectConnectionItem::getVertexIds() const
{
    return std::make_pair(m_loadedFromId, m_loadedToId);
}

void GObjectConnectionItem::setLineItem(ObjectItems::AbstractConnectionLine *pLine)
{
    if (pLine == nullptr ||
            nullptr == dynamic_cast<PluginObjectInterface*>(pLine)) {
        throw std::invalid_argument("VertexConnectionItem: Nullptr or invalid line item");
    }
    delete m_connectionLine;
    m_connectionLine = pLine;
    pLine->setParentItem(this);

    m_isStraightLine = (nullptr != dynamic_cast<ObjectItems::ArrowedConnectionLine*>(pLine));
}

AbstractConnectionLine *GObjectConnectionItem::getLineItem() const
{
    return m_connectionLine;
}

void GObjectConnectionItem::updateLine()
{
    if (nullptr == m_connectionLine) [[unlikely]] {
        return;
    }

    // Straight
    if (m_isStraightLine) {
        if (nullptr != m_fromVertex) {
            auto bRect = m_fromVertex->boundingRect();
            auto betweenPos = m_fromVertex->pos() + bRect.center();
            auto resLine = QLineF(betweenPos, m_connectionLine->getLine().p2());

            auto hypo = QLineF(bRect.center(), bRect.topLeft()).length();
            auto linePosParameter = hypo * 1.2 / resLine.length();
            m_connectionLine->setPositionFrom(resLine.pointAt(linePosParameter));
        }

        if (nullptr != m_toVertex) {
            auto bRect = m_toVertex->boundingRect();
            auto betweenPos = m_toVertex->pos() + bRect.center();
            auto resLine = QLineF(m_connectionLine->getLine().p1(), betweenPos);

            auto hypo = QLineF(bRect.center(), bRect.topLeft()).length();
            auto linePosParameter = hypo * 1.2 / resLine.length();
            m_connectionLine->setPositionTo(resLine.pointAt(1 - linePosParameter));
        }
        return;
    }

    // Not straight
    if (nullptr != m_fromVertex) {
        auto bRect = m_fromVertex->boundingRect();
        m_connectionLine->setPositionFrom(QPointF(bRect.center().x() + m_fromVertex->x(), bRect.bottom() + m_fromVertex->y() + 5));
    }
    if (nullptr != m_toVertex) {
        auto bRect = m_toVertex->boundingRect();
        m_connectionLine->setPositionTo(QPointF(bRect.center().x() + m_toVertex->x(), bRect.top() + m_toVertex->y() - 5));
    }
}

}  // namespace ObjectItems
