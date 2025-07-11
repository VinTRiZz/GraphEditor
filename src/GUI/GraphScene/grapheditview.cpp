#include "grapheditview.h"

namespace Graph
{

GraphEditView::GraphEditView(QWidget *parent) :
    GraphSceneBase(parent)
{

}

void GraphEditView::init()
{
    m_viewMode.init();
    m_editMode.init();
    GraphSceneBase::init();
}


void GraphEditView::setCurrentGraph(GraphExtendedObject *pGraph)
{
    // TODO: Connect graph
    GraphSceneBase::setCurrentGraph(pGraph);
}

}
