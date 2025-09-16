#ifndef GRAPHEDITVIEW_H
#define GRAPHEDITVIEW_H

#include "graphsceneview.h"

namespace Graph {

class GraphViewMode;
class GraphEditMode;

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
    GraphViewMode* m_viewMode {nullptr};
    GraphEditMode* m_editMode {nullptr};
};

}  // namespace Graph

#endif  // GRAPHEDITVIEW_H
