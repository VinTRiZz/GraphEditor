#include "vertexobjectitem.h"

#include <AppInfrastructure/ApplicationSettings.h>
#include <AppInfrastructure/ImageManager.h>
#include <Components/Logger/Logger.h>
#include <GraphObject/Object.h>
#include <Components/CustomQt/ObjectScene/Constants.h>

#include <QBuffer>
#include <QFileInfo>
#include <QGraphicsSceneMouseEvent>
#include <QLabel>
#include <QTextOption>

#include "connectionlineitem.h"

using namespace ObjectViewConstants;

namespace ObjectViewItems {

VertexObject::VertexObject(QGraphicsItem* parent) : PictureObjectItem(parent) {
    setSystemName("Вершина");

    setType(ObjectViewConstants::OBJECTTYPE_VERTEX);

    setFlag(QGraphicsItem::ItemIsSelectable, true);
    setFlag(QGraphicsItem::ItemClipsToShape, true);
    setFlag(QGraphicsItem::ItemSendsScenePositionChanges, true);

    auto& appSettings = ApplicationSettings::getInstance();

    PictureObjectItem::setSelectionColor(
        appSettings.getObjectsConfig().m_defaultSelectionColor);
    PictureObjectItem::setBackgroundColor(
        appSettings.getObjectsConfig().m_defaultSecondColor);
    PictureObjectItem::setBorderColor(
        appSettings.getObjectsConfig().m_defaultMainColor);
}

VertexObject::~VertexObject() {
    for (auto pLine : m_connectionsToThis) {
        pLine->setVertexTo(nullptr);
        pLine->unregister();
    }

    for (auto pLine : m_connectionsFromThis) {
        pLine->setVertexFrom(nullptr);
        pLine->unregister();
    }
}

void VertexObject::setImageByHash(const QString& imageHash) {
    auto& imgManager = ImageManager::getInstance();
    auto img = imgManager.getImageByHash(imageHash);
    PictureObjectItem::setImage(img, imageHash);
}

void VertexObject::setBorderColor(const QColor& penColor) {
    PictureObjectItem::setBorderColor(penColor.isValid()
                                          ? penColor
                                          : ApplicationSettings::getInstance()
                                                .getObjectsConfig()
                                                .m_defaultMainColor);
}

void VertexObject::setBackgroundColor(const QColor& penColor) {
    PictureObjectItem::setBackgroundColor(
        penColor.isValid() ? penColor
                           : ApplicationSettings::getInstance()
                                 .getObjectsConfig()
                                 .m_defaultSecondColor);
}

bool VertexObject::isLineSubscribed(VertexConnectionLine* pLine) {
    // Нет смысла проверять исходящие, т.к. нельзя регистрировать вершину саму
    // на себя
    for (auto pLineTo : m_connectionsToThis) {
        if (pLineTo->getVertexFrom()->getObjectId() ==
            pLine->getVertexFrom()->getObjectId()) {
            return true;
        }
    }
    return false;
}

void VertexObject::subscribeAsConnectionFrom(VertexConnectionLine* pLine) {
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

void VertexObject::unsubscribeConnectionFrom(VertexConnectionLine* pLine) {
    pLine->setVertexFrom(nullptr);
    m_connectionsFromThis.erase(pLine);
}

void VertexObject::subscribeAsConnectionTo(VertexConnectionLine* pLine) {
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

void VertexObject::unsubscribeConnectionTo(VertexConnectionLine* pLine) {
    pLine->setVertexTo(nullptr);
    m_connectionsToThis.erase(pLine);
}

QVariant VertexObject::itemChange(GraphicsItemChange change,
                                  const QVariant& value) {
    if (change == ItemPositionChange) {
        updateConnectionLines();
    }

    return PictureObjectItem::itemChange(change, value);
}

void VertexObject::updateConnectionLines() {
    unsigned connectionNumber{0};
    auto vertexRadius = static_cast<double>(boundingRect().width()) / 2.0;

    for (auto pConFrom : m_connectionsFromThis) {
        auto fromPos =
            QPointF(x() + vertexRadius,
                    y() + 2 * vertexRadius + pConFrom->getArrowSize() +
                        getLabel()->boundingRect().height() * 0.7);

        pConFrom->setPositionFrom(fromPos);
        connectionNumber++;
    }

    connectionNumber = 0;
    for (auto pConTo : m_connectionsToThis) {
        auto toPos = QPointF(x() + vertexRadius, y() - pConTo->getArrowSize());
        pConTo->setPositionTo(toPos);
        connectionNumber++;
    }
}

}  // namespace ObjectViewItems
