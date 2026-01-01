#include "plugincore.hpp"

#include <Components/Logger/Logger.h>

#include "simplevertexitem.hpp"
#include "imagevertexitem.hpp"

#include <QPainter>

namespace CommonPluginObjectName {
const QString SIMPLEVERTEX {"Simple vertex"};
const QString IMAGEVERTEX {"Image with name"};
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

Graph::PluginItemPropertyWidget *CommonPluginCore::getPropertyEditor(Graph::PluginObjectInterface *pTarget)
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

    if (name == CommonPluginObjectName::IMAGEVERTEX) {
        auto pImageVertex = new Graph::ImageVertexItem();
        pImageVertex->setPluginName(PLUGIN_NAME);
        pImageVertex->setPluginObjectName(CommonPluginObjectName::IMAGEVERTEX);

        auto emptyImageRect = QRect(0, 0, 500, 500);
        QImage emptyImg(emptyImageRect.width(), emptyImageRect.height(), QImage::Format_RGB32);
        emptyImg.fill(Qt::lightGray);

        QPainter imgPainter(&emptyImg);
        imgPainter.fillRect(emptyImageRect, QBrush(Qt::black, Qt::DiagCrossPattern));

        pImageVertex->setImage(emptyImg);
        return pImageVertex;
    }

    return nullptr;
}
