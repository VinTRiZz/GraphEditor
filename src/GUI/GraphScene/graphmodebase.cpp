#include "graphmodebase.h"


namespace Graph
{


GraphModeBase::GraphModeBase(QObject *parent)
    : QObject{parent}
{

}

GraphModeBase::~GraphModeBase()
{

}

void GraphModeBase::setGraphScene(GraphSceneBase *pScene)
{
    m_pScene = pScene;
}

}
