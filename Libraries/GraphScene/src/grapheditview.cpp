#include "grapheditview.h"

#include <Components/Logger/Logger.h>

#include <GraphItems/SimpleVertexItem.h>
#include <GraphItems/ImageVertexItem.h>

#include <Components/CustomQt/ObjectView/ObjectItems.h>

namespace Graph {

GraphEditView::GraphEditView(QWidget* parent) : OVInformationLayer(parent) {
    getCanvas()->setRect(QRectF(0, 0, 10000, 10000));

    auto pVertex = new ImageVertexItem(getCanvas());
    pVertex->setPos(800, 600);
    pVertex->setTitlePosition(VertexTitlePosition::VTP_Bottom);
    pVertex->setDisplayName("Test vertex");
    pVertex->setImage(QImage("/home/vintrizz/Pictures/IMG_20230917_143226.jpg"));

    auto pTestLine = new ObjectItems::ElegantConnectionLine(getCanvas());
    pTestLine->setDirection(ObjectItems::LineDirectionType::Bidirectional);
    pTestLine->setLine({QPointF(600, 530), QPointF(530, 456)});
    pTestLine->setHoverPen(QPen(Qt::red));
    pTestLine->setAcceptHoverEvents(true);

    auto pAnchor = new ObjectItems::AnchorItem(getCanvas());
    pAnchor->setZValue(OVLayers::ItemLayers::AnchorItemLayer);
    pAnchor->setPos(200, 200);
    pAnchor->setArrowDirections(ObjectItems::ArrowDirection::AI_AD_All);
    pAnchor->setArrowClickCallback([this, pAnchor](ObjectItems::AnchorItem* pLine, ObjectItems::ArrowDirection direct){
        auto pTestLine = new ObjectItems::ArrowedConnectionLine(getCanvas());
        pTestLine->setDirection(ObjectItems::LineDirectionType::Bidirectional);
        pTestLine->setHoverPen(QPen(Qt::red));
        pTestLine->setAcceptHoverEvents(true);
        pTestLine->show();

        pAnchor->connectToAnchor(pTestLine, true);

        connectGrabLine(pTestLine);
    });

    bool isMoving = false;
    connect(pAnchor, &ObjectItems::BasicItem::itemClicked,
            this, [this, isMoving, pAnchor]() mutable {
        isMoving = !isMoving;
        if (isMoving) {
            connectGrabItem(pAnchor);
        } else {
            disconnectGrabItem(pAnchor);
        }
    });

    auto pAnchor2 = new ObjectItems::AnchorItem(getCanvas());
    pAnchor2->setZValue(OVLayers::ItemLayers::AnchorItemLayer);
    pAnchor2->setPos(530, 600);
    connect(pAnchor2, &ObjectItems::BasicItem::itemClicked,
            this, [this, isMoving, pAnchor2]() mutable {
        auto lines = getGrabbedLines();
        if (!lines.empty()) {
            auto pLine = lines.front();
            disconnectGrabLine(pLine);
            pAnchor2->connectToAnchor(pLine, false);
            return;
        }

        isMoving = !isMoving;
        if (isMoving) {
            connectGrabItem(pAnchor2);
        } else {
            disconnectGrabItem(pAnchor2);
        }
    });

//    ObjectItems::DebugMaster::showBoundingRect(pAnchor);
//    ObjectItems::DebugMaster::showCenter(pAnchor);
}

void GraphEditView::setGraphMaintaner(const PMaintainer& pGraphMaintaner) {

    // TODO: Connect graph

    m_pMaintainer = pGraphMaintaner;
}

}  // namespace Graph
