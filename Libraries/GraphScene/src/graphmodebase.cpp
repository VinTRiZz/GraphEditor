#include "graphmodebase.h"

#include "graphsceneview.h"

namespace Graph {

GraphModeBase::GraphModeBase(QObject *parent) : QObject{parent} {}

GraphModeBase::~GraphModeBase() {}

void GraphModeBase::setGraphScene(GraphSceneView *pScene) { m_pScene = pScene; }

void GraphModeBase::processPress(QGraphicsItem *pItem)
{
    m_currentSubmode->processPress(pItem);
}

void GraphModeBase::processMove(QGraphicsItem *pItem, const QPointF &currentPos)
{
    m_currentSubmode->processMove(pItem, currentPos);
}

void GraphModeBase::processRelease(QGraphicsItem *pItem)
{
    m_currentSubmode->processRelease(pItem);
}

void GraphModeBase::start()
{
    auto pButtonMatrix = getScene()->getButtonMatrixHead();
    for (auto* pMode : m_submodes) {
        pButtonMatrix->addButton(pMode->getStarterButton());
    }
}

void GraphModeBase::stop()
{
    auto pButtonMatrix = getScene()->getButtonMatrixHead();
    for (auto* pMode : m_submodes) {
        auto modeButton = pMode->getStarterButton();
        pButtonMatrix->removeButton(modeButton.positionX, modeButton.positionY);
    }
}

bool GraphModeBase::isRunning() const { return m_isModeStarted; }

void GraphModeBase::setStarted() {
  m_isModeStarted = true;
  emit started();
}

void GraphModeBase::setStopped() {
  m_isModeStarted = false;
  emit stopped();
}

void GraphModeBase::setSubmode(GraphSubmodeBase *pMode)
{
    m_currentSubmode = pMode;
}

GraphSubmodeBase *GraphModeBase::getCurrentSubmode() const
{
    return m_currentSubmode;
}

void GraphModeBase::addSubmode(GraphSubmodeBase *pMode)
{
    m_submodes.push_back(pMode);
}

std::list<GraphSubmodeBase *> GraphModeBase::getSubmodes() const
{
    return m_submodes;
}

GraphSceneView *GraphModeBase::getScene() const { return m_pScene; }

} // namespace Graph
