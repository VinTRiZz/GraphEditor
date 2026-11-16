#include "ovinformationlayer.hpp"

#include <QMouseEvent>

#include "ovinternalscene.hpp"
#include "ovconstants.hpp"

#include <Components/CustomQt/ObjectView/ObjectItems.h>

namespace OVLayers {

OVInformationLayer::OVInformationLayer(QWidget *parent) :
    OVCanvasLayer(parent),
    OVContextMenuLayer<OVInformationLayer>()
{
    m_pCursorLabel = new ObjectItems::TextLabel;
    m_pCursorLabel->setFlag(QGraphicsItem::ItemIgnoresTransformations);
    getScene()->addItem(m_pCursorLabel);
    m_pCursorLabel->setZValue(ItemLayers::CursorLabelLayer);
    m_pCursorLabel->setBackgroundBrush(Qt::white);

    m_pInformationLabel = new QLabel(this);
    m_pInformationLabel->setWordWrap(true);
    m_pInformationLabel->setFixedSize(500, 50);

    m_highlightItem = new QGraphicsPathItem(getCanvas());
    m_highlightItem->setPen(QColor(180, 180, 90));

    // Настройка для отображения
    m_pInformationLabel->setStyleSheet(
        R"(
    color: black;
    font-size: 14px;
    background-color: rgba(200, 240, 210, 80);
    border: 1px solid black;
    border-top-left-radius: 6px;
    border-top-right-radius: 6px;
)");

    setCursorValuesPresenter([this](const QPointF& curPoint) -> QString {
        auto pObject = getObject(mapFromScene(curPoint));
        QString objectType {};
        QString objectName {};
        if (pObject != nullptr) {
            objectType = pObject->getSystemName();
            objectName = pObject->getDisplayName();
            m_pCursorLabel->setMaxSymbolCount(250);
        } else {
            m_pCursorLabel->setMaxSymbolCount(100);
        }

        return QString("X: %0\nY: %1%2%3").arg(
                    QString::number(curPoint.x()),
                    QString::number(curPoint.y()),
                    objectType.isNull() ? QString() : "\nObject type: " + objectType,
                    objectName.isNull() ? QString() : "\nObject name: " + objectName
                    );
    });

    setInfoLabelPresenter([this]() -> QString {
        auto isGridEnabled = getScene()->getIsGridEnabled();
        auto gridSize = getScene()->getGridSize();

        return QString("Масштаб: 1:%0   Сетка: %1\nИнструмент: %2").arg(
                    QString::number(getCurrentScale(), 'f', 3),
                     isGridEnabled ? QString("%0px").arg(gridSize)
                                   : QString("Выкл."),
                     m_currentToolName.isEmpty() ? QString("Нет")
                                                 : m_currentToolName);
    });

    updateInformationLabel();
    connect(this, &OVCanvasLayer::scaleChanged,
            this, [this](){
        updateInformationLabel();
        updateCursorLabel();
        updateHighlight();
    });
    connect(getScene(), &OVInternalScene::gridEnabled,
            this, &OVInformationLayer::updateInformationLabel);
}

ObjectItems::TextLabel *OVInformationLayer::getCursorLabel() const
{
    return m_pCursorLabel;
}

QLabel *OVInformationLayer::getInformationLabel() const
{
    return m_pInformationLabel;
}

void OVInformationLayer::setCurrentToolname(const QString &toolName)
{
    m_currentToolName = toolName;
    updateInformationLabel();
}

void OVInformationLayer::setInfoLabelPresenter(const std::function<QString ()> &pres)
{
    m_infoLabelPresenter = pres;
    updateInformationLabel();
}

void OVInformationLayer::setCursorValuesPresenter(const std::function<QString (const QPointF &)> &pres)
{
    m_cursorValuesPresenter = pres;
}

void OVInformationLayer::setCursorLabelEnabled(bool isEn)
{
    m_isCursorLabelEnabled = isEn;
    updateCursorLabel();
}

void OVInformationLayer::setInformationLabelEnabled(bool isEn)
{
    m_isInfoLabelEnabled = isEn;
    updateInformationLabel();
}

void OVInformationLayer::setHighlightEnabled(bool isEn)
{
    m_isHighlightEnabled = isEn;
    updateHighlight();
}

void OVInformationLayer::updateCursorLabel()
{
    if (!m_isCursorLabelEnabled) {
        m_pCursorLabel->hide();
        return;
    }

    auto cursorPos = mapToScene(mapFromGlobal(QCursor::pos()));
    m_pCursorLabel->setPos(cursorPos + QPointF(15, 15) / getCurrentScale());
    m_pCursorLabel->setDisplayName(m_cursorValuesPresenter(cursorPos));
}

void OVInformationLayer::updateInformationLabel() {
    if (!m_isInfoLabelEnabled) {
        m_pInformationLabel->hide();
        return;
    }
    m_pInformationLabel->setText(m_infoLabelPresenter());
}

void OVInformationLayer::updateHighlight()
{
    if (!m_isHighlightEnabled) {
        m_highlightItem->hide();
        return;
    }

    auto cursorPos = mapFromGlobal(QCursor::pos());
    auto pObject = getObject(cursorPos);
    if (pObject != nullptr) {
        m_highlightItem->setZValue(pObject->zValue());
        QTransform scaleTr;
        scaleTr.scale(1.1, 1.1);
        m_highlightItem->setPath(scaleTr.map(pObject->shape()));

        auto targetPos = m_highlightItem->parentItem()->mapFromScene(pObject->scenePos());
        targetPos += (pObject->boundingRect().bottomRight() - m_highlightItem->boundingRect().bottomRight()) / 2.0;
        m_highlightItem->setPos(targetPos);
        m_highlightItem->show();
    } else {
        m_highlightItem->hide();
    }
}

void OVInformationLayer::mouseMoveEvent(QMouseEvent *e)
{
    OVCanvasLayer::mouseMoveEvent(e);
    updateCursorLabel();
    updateHighlight();
}

void OVInformationLayer::enterEvent(QEvent *e)
{
    OVCanvasLayer::enterEvent(e);
    m_pCursorLabel->show();
}

void OVInformationLayer::leaveEvent(QEvent *e)
{
    OVCanvasLayer::leaveEvent(e);
    m_pCursorLabel->hide();
}

void OVInformationLayer::resizeEvent(QResizeEvent *e) {
    OVCanvasLayer::resizeEvent(e);
    m_pInformationLabel->move(10, height() - m_pInformationLabel->height());
}

void OVInformationLayer::contextMenuEvent(QContextMenuEvent *e)
{
    executeContextMenu(e);
}

ObjectItems::TextLabel *OVInformationLayer::getCursorLabel()
{
    return m_pCursorLabel;
}

} // namespace OVLayers
