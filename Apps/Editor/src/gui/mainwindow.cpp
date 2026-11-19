#include "mainwindow.hpp"
#include "ui_mainwindow.h"

#include <Components/Logger/Logger.h>
#include <Components/CustomQt/Widgets/Area/SidebarWidget.h>

MainWindow::MainWindow(QWidget* parent)
    : QMainWindow(parent), ui(new Ui::MainWindow) {    
    m_pSidebarFile = new QtCustom::Widgets::SidebarWidget(this);
    m_pSidebarItems = new QtCustom::Widgets::SidebarWidget(this);
    m_pSidebarSettings = new QtCustom::Widgets::SidebarWidget(this);
}

MainWindow::~MainWindow() {
    delete ui;
}
