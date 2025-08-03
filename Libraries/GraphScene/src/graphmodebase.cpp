#include "graphmodebase.h"

#include "graphsceneview.h"

namespace Graph {

GraphModeBase::GraphModeBase(QObject* parent) : QObject{parent} {}

GraphModeBase::~GraphModeBase() {}

void GraphModeBase::setGraphScene(GraphSceneView* pScene) {
    m_pScene = pScene;
}

bool GraphModeBase::isRunning() const {
    return m_isModeStarted;
}

void GraphModeBase::setStarted() {
    m_isModeStarted = true;
    emit started();
}

void GraphModeBase::setStopped() {
    m_isModeStarted = false;
    emit stopped();
}

GraphSceneView* GraphModeBase::getScene() const {
    return m_pScene;
}

}  // namespace Graph
