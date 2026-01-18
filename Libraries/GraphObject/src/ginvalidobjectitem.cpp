#include "ginvalidobjectitem.hpp"

namespace Graph
{

GInvalidObjectItem::GInvalidObjectItem(const QString &pluginName, const QString &objectName, QGraphicsItem* parent) :
    GObjectItem(parent)
{
    createSubitem(m_pInvalidRect);
    m_pInvalidRect->setBrush(QBrush(Qt::darkMagenta, Qt::DiagCrossPattern));
    m_pInvalidRect->setOpacity(0.6);

    createSubitem(m_pInvalidText);

    setPluginName(pluginName);
    setPluginObjectName(objectName);

    getLabel()->hide();

    m_pInvalidText->setText(QString("Not found:\n%0::\n%1").arg(getPluginName(), getPluginObjectName()));
    m_pInvalidText->setTransformOriginPoint(m_pInvalidText->boundingRect().center());
    m_pInvalidText->setRotation(-45);

    QFont f;
    f.setBold(true);
    m_pInvalidText->setFont(f);

    setSize(VertexSizeType::VST_Medium);
}

void GInvalidObjectItem::processSizeTypeChange(const QRectF &newSize)
{
    m_pInvalidRect->setRect(newSize);
    m_pInvalidText->setPos(newSize.center() - m_pInvalidText->boundingRect().center());
}

}
