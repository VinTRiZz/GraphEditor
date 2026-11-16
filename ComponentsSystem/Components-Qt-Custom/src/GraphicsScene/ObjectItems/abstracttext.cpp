#include "abstracttext.hpp"

#include <QFont>

#include <QTextOption>
#include <QTextDocument>

#include <Components/Logger/Logger.h>

namespace ObjectItems {

AbstractText::AbstractText(QGraphicsItem* parent) :
    BasicItem(parent)
{
    setSystemName("Text");
    setObjectType(ObjectType::OIT_AbstractText);

    createSubitem(m_textItem);
    m_textItem->setZValue(100); // Чтобы не перекрывали в будущем
    m_textItem->setFlag(ItemIsFocusable);
    m_textItem->setFlag(ItemAcceptsInputMethod);

    connect(this, &BasicItem::displayNameChanged,
            this, [this](){
        if (m_isTextEditedByUser) {
            return;
        }
        m_textItem->setPlainText(getDisplayName());
    });

    connect(this, &BasicItem::graphicalDataChanged,
            this, [this](){
        m_textItem->setDefaultTextColor(getLinePen().color());
    });

    // Шрифты
    auto font = m_textItem->font();
    font.setPixelSize(14);
    font.setStyle(QFont::StyleItalic);
    setFont(font);
    setTextAlignment(Qt::AlignCenter);

    setLinePen({Qt::black});
    m_textItem->setTextWidth(100);

    setEditableByUser(false);
}

void AbstractText::setFont(const QFont &f)
{
    m_textItem->setFont(f);
}

QFont AbstractText::getFont() const
{
    return m_textItem->font();
}

void AbstractText::setMaxSymbolCount(int maxSymbolCount)
{
    m_textItem->setTextWidth(maxSymbolCount);
}

void AbstractText::setTextAlignment(int algn)
{
    auto doc = m_textItem->document();
    auto option = doc->defaultTextOption();
    option.setAlignment(Qt::AlignmentFlag(algn));
    doc->setDefaultTextOption(option);
}

void AbstractText::setTextSizePt(double textSizePt)
{
    auto font = getFont();
    font.setPointSizeF(textSizePt);
    setFont(font);
    emit displayNameChanged();
}

void AbstractText::setEditableByUser(bool isEditableByUser)
{
    m_isEditableByUser = isEditableByUser;
    if (m_isEditableByUser) {
        m_textItem->setTextInteractionFlags(Qt::TextEditorInteraction);
        connect(m_textItem->document(), &QTextDocument::contentsChanged,
                this, [this](){
            m_isTextEditedByUser = true;
            setDisplayName(m_textItem->toPlainText());
            emit displayNameChanged();
            m_isTextEditedByUser = false;
        });
    } else {
        disconnect(m_textItem->document(), nullptr, this, nullptr);
        m_textItem->setTextInteractionFlags(Qt::TextBrowserInteraction);
    }
}

QGraphicsTextItem *AbstractText::getTextItem() const
{
    return m_textItem;
}

} // namespace ObjectItems
