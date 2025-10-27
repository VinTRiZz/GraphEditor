#ifndef GRAPHEDITVIEW_H
#define GRAPHEDITVIEW_H

#include <Components/CustomQt/ObjectView/InformationLayer.h>

#include <GraphObject/Maintainer.h>

namespace Graph {

class GraphEditView final : public OVLayers::OVInformationLayer {
    Q_OBJECT
public:
    GraphEditView(QWidget* parent = nullptr);

    void setGraphMaintaner(const Graph::PMaintainer& pGraphMaintaner);

private:
    PMaintainer m_pMaintainer {};
};

}  // namespace Graph

#endif  // GRAPHEDITVIEW_H
