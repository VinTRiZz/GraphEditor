#include <Components/Logger/Logger.h>
#include <Components/Common/ApplicationSettings.h>
#include <Components/Common/DirectoryManager.h>

#include <GraphObject/Common.h>
#include <PluginModule/PluginMaster.h>

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
    int res {0};

    initStacktrace();
    { // Нюанс работы (в деструкторах записываются кэши)
        QApplication a(argc, argv);
        initApplication(a);

        processArguments(a);
        initStylesheet(a);
        LOG_INFO("Starting GraphEditor");

        MainWindow w;
        w.show();
        res = a.exec();
    }

    Common::ApplicationSettings::getInstance().saveSettings();
    LOG_OK_SYNC("App exited normally");
    return res;
}

void initSettings() {
    auto& settingsInstance = Common::ApplicationSettings::getInstance();

    for (auto& [settingSection, settings] : Graph::SettingsNames::SETTING_SECTIONS) {
        for (auto& sett : settings) {
            if (!settingsInstance.hasSetting(settingSection, sett)) {
                settingsInstance.addSetting(settingSection, sett);
            }
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
    a.setWindowIcon(QIcon(":/images/icons/app/grapheditor.svg"));
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

    auto profileFilePath = parser.value(profileTypeOption);
    if (profileFilePath.isNull()) {
        profileFilePath = directoryManager.getDirectory(Common::DirectoryManager::DirectoryType::Config).absolutePath() + QDir::separator() + "default.ini";
    }

    LOG_INFO("Reading configs...");
    auto& settingsInstance = Common::ApplicationSettings::getInstance();
    settingsInstance.loadSettings(profileFilePath);
    initSettings();

    auto& pluginMaster = Graph::PluginMaster::getInstance();
    auto pluginDirPath = directoryManager.getDirectory(Common::DirectoryManager::DirectoryType::Plugins).absolutePath();
    pluginMaster.init(pluginDirPath.toStdString());
}


void initStylesheet(QApplication& a) {

    auto styleFiles = {
        ":/styles/common.qss",
        ":/styles/tabwidget.qss",
        ":/styles/toolbox.qss",
    };

    QString appStyles;
    for (auto& stylePath : styleFiles) {
        QFile stylesFile(stylePath);
        if (!stylesFile.open(QIODevice::ReadOnly)) {
            LOG_ERROR("Error opening styles file:", stylesFile.errorString());
            continue;
        }
        appStyles += "\n" + stylesFile.readAll();
    }

    a.setStyleSheet(appStyles);
}
