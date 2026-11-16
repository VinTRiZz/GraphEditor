#pragma once

#include <QString>
#include <QSize>

#include <memory>

class QWidget;

namespace QtCustom::Widgets {

class SidebarViewInterface
{
public:
    explicit SidebarViewInterface(QWidget* self);
    ~SidebarViewInterface();

    enum class ShowDirection
    {
        SD_FromLeft,
        SD_FromRight,
    };
    void setSidebarsDirection(ShowDirection shDir);

    bool addSidebar(const QString& buttonTitle, const QSize& showSize, QWidget* swidget);
    QWidget* getSidebar(const QString& buttonTitle);
    void removeSidebar(const QString& buttonTitle);

private:
    QWidget* m_sidebarParentWidget {nullptr};
    ShowDirection m_showDir {ShowDirection::SD_FromLeft};

    struct Impl;
    std::unique_ptr<Impl> d;

    void reorderSidebars();
};

} // namespace QtCustom::Widgets
