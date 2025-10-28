#include <Components/Common/ApplicationSettings.h>
#include <Components/Common/DirectoryManager.h>
#include <Components/Logger/Logger.h>

#include <PluginMaster/PluginMaster.h>

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

    QCommandLineOption profileTypeOption(QStringList() << "p" << "profile",
                                 "Profile to open with", "Profile",
                                 QString());
    parser.addOption(profileTypeOption);

    parser.process(a);

    // Инициализация директорий
    QString dirPath = parser.value(dirOption);
    if (dirPath.isNull()) {
        dirPath = "GraphEditor";
    }
    auto& inst = Common::DirectoryManager::getInstance();
    inst.setRootPath(dirPath);

    auto& pluginMaster = Graph::PluginMaster::getInstance();
    pluginMaster.init(inst.getDirectory(Common::DirectoryManager::DirectoryType::Plugins).absolutePath().toStdString());
    Logging::LoggingMaster::getInstance(inst.getDirectory(Common::DirectoryManager::DirectoryType::Logs).absolutePath().toStdString());

    QFile stylesFile(":/common/styles/mainstyles.qss");
    if (stylesFile.open(QIODevice::ReadOnly)) {
        a.setStyleSheet(stylesFile.readAll());
        LOG_OK("Styles set");
    } else {
        LOG_ERROR("Error opening styles:", stylesFile.errorString());
    }

    LOG_INFO_SYNC("Started GraphEditor");
    auto& settingsInstance = Common::ApplicationSettings::getInstance();

    auto profileFile = parser.value(profileTypeOption);
    settingsInstance.loadSettings(profileFile);

    LOG_INFO("Starting app...");
    MainWindow w;
    w.show();
    auto res = a.exec();
    LOG_OK("App exited normally");

    settingsInstance.saveSettings(profileFile);
    return res;
}
