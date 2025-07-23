#include "labelitem.h"

#include <QTextDocument>
#include <QTextOption>


namespace ObjectViewItems
{


LabelItem::LabelItem(QGraphicsItem* parent) :
    ItemBase(parent)
{
    m_vertexTextRect = new QGraphicsRectItem(this);

    m_vertexText    = new QGraphicsTextItem(this);
    m_vertexText->setDefaultTextColor(Qt::black);
    m_vertexText->setZValue(0);

    QFont font = m_vertexText->font();
    font.setPixelSize(14);
    font.setStyle(QFont::StyleItalic);
    m_vertexText->setFont(font);
    m_vertexText->setDefaultTextColor(Qt::black);

    QTextDocument* doc = m_vertexText->document();
    QTextOption option = doc->defaultTextOption();
    option.setAlignment(Qt::AlignCenter);
    doc->setDefaultTextOption(option);

    m_vertexText->setTextWidth(100);
}

void LabelItem::setName(const QString &iText)
{
    setToolTip(iText);
    ItemBase::setName(iText);
    m_vertexTextRect->setRect(m_vertexText->boundingRect());
}

void LabelItem::setShortName(const QString &iText)
{
    m_vertexText->setPlainText(iText);
    ItemBase::setShortName(iText);
    m_vertexTextRect->setRect(m_vertexText->boundingRect());
}

void LabelItem::setMainColor(const QColor &iColor)
{
    m_vertexText->setDefaultTextColor(iColor);
    ItemBase::setMainColor(iColor);
}

void LabelItem::setBackgroundColor(const QColor &iColor)
{
    m_vertexTextRect->setBrush(iColor);
    ItemBase::setBackgroundColor(iColor);
}

void LabelItem::setTextStyle(QFont::Style fStyle)
{

}

void LabelItem::setTextSize(int pixelSize)
{

}

QRectF LabelItem::boundingRect() const
{
    QRectF bRect = ItemBase::boundingRect();
    bRect.setHeight(std::max(bRect.height(), m_vertexTextRect->boundingRect().height()));
    bRect.setWidth(std::max(bRect.width(), m_vertexTextRect->boundingRect().width()));
    return bRect;
}

}
