#include <Common/ApplicationSettings.h>
#include <Common/DirectoryManager.h>
#include <Common/Logging.h>

#include <QApplication>
#include <QCommandLineParser>

#include <QDir>

#include "mainwindow.h"

int main(int argc, char* argv[]) {
    QApplication a(argc, argv);

    qDebug() << "STARTED WITH ARGS:";
    qDebug() << "===============";
    for (int i = 1; i < argc; ++i) {
        qDebug() << i << ":" << argv[i];
    }
    qDebug() << "===============";

    // Независимые настройки
    a.setApplicationName("GraphEditor");
    a.setApplicationVersion(GRAPH_EDITOR_VERSION);

    QCommandLineParser parser;
    parser.setApplicationDescription(
        "Graph editor -- application for editing graphs, structuring data, "
        "etc.");
    parser.addHelpOption();
    parser.addVersionOption();

    QCommandLineOption dirOption(QStringList() << "d" << "dir",
                                 "Data directory of an app", "DataDir",
                                 QString());
    parser.addOption(dirOption);

    parser.process(a);

    // Инициализация директорий
    QString dirPath = parser.value(dirOption);
    if (dirPath.isNull()) {
        dirPath = "GraphEditor";
    }
    auto& inst = DirectoryManager::getInstance(dirPath);

    QFile stylesFile(":/common/styles/mainstyles.qss");
    if (stylesFile.open(QIODevice::ReadOnly)) {
        a.setStyleSheet(stylesFile.readAll());
        LOG_OK("Styles set");
    } else {
        LOG_ERROR("Error opening styles:", stylesFile.errorString());
    }

    Logging::LoggingMaster::getInstance().clearExtraLogs();
    LOG_INFO_SYNC("Started GraphEditor");

    auto& settingsInstance = ApplicationSettings::getInstance();
    settingsInstance.loadSettings();

    LOG_INFO("Starting app...");
    MainWindow w;
    w.show();
    auto res = a.exec();
    LOG_OK("App exited normally");

    settingsInstance.saveSettings();
    return res;
}
