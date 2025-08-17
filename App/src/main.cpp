#include <Common/ApplicationSettings.h>
#include <Common/Logging.h>

#include <QApplication>
#include <QDateTime>

#include "mainwindow.h"

void removeExtraLogs() {
    auto logdir = Logging::LoggingMaster::getLogsDir();
    auto logfiles = logdir.entryList();
    logfiles.removeOne(".");
    logfiles.removeOne("..");  // Игнорируем текущую и директорию выше (кьют
                               // багованный в этом плане)

    auto maxLogCount = ApplicationSettings::getInstance().getGeneralConfig().getMaxLogFileCount();
    if (logfiles.size() > maxLogCount) {
        LOG_INFO("Removing logfiles");
        auto currentLogfile =
            QFileInfo(Logging::LoggingMaster::getCurrentLogfile()).baseName();
        std::sort(logfiles.begin(), logfiles.end(),
                  [](const QString& logFilePathLeft,
                     const QString& logFilePathRight) {
                      // TODO: Вынести в отдельную функцию из логгера
                      // Текущий формат: 20-59-44_03-07-2025
                      auto leftBasenameDate = QDateTime::fromString(
                          QFileInfo(logFilePathLeft).baseName(), "");
                      auto rightBasenameDate = QDateTime::fromString(
                          QFileInfo(logFilePathRight).baseName(), "");
                      return (leftBasenameDate < rightBasenameDate);
                  });
        std::reverse(logfiles.begin(), logfiles.end());
        logfiles.erase(
            logfiles.begin(),
            logfiles.begin() + maxLogCount);  // Игнорируем первые N файлов

        unsigned removedLogFileCount{0};
        for (auto& logfile : logfiles) {
            if (removedLogFileCount > maxLogCount) {
                break;
            }
            if (logfile != currentLogfile) {
                QFile::remove(logdir.absoluteFilePath(logfile));
                removedLogFileCount++;
            }
        }
        LOG_OK("Removed", removedLogFileCount, "logfiles");
    }
}

int main(int argc, char* argv[]) {
    QApplication a(argc, argv);
    auto& settingsInstance = ApplicationSettings::getInstance();

    removeExtraLogs();
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
