#ifndef LABELITEM_H
#define LABELITEM_H

#include "itembase.h"

#include <QFont>

namespace ObjectViewItems
{

class LabelItem : public ItemBase
{
public:
    explicit LabelItem(QGraphicsItem* parent = nullptr);

    void setName(const QString& iText) override;
    void setShortName(const QString& iText) override;

    void setMainColor(const QColor& iColor) override;
    void setBackgroundColor(const QColor& iColor) override;

    void setTextStyle(QFont::Style fStyle);
    void setTextSize(int pixelSize);

private:
    QGraphicsRectItem*  m_vertexTextRect    {nullptr};
    QGraphicsTextItem*  m_vertexText        {nullptr};
};

}

#endif // LABELITEM_H
