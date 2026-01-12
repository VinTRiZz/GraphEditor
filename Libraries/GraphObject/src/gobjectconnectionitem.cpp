#include "gobjectconnectionitem.hpp"

#include <Components/Common/ApplicationSettings.h>
#include <Components/Logger/Logger.h>
#include <GraphObject/GraphObject.h>

#include "gobjectitem.hpp"

#include <math.h>

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
        m_connectionLine->setPositionFrom(QPointF(bRect.center().x() + m_fromVertex->x(), bRect.bottom() + m_fromVertex->y() + 5));
    }

    if (nullptr != m_toVertex) {
        auto bRect = m_toVertex->boundingRect();
        if (m_isStraightLine) {
            auto angle = m_connectionLine->getLine().angle();
            auto hypot = m_connectionLine->getLine().length();
            auto p1 = m_connectionLine->getLine().p1();

            double xOffset = bRect.width() / 2.0 * std::cos(angle);
            double yOffset = bRect.height() / 2.0 * std::sin(angle);

            LOG_DEBUG("VALUES:", angle, hypot, p1, xOffset, yOffset);
            auto targetPos = QPointF(p1.x() + hypot * std::cos(angle) + xOffset,
                                    p1.y() + hypot * std::sin(angle) + yOffset);
            m_connectionLine->setPositionTo(targetPos);
        } else {
            m_connectionLine->setPositionTo(QPointF(bRect.center().x() + m_toVertex->x(), bRect.top() + m_toVertex->y() - 5));
        }
    }
}

}  // namespace ObjectItems
