#include "sidebarviewinterface.hpp"

#include <Components/CustomQt/Widgets/Area/SidebarWidget.h>

#include <map>

namespace QtCustom::Widgets {

struct SidebarViewInterface::Impl
{
    std::map<QString, SidebarWidget*> sidebars;
};

SidebarViewInterface::SidebarViewInterface(QWidget* self) :
    m_sidebarParentWidget {self},
    d {new Impl}
{

}

SidebarViewInterface::~SidebarViewInterface()
{

}

void SidebarViewInterface::setSidebarsDirection(ShowDirection shDir)
{
    m_showDir = shDir;

    auto shDirEqu = (m_showDir == ShowDirection::SD_FromLeft) ?
        QtCustom::Widgets::SidebarWidget::Direction::Right :
        QtCustom::Widgets::SidebarWidget::Direction::Left;
    for (auto& [_, sidebar] : d->sidebars) {
        sidebar->setShowTowardsDirection(shDirEqu);
    }
}

bool SidebarViewInterface::addSidebar(const QString &buttonTitle, const QSize &showSize, QWidget *swidget)
{
    if (d->sidebars.count(buttonTitle)) {
        return false;
    }

    auto& sidebar = d->sidebars[buttonTitle];
    sidebar = new SidebarWidget(m_sidebarParentWidget);
    sidebar->getButton()->setText(buttonTitle);
    sidebar->setWidgetHeight(showSize.height());
    sidebar->setWidgetWidth(showSize.width());
    sidebar->setButtonPosition(QPoint(10, 10));
    sidebar->getButton()->setFixedSize(100, 30);

    auto shDirEqu = (m_showDir == ShowDirection::SD_FromLeft) ?
                        QtCustom::Widgets::SidebarWidget::Direction::Right :
                        QtCustom::Widgets::SidebarWidget::Direction::Left;
    sidebar->setShowTowardsDirection(shDirEqu);

    sidebar->setToggleCallback([this, buttonTitle, callerSidebar = sidebar](auto pButton, auto currentState){
        if (currentState == QtCustom::Widgets::SidebarWidget::HS_Shown) {
            for (auto& [_, sidebar] : d->sidebars) {
                if (sidebar == callerSidebar) {
                    continue;
                }
                sidebar->getButton()->hide();
            }
            pButton->setText("Hide");
        } else {
            pButton->setText(buttonTitle);
            for (auto& [_, sidebar] : d->sidebars) {
                if (sidebar == callerSidebar) {
                    continue;
                }
                sidebar->getButton()->show();
            }
        }
    });

    sidebar->setWidget(swidget);
    sidebar->hideTargetWidget();


    reorderSidebars();
    return true;
}

QWidget *SidebarViewInterface::getSidebar(const QString &buttonTitle)
{
    if (d->sidebars.count(buttonTitle)) {
        return d->sidebars[buttonTitle];
    }
    return {};
}

void SidebarViewInterface::removeSidebar(const QString &buttonTitle)
{
    if (!d->sidebars.count(buttonTitle)) {
        return;
    }
    auto& sidebar = d->sidebars[buttonTitle];
    sidebar->hide();
    sidebar->deleteLater();
    d->sidebars.erase(buttonTitle);
    reorderSidebars();
}

void SidebarViewInterface::reorderSidebars()
{
    // Reorder sidebars
    constexpr auto deltaOffset {5};
    QPoint currentStarts {10, deltaOffset};
    for (auto& [_, pSidebar] : d->sidebars) {
        auto pButton = pSidebar->getButton();
        pSidebar->setButtonPosition(currentStarts);
        currentStarts.setY(currentStarts.y() + pButton->height() + deltaOffset);
    }
}

} // namespace QtCustom::Widgets
