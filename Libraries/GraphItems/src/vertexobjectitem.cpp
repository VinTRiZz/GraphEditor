#include "vertexobjectitem.h"

#include <AppInfrastructure/GraphEditorSettings.h>
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

using namespace ObjectViewItems;

namespace Graph {

VertexObjectItem::VertexObjectItem(QGraphicsItem* parent) : VertexItemBase(parent) {
    setSystemName("Вершина");

    setType(ObjectViewItems::ObjectType(OBJECTTYPE_VERTEX));

    setFlag(QGraphicsItem::ItemIsSelectable, true);
    setFlag(QGraphicsItem::ItemClipsToShape, true);
    setFlag(QGraphicsItem::ItemSendsScenePositionChanges, true);

    auto& appSettings = GraphEditorSettings::getInstance();

    m_vertexImage = new ObjectViewItems::PictureObjectItem(this);
    registerSubitem(m_vertexImage);
    m_vertexImage->setSelectionColor(
        appSettings.getObjectsConfig().m_defaultSelectionColor);
    m_vertexImage->setBackgroundColor(
        appSettings.getObjectsConfig().m_defaultSecondColor);
    m_vertexImage->setBorderColor(
        appSettings.getObjectsConfig().m_defaultMainColor);

    setZValue(Layers::VERTEX_LAYER);

    QRect vertexRect;
    vertexRect.setWidth(Sizes::VERTEX_RADIUS * 2);
    vertexRect.setHeight(Sizes::VERTEX_RADIUS * 2);
    m_vertexImage->setRect(vertexRect);
}

VertexObjectItem::~VertexObjectItem() {
    for (auto pLine : m_connectionsToThis) {
        pLine->setVertexTo(nullptr);
        pLine->unregister();
    }

    for (auto pLine : m_connectionsFromThis) {
        pLine->setVertexFrom(nullptr);
        pLine->unregister();
    }
}

void VertexObjectItem::fromVertex(const GVertex &vert)
{
    setObjectId(vert.id);
    setPos(vert.posX, vert.posY);

    setDisplayName(vert.shortName);
    setToolTip(vert.name);
    setDescription(vert.description);

    setBorderColor(vert.borderColor);
    setBackgroundColor(vert.backgroundColor);

    if (!vert.image.isNull()) {
        m_vertexImage->setImage(vert.image);
        // TODO: Задавать хеш изображения на основе данных в файле
    }
}

GVertex VertexObjectItem::toVertex() const
{
    GVertex graphVertex;
    graphVertex.id = getObjectId();
    graphVertex.posX = x();
    graphVertex.posY = y();

    graphVertex.shortName = getDisplayName();
    graphVertex.name = toolTip();
    graphVertex.description = getDescription();

    graphVertex.borderColor = getBorderColor();
    graphVertex.backgroundColor = getBackgroundColor();

    graphVertex.image = m_vertexImage->getImage();

    return graphVertex;
}

void VertexObjectItem::setImageByHash(const QString& imageHash) {
    auto& imgManager = ImageManager::getInstance();
    auto img = imgManager.getImageByHash(imageHash);
    m_vertexImage->setImage(img);

    // TODO: Задавать хеш изображения
}

void VertexObjectItem::setBorderColor(const QColor& penColor) {
    m_vertexImage->setBorderColor(penColor.isValid()
                                          ? penColor
                                          : GraphEditorSettings::getInstance()
                                                .getObjectsConfig()
                                                .m_defaultMainColor);
}

void VertexObjectItem::setBackgroundColor(const QColor& penColor) {
    m_vertexImage->setBackgroundColor(
        penColor.isValid() ? penColor
                           : GraphEditorSettings::getInstance()
                                 .getObjectsConfig()
                                 .m_defaultSecondColor);
}

QPainterPath VertexObjectItem::shape() const {
    QPainterPath res = m_vertexImage->shape();
    res.addPath(VertexItemBase::shape());
    return res;
}

}  // namespace ObjectViewItems
