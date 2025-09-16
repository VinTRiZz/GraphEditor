#include "grapheditview.h"

#include <Common/Logging.h>

#include "grapheditmode.h"
#include "graphviewmode.h"

#include "submodes/propertyeditmode.h"

namespace Graph {

GraphEditView::GraphEditView(QWidget* parent) : GraphSceneView(parent) {

    m_viewMode = new GraphViewMode(this);
    m_editMode = new GraphEditMode(this);

    connect(m_viewMode, &GraphViewMode::started, this,
            &GraphEditView::startedView);
    m_viewMode->setGraphScene(this);

    connect(m_editMode, &GraphEditMode::started, this,
            &GraphEditView::startedEdit);
    m_editMode->setGraphScene(this);

    m_viewMode->init();

    m_editMode->init();
    connect(m_editMode->getPropertyEditMode(),
            &PropertyEditMode::openPropertyEditor, this,
            &GraphEditView::openPropertyEditor);
    connect(m_editMode->getPropertyEditMode(),
            &PropertyEditMode::closePropertyEditor, this,
            &GraphEditView::closePropertyEditor);

    LOG_INFO("Inited graph view");

    startEditMode();  // TODO: Replace with VIEW mode when it's ready
}

void GraphEditView::setGraphMaintaner(const PMaintainer& pGraphMaintaner) {
    // TODO: Connect graph
    GraphSceneView::setGraphMaintaner(pGraphMaintaner);
}

void GraphEditView::startViewMode() {
    setMode(m_viewMode);
    LOG_INFO("Started view mode");
}

void GraphEditView::startEditMode() {
    setMode(m_editMode);
    LOG_INFO("Started edit mode");
}

bool GraphEditView::isEditMode() const {
    return m_editMode->isRunning();
}

bool GraphEditView::isViewMode() const {
    return m_viewMode->isRunning();
}

}  // namespace Graph
