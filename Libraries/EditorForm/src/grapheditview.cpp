#include "grapheditview.h"

#include <Common/Logging.h>

namespace Graph
{

GraphEditView::GraphEditView(QWidget *parent) :
    GraphSceneBase(parent)
{

}

void GraphEditView::init()
{
    connect(&m_viewMode, &GraphViewMode::started,
            this, &GraphEditView::startedView);
    connect(&m_editMode, &GraphEditMode::started,
            this, &GraphEditView::startedEdit);

    m_viewMode.setGraphScene(this);
    m_editMode.setGraphScene(this);

    GraphSceneBase::init();
    m_viewMode.init();
    m_editMode.init();
    LOG_INFO("Inited graph view");

    startViewMode();
}


void GraphEditView::setCurrentGraph(GraphExtendedObject *pGraph)
{
    // TODO: Connect graph
    GraphSceneBase::setCurrentGraph(pGraph);
}

void GraphEditView::startViewMode()
{
    setMode(&m_viewMode);
    LOG_INFO("Started view mode");
}

void GraphEditView::startEditMode()
{
    setMode(&m_editMode);
    LOG_INFO("Started edit mode");
}

bool GraphEditView::isEditMode() const
{
    return m_editMode.isRunning();
}

bool GraphEditView::isViewMode() const
{
    return m_viewMode.isRunning();
}

}
