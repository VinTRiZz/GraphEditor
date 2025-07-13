#include "grapheditview.h"

namespace Graph
{

GraphEditView::GraphEditView(QWidget *parent) :
    GraphSceneBase(parent)
{
    connect(&m_viewMode, &GraphViewMode::started,
            this, &GraphEditView::startedView);
    connect(&m_editMode, &GraphEditMode::started,
            this, &GraphEditView::startedEdit);
}

void GraphEditView::init()
{
    m_viewMode.init();
    m_editMode.init();
    GraphSceneBase::init();

    startViewMode();
}


void GraphEditView::setCurrentGraph(GraphExtendedObject *pGraph)
{
    // TODO: Connect graph
    GraphSceneBase::setCurrentGraph(pGraph);
}

void GraphEditView::startViewMode()
{
    if (m_viewMode.isRunning()) {
        return;
    }
    m_editMode.stop();
    m_viewMode.start();
}

void GraphEditView::startEditMode()
{
    if (m_editMode.isRunning()) {
        return;
    }
    m_viewMode.stop();
    m_editMode.start();
}

}
