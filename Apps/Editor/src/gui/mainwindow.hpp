#pragma once

#include <QMainWindow>

QT_BEGIN_NAMESPACE
namespace Ui {
class MainWindow;
}
QT_END_NAMESPACE

namespace QtCustom::Widgets {
class SidebarWidget;
}

/**
 * @brief The MainWindow class Главное окно приложения
 */
class MainWindow : public QMainWindow {
    Q_OBJECT

public:
    explicit MainWindow(QWidget* parent = nullptr);
    ~MainWindow();

private:
    Ui::MainWindow* ui;

    QtCustom::Widgets::SidebarWidget* m_pSidebarSettings {};
    QtCustom::Widgets::SidebarWidget* m_pSidebarFile {};
    QtCustom::Widgets::SidebarWidget* m_pSidebarItems {};
};
