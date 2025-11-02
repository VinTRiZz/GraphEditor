#include "grapheditview.h"

#include <Components/Logger/Logger.h>

#include <GraphItems/SimpleVertexItem.h>
#include <GraphItems/ImageVertexItem.h>

namespace Graph {

GraphEditView::GraphEditView(QWidget* parent) : OVInformationLayer(parent) {
    getCanvas()->setRect(QRectF(0, 0, 10000, 10000));

    auto pVertex1 = new ImageVertexItem;
    pVertex1->setPos(100, 100);
    pVertex1->setTitlePosition(VertexTitlePosition::VTP_Bottom);
    pVertex1->setDisplayName("Test vertex");
    pVertex1->setImage(QImage("/home/vintrizz/Pictures/IMG_20230917_143226.jpg"));
    auto vrt1dbg = ObjectItems::DebugMaster(pVertex1);
    // vrt1dbg.debug_setBoundingRectVisible();
    // vrt1dbg.debug_setCustomRectVisible(toVertexBoundingRect(pVertex1->getVertexSizeType()));
    addItem(pVertex1);
}

void GraphEditView::setGraphMaintaner(const PMaintainer& pGraphMaintaner) {

    // TODO: Connect graph

    m_pMaintainer = pGraphMaintaner;
}

}  // namespace Graph
