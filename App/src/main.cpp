#include "mainwindow.h"
#include <QApplication>

#include <Common/Logging.h>
#include <Common/ApplicationSettings.h>

int main(int argc, char *argv[])
{
    LOG_INFO_SYNC("Started GraphEditor");
    QApplication a(argc, argv);

    auto& settingsInstance = ApplicationSettings::getInstance();
    settingsInstance.loadSettings();

    QFile stylesFile(":/common/styles/mainstyles.qss");
    if (stylesFile.open(QIODevice::ReadOnly)) {
        a.setStyleSheet(stylesFile.readAll());
        LOG_OK("Styles set");
    } else {
        LOG_ERROR("Error opening styles:", stylesFile.errorString());
    }

    MainWindow w;
    w.show();
    auto res = a.exec();
    settingsInstance.saveSettings();
    return res;
}
