#include "grapheditor.h"

namespace Graph
{

GraphEditor::GraphEditor(QWidget *parent) :
    GraphViewBase(parent)
{

}

void GraphEditor::init()
{
    GraphViewBase::init();
}

void GraphEditor::startMode()
{
    m_isModeStarted = true;
}

bool GraphEditor::isModeStarted() const
{
    return m_isModeStarted;
}

void GraphEditor::stopMode()
{
    m_isModeStarted = false;
}

}
