#include "graphviewer.h"

namespace Graph
{

GraphViewer::GraphViewer(QWidget *parent) :
    GraphViewBase(parent)
{
    
}

void GraphViewer::init()
{
    GraphViewBase::init();
}

void GraphViewer::startMode()
{
    m_isModeStarted = true;
}

bool GraphViewer::isModeStarted() const
{
    return m_isModeStarted;
}

void GraphViewer::stopMode()
{
    m_isModeStarted = false;
}

}
