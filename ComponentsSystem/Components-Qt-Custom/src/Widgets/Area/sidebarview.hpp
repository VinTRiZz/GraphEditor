#pragma once

#include <QTableView>
#include <QTreeView>

#include "sidebarviewinterface.hpp"

namespace QtCustom::Widgets {

class SidebarTableView : public QTableView,
                         public SidebarViewInterface
{
public:
    explicit SidebarTableView(QWidget* parent = nullptr);
};


class SidebarTreeView : public QTreeView,
                         public SidebarViewInterface
{
public:
    explicit SidebarTreeView(QWidget* parent = nullptr);
};

} // namespace QtCustom::Widgets
