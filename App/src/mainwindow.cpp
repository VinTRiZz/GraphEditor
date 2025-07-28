#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QFile>
#include <Common/Logging.h>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    setWindowTitle(QString("Редактор графов (версия %0)").arg(GRAPH_EDITOR_VERSION));
    QIcon windowIcon(":/common/images/icons/app/grapheditor.svg");
    setWindowIcon(windowIcon);
}

MainWindow::~MainWindow()
{
    delete ui;
}

