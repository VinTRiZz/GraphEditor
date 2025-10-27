#include "graphmodebase.h"

#include "graphsceneview.h"

namespace Graph {

GraphSubmodeBase::GraphSubmodeBase(GraphModeBase* parentMode)
    : m_pParentMode{parentMode}, m_context{parentMode->getContext()} {
    // Дефолтный конфиг
    m_starterButton.action = [this](QPushButton* pButton) -> void {
        emit requestModeStart();
    };
    m_starterButton.isEnabled = true;
}

ButtonMatrix::ButtonConfig& GraphSubmodeBase::getStarterButton() {
    return m_starterButton;
}

bool GraphSubmodeBase::isModeActive() const {
    return (m_pParentMode->getCurrentSubmode() == this);
}

GraphModeBase::GraphModeBase(QObject* parent) : QObject{parent} {}

GraphModeBase::~GraphModeBase() {}

void GraphModeBase::setGraphScene(GraphSceneView* pScene) {
    m_pScene = pScene;
}

void GraphModeBase::processPress(QGraphicsItem* pItem) {
    m_currentSubmode->processPress(pItem);
}

void GraphModeBase::processMove(QGraphicsItem* pItem,
                                const QPointF& currentPos) {
    m_currentSubmode->processMove(pItem, currentPos);
}

void GraphModeBase::processRelease(QGraphicsItem* pItem) {
    m_currentSubmode->processRelease(pItem);
}

void GraphModeBase::clearCurrentMode() {
    if (nullptr == m_currentSubmode) {
        return;
    }

    m_currentSubmode->clearMode();
    auto& buttonConf = m_currentSubmode->getStarterButton();
    auto pButton = getScene()->getButtonMatrixHead()->getButton(
        buttonConf.positionX, buttonConf.positionY);
    pButton->setIcon(buttonConf.icon);
}

void GraphModeBase::start() {
    auto pButtonMatrix = getScene()->getButtonMatrixHead();
    for (auto* pMode : m_submodes) {
        pButtonMatrix->addButton(pMode->getStarterButton());
    }
    setSubmode(m_submodes.front());
}

void GraphModeBase::stop() {
    auto pButtonMatrix = getScene()->getButtonMatrixHead();
    for (auto* pMode : m_submodes) {
        auto modeButton = pMode->getStarterButton();
        pButtonMatrix->removeButton(modeButton.positionX, modeButton.positionY);
    }
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

void GraphModeBase::setSubmode(GraphSubmodeBase* pMode) {
    // Полный дисконнект
    if (nullptr != m_currentSubmode) [[likely]] {
        disconnect(m_currentSubmode, &GraphSubmodeBase::requestModeClear, this,
                   &GraphModeBase::clearCurrentMode);
    }

    clearCurrentMode();
    m_currentSubmode = pMode;

    auto& buttonConf = m_currentSubmode->getStarterButton();
    auto pButton = getScene()->getButtonMatrixHead()->getButton(
        buttonConf.positionX, buttonConf.positionY);
    pButton->setIcon(buttonConf.secondIcon);

    // Полный коннект
    connect(m_currentSubmode, &GraphSubmodeBase::requestModeClear, this,
            &GraphModeBase::clearCurrentMode);
}

GraphSubmodeBase* GraphModeBase::getCurrentSubmode() const {
    return m_currentSubmode;
}

void GraphModeBase::addSubmode(GraphSubmodeBase* pMode) {
    m_submodes.push_back(pMode);
    connect(pMode, &GraphSubmodeBase::requestModeStart, pMode,
            [this, pMode]() { setSubmode(pMode); });
}

std::list<GraphSubmodeBase*> GraphModeBase::getSubmodes() const {
    return m_submodes;
}

GraphSceneView* GraphModeBase::getScene() const {
    return m_pScene;
}

GraphModeContext& GraphModeBase::getContext() {
    return m_modeContext;
}

}  // namespace Graph
