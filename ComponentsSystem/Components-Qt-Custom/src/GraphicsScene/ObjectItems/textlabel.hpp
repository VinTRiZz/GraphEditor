#pragma once

#include "abstracttext.hpp"

namespace ObjectItems {

class TextLabel : public AbstractText
{
public:
    explicit TextLabel(QGraphicsItem* parent = nullptr);

    void setContrastRectVisible(bool isCREctVisible = true);

private:
    QGraphicsRectItem* m_contrastRect {nullptr};
};

} // namespace ObjectItems

