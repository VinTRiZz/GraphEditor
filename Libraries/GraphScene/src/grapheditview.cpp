#include "grapheditview.h"

#include <Components/Logger/Logger.h>

#include <GraphItems/SimpleVertexItem.h>
#include <GraphItems/ImageVertexItem.h>

#include <Components/CustomQt/ObjectView/ObjectItems.h>

namespace Graph {

GraphEditView::GraphEditView(QWidget* parent) : OVInformationLayer(parent) {
    getCanvas()->setRect(QRectF(0, 0, 10000, 10000));
}

void GraphEditView::setGraphMaintaner(const PMaintainer& pGraphMaintaner) {

    // TODO: Connect graph

    m_pMaintainer = pGraphMaintaner;
}

}  // namespace Graph
