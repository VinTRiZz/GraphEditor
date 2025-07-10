#include "grapheditview.h"

namespace Graph
{

GraphEditView::GraphEditView(QWidget *parent) :
    GraphEditor(parent),
    GraphViewer(parent)
{

}

void GraphEditView::init()
{
    GraphViewer::init();
    GraphEditor::init();
}

void GraphEditView::startMode()
{
    if (GraphViewer::isModeStarted()) {
        GraphViewer::stopMode();
        GraphEditor::startMode();
    } else {
        GraphEditor::stopMode();
        GraphViewer::startMode();
    }
}

bool GraphEditView::isModeStarted() const
{
    return (GraphViewer::isModeStarted() || GraphEditor::isModeStarted());
}

void GraphEditView::stopMode()
{
    GraphViewer::stopMode();
    GraphEditor::stopMode();
}

}
