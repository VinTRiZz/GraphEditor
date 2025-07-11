#ifndef GRAPHEDITVIEW_H
#define GRAPHEDITVIEW_H

#include "graphscenebase.h"

#include "grapheditmode.h"
#include "graphviewmode.h"

namespace Graph
{

class GraphEditView final : public GraphSceneBase
{
    Q_OBJECT
public:
    GraphEditView(QWidget* parent = nullptr);

    void init() override;

    void setCurrentGraph(Graph::GraphExtendedObject* pGraph) override;

signals:
    void startedNoneMode();
    void startedViewMode();
    void startedEditMode();

private:
    GraphViewMode m_viewMode;
    GraphEditMode m_editMode;
};

}

#endif // GRAPHEDITVIEW_H
