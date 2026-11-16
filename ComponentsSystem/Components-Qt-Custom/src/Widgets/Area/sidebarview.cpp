#include "sidebarview.hpp"

namespace QtCustom::Widgets {

SidebarTableView::SidebarTableView(QWidget* parent) :
    QTableView(parent),
    SidebarViewInterface(this)
{

}

SidebarTreeView::SidebarTreeView(QWidget* parent) :
    QTreeView(parent),
    SidebarViewInterface(this)
{

}

} // namespace QtCustom::Widgets
