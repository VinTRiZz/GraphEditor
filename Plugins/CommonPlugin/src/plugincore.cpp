#include "plugincore.hpp"

#include <Components/Logger/Logger.h>

#include "simplevertexitem.hpp"
#include "imagevertexitem.hpp"

#include <QPainter>

namespace CommonPluginObjectName {
const QString SIMPLEVERTEX {"Simple vertex"};
const QString IMAGEVERTEX {"Image with name"};
const QString CONNECTIONLINE {"Connection line"};
}


CommonPluginCore::CommonPluginCore() :
    Graph::AbstractPluginCore(PLUGIN_NAME)
{
    registerObject(PluginObjectType::Vertex, CommonPluginObjectName::SIMPLEVERTEX);
    registerObject(PluginObjectType::Vertex, CommonPluginObjectName::IMAGEVERTEX);
}

CommonPluginCore::~CommonPluginCore()
{

}

Graph::PluginConfigurationWidget *CommonPluginCore::getConfigurationEditor()
{
    return nullptr;
}

Graph::PluginInteractionWidget *CommonPluginCore::getInteractor()
{
    return nullptr;
}

Graph::PluginObjectInterface *CommonPluginCore::createObject(const QString& name)
{
    if (name == CommonPluginObjectName::SIMPLEVERTEX) {
        auto pSimpleVertex = new Graph::SimpleVertexItem();
        pSimpleVertex->setPluginName(PLUGIN_NAME);
        pSimpleVertex->setPluginObjectName(CommonPluginObjectName::SIMPLEVERTEX);
        return pSimpleVertex;
    }

    if (name == CommonPluginObjectName::CONNECTIONLINE) {
        auto pLineItem = new Graph::GObjectConnectionItem;
        pLineItem->setPluginName(PLUGIN_NAME);
        pLineItem->setPluginObjectName(CommonPluginObjectName::CONNECTIONLINE);
        return pLineItem;
    }

    if (name == CommonPluginObjectName::IMAGEVERTEX) {
        auto pImageVertex = new Graph::ImageVertexItem();
        pImageVertex->setPluginName(PLUGIN_NAME);
        pImageVertex->setPluginObjectName(CommonPluginObjectName::IMAGEVERTEX);

        auto emptyImageRect = Graph::toVertexBoundingRect(pImageVertex->getSize());
        QImage emptyImg(emptyImageRect.width(), emptyImageRect.height(), QImage::Format_RGB32);
        emptyImg.fill(Qt::lightGray);

        QPainter imgPainter(&emptyImg);
        imgPainter.fillRect(emptyImageRect, QBrush(Qt::black, Qt::DiagCrossPattern));

        pImageVertex->setImage(emptyImg);
        return pImageVertex;
    }

    return nullptr;
}
