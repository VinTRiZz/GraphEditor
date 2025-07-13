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

    void startViewMode();
    void startEditMode();

signals:
    void startedView();
    void startedEdit();

private:
    GraphViewMode m_viewMode;
    GraphEditMode m_editMode;
};

}

#endif // GRAPHEDITVIEW_H
