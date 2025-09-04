#include <Common/ApplicationSettings.h>
#include <Common/DirectoryManager.h>
#include <Common/Logging.h>

#include <QApplication>
#include <QCommandLineParser>
#include <QDateTime>

#include "mainwindow.h"

int main(int argc, char* argv[]) {
    QApplication a(argc, argv);

    a.setApplicationName("GraphEditor");
    a.setApplicationVersion(GRAPH_EDITOR_VERSION);

    QCommandLineParser parser;
    parser.setApplicationDescription(
        "Graph editor -- application for editing graphs, structuring data, "
        "etc.");
    parser.addHelpOption();
    parser.addVersionOption();

    QCommandLineOption dirOption(QStringList() << "d" << "dir",
                                 "Data directory of an app", "DataDir", QString());
    parser.addOption(dirOption);

    parser.process(a);

    QString dirPath = parser.value(dirOption);
    if (!dirPath.isEmpty()) {
        auto& directoryManager = DirectoryManager::getInstance(dirPath);
    }

    auto& settingsInstance = ApplicationSettings::getInstance();

    LOG_INFO_SYNC("Started GraphEditor");
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

    LOG_INFO("Starting app...");
    auto res = a.exec();
    LOG_OK("App exited normally");

    settingsInstance.saveSettings();
    return res;
}
