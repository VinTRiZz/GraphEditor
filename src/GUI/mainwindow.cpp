#include "mainwindow.h"
#include "ui_mainwindow.h"

#include "Filework/savemaster.h"

#include <QFile>
#include "logging.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    QFile stylesFile("://DATA/styles/mainstyles.qss");
    if (!stylesFile.open(QIODevice::ReadOnly)) {
        LOG_ERROR("Error opening styles:", stylesFile.errorString());
        return;
    }
    setStyleSheet(stylesFile.readAll());
    LOG_OK("Styles set");

    ui->graphEditorForm->init();
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::startValidanceTest()
{
    ui->graphEditorForm->startValidanceTest();
}

