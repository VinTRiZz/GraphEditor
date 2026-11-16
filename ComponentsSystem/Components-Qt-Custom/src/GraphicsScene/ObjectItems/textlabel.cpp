#include "textlabel.hpp"

#include <QPen>
#include <QBrush>

namespace ObjectItems {

TextLabel::TextLabel(QGraphicsItem* parent) :
    AbstractText(parent)
{
    setObjectType(ObjectType::OIT_TextLabel);

    createSubitem(m_contrastRect);

    connect(this, &BasicItem::displayNameChanged,
            this, [this](){
        m_contrastRect->setRect(getTextItem()->boundingRect());
    });

    connect(this, &BasicItem::graphicalDataChanged,
            this, [this](){
        m_contrastRect->setPen(getLinePen());
        m_contrastRect->setBrush(getBackgroundBrush());
    });
}

void TextLabel::setContrastRectVisible(bool isCREctVisible)
{
    m_contrastRect->setVisible(isCREctVisible);
}

} // namespace ObjectItems
