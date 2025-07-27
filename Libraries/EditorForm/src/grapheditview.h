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

    void setGraphMaintaner(const std::shared_ptr<Graph::GraphMaintaner>& pGraphMaintaner) override;

    void startViewMode();
    void startEditMode();

    bool isEditMode() const;
    bool isViewMode() const;

signals:
    void startedView();
    void startedEdit();

private:
    GraphViewMode m_viewMode;
    GraphEditMode m_editMode;
};

}

#endif // GRAPHEDITVIEW_H
