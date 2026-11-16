#include "mainwindow.hpp"

#include <Components/Logger/Logger.h>

#include <QFile>

#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget* parent)
    : QMainWindow(parent), ui(new Ui::MainWindow) {
}

MainWindow::~MainWindow() {
    delete ui;
}
