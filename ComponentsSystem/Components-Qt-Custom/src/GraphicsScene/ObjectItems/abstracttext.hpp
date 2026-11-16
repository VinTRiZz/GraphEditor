#pragma once

#include "basicitem.hpp"

namespace ObjectItems {

class AbstractText : public BasicItem
{
public:
    explicit AbstractText(QGraphicsItem* parent = nullptr);

    void setFont(const QFont& f);
    QFont getFont() const;

    void setMaxSymbolCount(int maxSymbolCount);
    void setTextAlignment(int algn);
    void setTextSizePt(double textSizePt);

    void setEditableByUser(bool isEditableByUser);

private:
    QGraphicsTextItem* m_textItem {nullptr};
    bool m_isEditableByUser {false};
    bool m_isTextEditedByUser {false};

protected:
    QGraphicsTextItem* getTextItem() const;
};

} // namespace ObjectItems

