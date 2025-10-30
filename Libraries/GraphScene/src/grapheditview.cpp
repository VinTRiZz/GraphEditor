#include "grapheditview.h"

#include <Components/Logger/Logger.h>

#include <GraphItems/SimpleVertexItem.h>

namespace Graph {

GraphEditView::GraphEditView(QWidget* parent) : OVInformationLayer(parent) {
    getCanvas()->setRect(QRectF(0, 0, 10000, 10000));

    auto pVertex1 = new SimpleVertexItem;
    pVertex1->setPos(100, 100);
    pVertex1->setTitlePosition(VertexTitlePosition::VTP_Bottom);
    pVertex1->setDisplayName("Bottom circle");
    pVertex1->setShapeType(SimpleVertexShapeType::SVST_Circle);
    pVertex1->setVertexSizeType(VertexSizeType::VST_UltraSmall);
    auto vrt1dbg = ObjectItems::DebugMaster(pVertex1);
    // vrt1dbg.debug_setBoundingRectVisible();
    // vrt1dbg.debug_setCustomRectVisible(toVertexBoundingRect(pVertex1->getVertexSizeType()));
    addItem(pVertex1);

    pVertex1 = new SimpleVertexItem;
    pVertex1->setPos(300, 100);
    pVertex1->setTitlePosition(VertexTitlePosition::VTP_Center);
    pVertex1->setDisplayName("Center ellipse");
    pVertex1->setShapeType(SimpleVertexShapeType::SVST_Ellipse);
    pVertex1->setVertexSizeType(VertexSizeType::VST_Small);
    addItem(pVertex1);
    auto vrt2dbg = ObjectItems::DebugMaster(pVertex1);
    // vrt2dbg.debug_setBoundingRectVisible();
    // vrt2dbg.debug_setCustomRectVisible(toVertexBoundingRect(pVertex1->getVertexSizeType()));

    pVertex1 = new SimpleVertexItem;
    pVertex1->setPos(500, 100);
    pVertex1->setTitlePosition(VertexTitlePosition::VTP_Top);
    pVertex1->setDisplayName("Top rect");
    pVertex1->setShapeType(SimpleVertexShapeType::SVST_Rect);
    // regular vertex
    addItem(pVertex1);
    auto vrt3dbg = ObjectItems::DebugMaster(pVertex1);
    // vrt3dbg.debug_setBoundingRectVisible();
    // vrt3dbg.debug_setCustomRectVisible(toVertexBoundingRect(pVertex1->getVertexSizeType()));

    pVertex1 = new SimpleVertexItem;
    pVertex1->setPos(100, 300);
    pVertex1->setTitlePosition(VertexTitlePosition::VTP_RightBottom);
    pVertex1->setDisplayName("RightBottom square");
    pVertex1->setShapeType(SimpleVertexShapeType::SVST_Square);
    pVertex1->setVertexSizeType(VertexSizeType::VST_Big);
    addItem(pVertex1);
    auto vrt4dbg = ObjectItems::DebugMaster(pVertex1);
    // vrt4dbg.debug_setBoundingRectVisible();
    // vrt4dbg.debug_setCustomRectVisible(toVertexBoundingRect(pVertex1->getVertexSizeType()));

    pVertex1 = new SimpleVertexItem;
    pVertex1->setPos(400, 300);
    pVertex1->setTitlePosition(VertexTitlePosition::VTP_Bottom);
    pVertex1->setDisplayName("Bottom parallelogram");
    pVertex1->setShapeType(SimpleVertexShapeType::SVST_Parallelogram);
    pVertex1->setVertexSizeType(VertexSizeType::VST_UltraSmall);
    addItem(pVertex1);
    auto vrt5dbg = ObjectItems::DebugMaster(pVertex1);
    // vrt5dbg.debug_setBoundingRectVisible();
    // vrt5dbg.debug_setCustomRectVisible(toVertexBoundingRect(pVertex1->getVertexSizeType()));
}

void GraphEditView::setGraphMaintaner(const PMaintainer& pGraphMaintaner) {

    // TODO: Connect graph

    m_pMaintainer = pGraphMaintaner;
}

}  // namespace Graph
