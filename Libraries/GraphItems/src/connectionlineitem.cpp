#include "connectionlineitem.h"

#include <AppInfrastructure/ApplicationSettings.h>
#include <Components/Logger/Logger.h>
#include <GraphObject/Object.h>
#include <Components/CustomQt/ObjectScene/Constants.h>


#include "vertexobjectitem.h"

using namespace ObjectViewItems;

namespace ObjectViewItems {

VertexConnectionLine::VertexConnectionLine(QGraphicsItem* parent)
    : ElegantArrowLine(parent) {
    setSystemName("Соединение вершин");

    auto& appSettings = ApplicationSettings::getInstance();

    ElegantArrowLine::setGradient1Color(appSettings.getObjectsConfig().m_defaultLineMainColor);
    ElegantArrowLine::setGradient2Color(appSettings.getObjectsConfig().m_defaultLineMainColor);

    ElegantArrowLine::setSelectionColor(
        appSettings.getObjectsConfig().m_defaultLineSelectionColor);

    auto pLabel = getLabel();
    pLabel->setBorderColor(appSettings.getObjectsConfig().m_defaultLabelTextColor);

    ElegantArrowLine::setWeight(1);
}

VertexConnectionLine::~VertexConnectionLine() {
    if (m_fromVertex) {
        m_fromVertex->unsubscribeConnectionFrom(this);
    }

    if (m_toVertex) {
        m_toVertex->unsubscribeConnectionTo(this);
    }
}

void VertexConnectionLine::setVertexFrom(VertexObject* pVertexFrom) {
    if (m_toVertex == pVertexFrom) {
        return;
    }
    m_fromVertex = pVertexFrom;
}

VertexObject* VertexConnectionLine::getVertexFrom() const {
    return m_fromVertex;
}

void VertexConnectionLine::setVertexTo(VertexObject* pVertexTo) {
    if (m_fromVertex == pVertexTo) {
        return;
    }
    m_toVertex = pVertexTo;
}

VertexObject* VertexConnectionLine::getVertexTo() const {
    return m_toVertex;
}

void VertexConnectionLine::resetPositions() {
    if (nullptr == m_toVertex) {
        return;
    }
    m_toVertex->updateConnectionLines();
}

}  // namespace ObjectViewItems
