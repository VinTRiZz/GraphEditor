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

    m_isModeStarted = true;
}

bool GraphEditMode::isRunning() const
{
    return m_isModeStarted;
}

void GraphEditMode::stop()
{
    // TODO: Reset button matrix

    m_isModeStarted = false;
}

}
