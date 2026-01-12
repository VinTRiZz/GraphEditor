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

    ObjectItems::ArrowedConnectionLine* pLine {nullptr};
    createSubitem(pLine);
    pLine->setDirection(LineDirectionType::Forward);
    setLineItem(pLine);

    auto& appSettings = Common::ApplicationSettings::getInstance();

    m_connectionLine->setLinePen(Colors::DEFAULT_COLOR_CONNECTION_LINE);
    m_connectionLine->setLineSelectionPen(Colors::DEFAULT_COLOR_CONNECTION_SEL);

    setZValue(Layers::CONNECTION_LAYER);
}

GObjectConnectionItem::~GObjectConnectionItem() {

}

QJsonObject GObjectConnectionItem::toJson() const
{
    auto resJson = PluginObjectInterface::toJson();

    return resJson;
}

bool GObjectConnectionItem::fromJson(const QJsonObject &arr)
{
    auto res = PluginObjectInterface::fromJson(arr);

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

void GObjectConnectionItem::setLineItem(ObjectItems::AbstractConnectionLine *pLine)
{
    if (pLine == nullptr) {
        throw std::invalid_argument("VertexConnectionItem: Nullptr line item");
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

    if (nullptr != m_fromVertex) {
        auto bRect = m_fromVertex->boundingRect();
        if (m_isStraightLine) {
            auto betweenPos = m_fromVertex->pos() + bRect.center();
            auto resLine = QLineF(betweenPos, m_connectionLine->getLine().p2());

            auto hypo = QLineF(bRect.center(), bRect.topLeft()).length();
            auto linePosParameter = hypo * 1.2 / resLine.length();
            m_connectionLine->setPositionFrom(resLine.pointAt(linePosParameter));
        } else {
            m_connectionLine->setPositionFrom(QPointF(bRect.center().x() + m_fromVertex->x(), bRect.bottom() + m_fromVertex->y() + 5));
        }
    }

    if (nullptr != m_toVertex) {
        auto bRect = m_toVertex->boundingRect();
        if (m_isStraightLine) {
            auto betweenPos = m_toVertex->pos() + bRect.center();
            auto resLine = QLineF(m_connectionLine->getLine().p1(), betweenPos);

            auto hypo = QLineF(bRect.center(), bRect.topLeft()).length();
            auto linePosParameter = hypo * 1.2 / resLine.length();
            m_connectionLine->setPositionTo(resLine.pointAt(1 - linePosParameter));
        } else {
            m_connectionLine->setPositionTo(QPointF(bRect.center().x() + m_toVertex->x(), bRect.top() + m_toVertex->y() - 5));
        }
    }
}

}  // namespace ObjectItems
