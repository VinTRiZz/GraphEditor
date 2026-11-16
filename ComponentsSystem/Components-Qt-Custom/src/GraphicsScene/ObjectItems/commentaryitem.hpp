#pragma once

#include "groupitem.hpp"
#include "textlabel.hpp"

namespace ObjectItems {

class CommentItem : public GroupItem
{
    Q_OBJECT
public:
    explicit CommentItem(QGraphicsItem* parent = nullptr);

private:
    QGraphicsPathItem* m_referencerItem {nullptr};
    TextLabel* m_commentaryText {nullptr};
    bool m_isTextEditedByUser {false};
};

} // namespace ObjectItems

