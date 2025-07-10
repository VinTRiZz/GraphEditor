#ifndef GRAPHEDITVIEW_H
#define GRAPHEDITVIEW_H

#include "grapheditor.h"
#include "graphviewer.h"

namespace Graph
{

class GraphEditView final : public GraphEditor, public GraphViewer
{
public:
    GraphEditView(QWidget* parent = nullptr);

    void init() override;

    void startMode() override;
    bool isModeStarted() const override;
    void stopMode() override;

private:

};

}

#endif // GRAPHEDITVIEW_H
