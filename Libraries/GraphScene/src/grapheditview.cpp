#include "grapheditview.h"

#include <Components/Logger/Logger.h>

namespace Graph {

GraphEditView::GraphEditView(QWidget* parent) : OVInformationLayer(parent) {

}

void GraphEditView::setGraphMaintaner(const PMaintainer& pGraphMaintaner) {

    // TODO: Connect graph

    m_pMaintainer = pGraphMaintaner;
}

}  // namespace Graph
