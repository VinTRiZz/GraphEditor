#include <Components/Logger/Logger.h>
#include <Components/Common/ApplicationSettings.h>
#include <Components/Common/DirectoryManager.h>
#include <Components/Common/FileImportManager.h>
#include <Components/Encryption/Hash.h>

#include <PluginMaster/PluginMaster.h>

#include <QApplication>
#include <QCommandLineParser>

#include <csignal>
#include <signal.h>
#include <boost/stacktrace.hpp>

#include <filesystem>

#include <QDir>

#include "mainwindow.h"

void initSettings();
void initStacktrace();
void initApplication(QApplication& a);
void processArguments(QApplication& a);
void initStylesheet(QApplication& a);

void printStacktrace(int signumber);

int main(int argc, char* argv[]) {
    initStacktrace();

    QApplication a(argc, argv);
    initApplication(a);

    processArguments(a);
    initStylesheet(a);

    LOG_INFO("Starting GraphEditor");
    MainWindow w;
    w.show();
    auto res = a.exec();

    Common::ApplicationSettings::getInstance().saveSettings();
    LOG_OK_SYNC("App exited normally");
    return res;
}

void initSettings() {
    auto& settingsInstance = Common::ApplicationSettings::getInstance();

    const std::vector<QString> SYS_SETTINGS {
        "canvas_size",
    };

    for (auto& sett : SYS_SETTINGS) {
        if (!settingsInstance.hasSetting("SYSTEM", sett)) {
            settingsInstance.addSetting("SYSTEM", sett);
        }
    }
}

void initStacktrace() {
    ::signal(SIGSEGV, &printStacktrace);
    ::signal(SIGABRT, &printStacktrace);
    ::signal(SIGTERM, &printStacktrace);
    ::signal(SIGFPE, &printStacktrace);
}

void printStacktrace(int signumber) {
    LOG_ERROR("SIGNAL:", signumber, "(", strsignal(signumber), ")");
    ::signal(signumber, SIG_DFL);

    LOG_EMPTY("STACK TRACE:");
    LOG_EMPTY("====================================================");
    boost::stacktrace::stacktrace stackTrace;
    int traceLayer {0};
    for (auto& line : stackTrace) {
        LOG_EMPTY(traceLayer++, to_string(line));
    }

    LOG_EMPTY("====================================================");
    ::exit(-1);
}

void initApplication(QApplication& a) {
    a.setApplicationName("GraphEditor");
    a.setApplicationVersion(GRAPH_EDITOR_VERSION);
    a.setApplicationDisplayName(QString("Редактор графов (версия %0)").arg(GRAPH_EDITOR_VERSION));
    a.setWindowIcon(QIcon(":/common/images/icons/app/grapheditor.svg"));
}

void processArguments(QApplication& a) {
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
    auto& directoryManager = Common::DirectoryManager::getInstance();
    directoryManager.setRootPath(dirPath);

    auto logsFileDirPath = directoryManager.getDirectory(Common::DirectoryManager::DirectoryType::Logs).absolutePath();
    Logging::LoggingMaster::getInstance(logsFileDirPath.toStdString());

    auto cacheDirPath = directoryManager.getDirectory(Common::DirectoryManager::DirectoryType::Temporary).absolutePath();
    auto& fiManager = Common::FileImportManager::getInstance();
    fiManager.setHashCalculator(&Encryption::sha256);
    fiManager.setRootDirectory(cacheDirPath.toStdString());
    fiManager.updateCache();

    auto& pluginMaster = Graph::PluginMaster::getInstance();
    auto pluginDirPath = directoryManager.getDirectory(Common::DirectoryManager::DirectoryType::Plugins).absolutePath();
    pluginMaster.init(pluginDirPath.toStdString());

    auto profileFilePath = parser.value(profileTypeOption);
    if (profileFilePath.isNull()) {
        profileFilePath = directoryManager.getDirectory(Common::DirectoryManager::DirectoryType::Config).absolutePath() + QDir::separator() + "default.ini";
    }

    LOG_INFO("Reading configs...");
    auto& settingsInstance = Common::ApplicationSettings::getInstance();
    settingsInstance.loadSettings(profileFilePath);
    initSettings();
}


void initStylesheet(QApplication& a) {
    QFile stylesFile(":/common/styles/mainstyles.qss");
    if (stylesFile.open(QIODevice::ReadOnly)) {
        a.setStyleSheet(stylesFile.readAll());
        LOG_OK("Styles set");
    } else {
        LOG_ERROR("Error opening styles:", stylesFile.errorString());
    }
}
