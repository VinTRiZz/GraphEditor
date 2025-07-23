#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QFile>
#include <Common/Logging.h>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    QFile stylesFile(":/common/styles/mainstyles.qss");
    if (!stylesFile.open(QIODevice::ReadOnly)) {
        LOG_ERROR("Error opening styles:", stylesFile.errorString());
        return;
    }
    setStyleSheet(stylesFile.readAll());
    LOG_OK("Styles set");

    ui->graphEditorForm->init();

    setWindowTitle(QString("Редактор графов (версия %0)").arg(GRAPH_EDITOR_VERSION));
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::startValidanceTest()
{
    ui->graphEditorForm->startValidanceTest();
}

