#include "informationobjectview.h"

#include <Common/Logging.h>

#include <QMouseEvent>

InformationObjectView::InformationObjectView(QWidget* parent)
    : InteractiveObjectView(parent) {
    m_pCursorLabel = new ObjectViewItems::LabelItem();
    m_pCursorLabel->setSecondColor(QColor(200, 240, 210, 80));
    m_pCursorLabel->setMainColor(Qt::black);
    m_pCursorLabel->setTextSize(10);
    m_pCursorLabel->setFlag(QGraphicsItem::ItemIgnoresTransformations, true);
    m_pCursorLabel->setZValue(1e6);  // Чтобы точно не закрыло
    scene()->addItem(m_pCursorLabel);

    m_pInformationLabel = new QLabel(this);
    m_pInformationLabel->setWordWrap(true);
    m_pInformationLabel->setFixedSize(500, 23);

    // Настройка для отображения
    m_pInformationLabel->setStyleSheet(
        R"(
    color: black;
    font-size: 14px;
    background-color:
    rgba(200, 240, 210, 80);
    border: 1px solid black;
    border-top-left-radius: 6px;
    border-top-right-radius: 6px;
)");

    updateInformation();

    connect(this, &InteractiveObjectView::scaleChanged, this, [this]() {
        updateInformation();
        updateCursorLabel(mapFromGlobal(cursor().pos()));
    });

    connect(scene(), &ObjectsInternalScene::gridSetEnabled, this,
            &InformationObjectView::updateInformation);
}

void InformationObjectView::updateInformation() {
    auto isGridEnabled = scene()->getIsGridEnabled();
    auto gridSize = scene()->getGridSize();
    auto infoText = QString("Масштаб: 1:%0   Сетка: %1   Инструмент: %2")
                        .arg(QString::number(getCurrentScale(), 'f', 3),
                             isGridEnabled ? QString("%0px").arg(gridSize)
                                           : QString("Выкл."),
                             m_currentToolName.isEmpty() ? QString("Нет")
                                                         : m_currentToolName);

    m_pInformationLabel->setText(infoText);
}

void InformationObjectView::updateCursorLabel(const QPoint& currentPos) {
    auto currentPosMapped = mapToScene(currentPos);
    m_pCursorLabel->setPos(
        mapToScene({currentPos.x() + 20, currentPos.y() + 20}));

    QString hoverItemName{};
    auto hoverItem = itemAt(currentPos);
    if (nullptr != hoverItem) {
        auto pHoverItemParent = getParentOfComplex(hoverItem);
        if (nullptr != pHoverItemParent) {
            hoverItemName = pHoverItemParent->getSystemName();
        } else {
            hoverItemName =
                hoverItem->data(ObjectViewConstants::OBJECTFIELD_NAME_SYSTEM)
                    .toString();
        }
    }
    m_pCursorLabel->setShortName(QString("X: %0\nY: %1%2")
                                     .arg(QString::number(currentPosMapped.x()),
                                          QString::number(currentPosMapped.y()),
                                          !hoverItemName.isEmpty()
                                              ? QString("\n") + hoverItemName
                                              : QString()));
}

void InformationObjectView::setCurrentToolName(const QString& toolName) {
    m_currentToolName = toolName;
    updateInformation();
}

void InformationObjectView::mouseMoveEvent(QMouseEvent* e) {
    updateCursorLabel(e->pos());
    InteractiveObjectView::mouseMoveEvent(e);
}

void InformationObjectView::resizeEvent(QResizeEvent* e) {
    m_pInformationLabel->move(10, height() - m_pInformationLabel->height());
    InteractiveObjectView::resizeEvent(e);
}
