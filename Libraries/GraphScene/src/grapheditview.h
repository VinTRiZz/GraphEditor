#ifndef GRAPHEDITVIEW_H
#define GRAPHEDITVIEW_H

#include "grapheditmode.h"
#include "graphsceneview.h"
#include "graphviewmode.h"

namespace Graph {

class GraphEditView final : public GraphSceneView {
    Q_OBJECT
public:
    GraphEditView(QWidget* parent = nullptr);

    void setGraphMaintaner(const Graph::PMaintainer& pGraphMaintaner) override;

    void startViewMode();
    void startEditMode();

    bool isEditMode() const;
    bool isViewMode() const;

signals:
    void startedView();
    void startedEdit();

    void openPropertyEditor(QGraphicsItem* pTargetItem);
    void closePropertyEditor();

private:
    GraphViewMode m_viewMode;
    GraphEditMode m_editMode;
};

}  // namespace Graph

#endif  // GRAPHEDITVIEW_H
