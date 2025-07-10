#include "mainwindow.h"
#include "ui_mainwindow.h"

#include "Filework/savemaster.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::startValidanceTest()
{
    ui->objectScene->startValidanceTest();
    SaveMaster::testValidance();
}

