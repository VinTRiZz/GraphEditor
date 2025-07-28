#include "mainwindow.h"
#include <QApplication>

#include <Common/Logging.h>

int main(int argc, char *argv[])
{
    LOG_INFO_SYNC("Started GraphEditor");
    QApplication a(argc, argv);
    MainWindow w;
    w.show();
    return a.exec();
}
