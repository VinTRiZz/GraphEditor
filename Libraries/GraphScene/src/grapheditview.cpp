#include "grapheditview.h"

#include <Components/Logger/Logger.h>

#include <GraphItems/SimpleVertexItem.h>
#include <GraphItems/ImageVertexItem.h>

#include <Components/CustomQt/ObjectView/ObjectItems.h>

namespace Graph {

GraphEditView::GraphEditView(QWidget* parent) : OVInformationLayer(parent) {
    getCanvas()->setRect(QRectF(0, 0, 10000, 10000));

//    auto pVertex = new ImageVertexItem;
//    pVertex->setPos(100, 100);
//    pVertex->setTitlePosition(VertexTitlePosition::VTP_Bottom);
//    pVertex->setDisplayName("Test vertex");
//    pVertex->setImage(QImage("/home/vintrizz/Pictures/IMG_20230917_143226.jpg"));

//    ObjectItems::DebugMaster::showBoundingRect(pVertex);
//    ObjectItems::DebugMaster::showCenter(pVertex);
//    addItem(pVertex);

    auto pTestLine = new ObjectItems::ArrowedConnectionLine;
    pTestLine->setDirection(ObjectItems::LineDirectionType::Bidirectional);
    pTestLine->setLine({QPointF(100, 130), QPointF(230, 156)});
    pTestLine->setHoverPen(QPen(Qt::red));
    pTestLine->setAcceptHoverEvents(true);
    addItem(pTestLine);

    auto pAnchor = new ObjectItems::AnchorItem;
    pAnchor->setPos(200, 200);
    pAnchor->setArrowDirections(ObjectItems::ArrowDirection::AI_AD_All);
    pAnchor->setArrowClickCallback([](ObjectItems::AnchorItem* pLine, ObjectItems::ArrowDirection direct){
        LOG_DEBUG("Clicked:", direct, reinterpret_cast<uint64_t>(pLine));
    });
//    ObjectItems::DebugMaster::showBoundingRect(pAnchor);
//    ObjectItems::DebugMaster::showCenter(pAnchor);
    addItem(pAnchor);
}

void GraphEditView::setGraphMaintaner(const PMaintainer& pGraphMaintaner) {

    // TODO: Connect graph

    m_pMaintainer = pGraphMaintaner;
}

}  // namespace Graph
