#include "mainwindow.h"
#include <QApplication>

#include "logging.h"

#include <QFileInfo>

int main(int argc, char *argv[])
{
    LOG_INFO_SYNC("Started GraphEditor");
    QApplication a(argc, argv);
    MainWindow w;
    w.show();
    return a.exec();
}
