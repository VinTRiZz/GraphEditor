#include "grapheditmode.h"

#include "graphscenebase.h"

namespace Graph
{

GraphEditMode::GraphEditMode(QWidget *parent) :
    GraphModeBase(parent)
{

}

void GraphEditMode::init()
{

}

void GraphEditMode::start()
{
    // TODO: Setup button matrix

    setStarted();
}

void GraphEditMode::stop()
{
    // TODO: Reset button matrix

    setStopped();
}

void GraphEditMode::processPress(QGraphicsItem *pItem)
{
    auto pScene = getScene();

    // Нельзя выбирать соединения (пока что!)
    if (pItem->data(ObjectSceneConstants::OBJECTFIELD_OBJECTTYPE).toInt() == GraphSceneBase::OBJECT_TYPE_CONNECTION) {
        return;
    }

    if (nullptr != pScene->getGrabObject()) {
        pScene->rejectGrabObject();
        return;
    }

    pScene->setGrabObject(pItem);
}

void GraphEditMode::processMove(QGraphicsItem *pItem, const QPointF &currentPos)
{

}

void GraphEditMode::processRelease(QGraphicsItem *pItem)
{

}

}
