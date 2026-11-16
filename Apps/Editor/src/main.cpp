#include <Components/Logger/Logger.h>
#include <Components/Ecosystem/DirectoryManager.h>
#include <Components/Ecosystem/ApplicationSettings.h>

#include "gui/mainwindow.hpp"
#include <QApplication>

#include <QFile> // for styles

int main(int argc, char* argv[]) {
    int res = -1;
    auto& appSettings = Common::ApplicationSettings::getInstance();
    {
        QApplication a(argc, argv);

        auto& dirManager = Common::DirectoryManager::getInstance();
        dirManager.setRootPath(PROJECT_NAME_STRING);

        COMPLOG_SET_LOGSDIR(dirManager.getDirectory(Common::Logs));

        appSettings.loadSettings(dirManager.getDirectory(Common::Config) / "default.ini");

        a.setApplicationName(PROJECT_NAME_STRING);
        a.setApplicationDisplayName(QString("%1 (version %2)").arg(PROJECT_NAME_STRING, PROJECT_VERSION_STRING));
        a.setApplicationVersion(PROJECT_VERSION_STRING);

        QStringList styleResources {
            ":/styles/common.qss",
            ":/styles/buttons.qss",
            ":/styles/editors.qss",
            ":/styles/area.qss",
            ":/styles/views.qss",
            ":/styles/menus.qss"
        };
        QString resultStyle;
        for (auto& style : styleResources) {
            QFile resFile(style);
            resFile.open(QIODevice::ReadOnly);
            if (!resFile.isOpen()) {
                COMPLOG_SYNC_WARNING("Failed to load style:", style.toStdString());
                continue;
            }
            resultStyle += resFile.readAll() + '\n';
        }
        a.setStyleSheet(resultStyle);

        MainWindow w;
        w.show();
        res = a.exec();
    }

    appSettings.saveSettings();
    COMPLOG_INFO("Manage panel exited");
    return res;
}
