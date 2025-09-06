#include "logging.h"

#include <QDateTime>

#include "applicationsettings.h"
#include "directorymanager.h"

namespace Logging {

void LoggingMaster::waitForTasks() {
    std::unique_lock<std::mutex> lock(addTaskMx);
    addTaskCV.wait(lock);
}

void LoggingMaster::taskAdded() {
    std::unique_lock<std::mutex> lock(addTaskMx);
    addTaskCV.notify_one();
}

std::string LoggingMaster::getCurrentTimestamp() const {
    // System time value
    auto timeValue = std::time(nullptr);
    auto locTime = std::localtime(&timeValue);

    auto formattedNumber = [](uint8_t iNum) -> std::string {
        return (iNum > 9 ? std::to_string(iNum)
                         : std::string("0") + std::to_string(iNum));
    };

    // TODO: Вынести в отдельную функцию из логгера для инкапсуляции формата
    return {formattedNumber(locTime->tm_mday) + "." +
            formattedNumber(locTime->tm_mon) + ".20" +
            formattedNumber(locTime->tm_year - 100) + " " +
            formattedNumber(locTime->tm_hour) + ":" +
            formattedNumber(locTime->tm_min) + ":" +
            formattedNumber(locTime->tm_sec)};
}

std::string LoggingMaster::getCurrentTimestampFormatted() const {
    // System time value
    auto timeValue = std::time(nullptr);
    auto locTime = std::localtime(&timeValue);

    auto formattedNumber = [](uint8_t iNum) -> std::string {
        return (iNum > 9 ? std::to_string(iNum)
                         : std::string("0") + std::to_string(iNum));
    };

    return {formattedNumber(locTime->tm_hour) + "-" +
            formattedNumber(locTime->tm_min) + "-" +
            formattedNumber(locTime->tm_sec) + "_" +
            formattedNumber(locTime->tm_mday) + "-" +
            formattedNumber(locTime->tm_mon) + "-20" +
            formattedNumber(locTime->tm_year - 100)};
}

void LoggingMaster::clearExtraLogs() {
    auto logdir =
        DirectoryManager::getSystemDirectory(DirectoryManager::DirectoryTypeSystem::Logs);
    auto logfiles = logdir.entryList();
    logfiles.removeOne(".");
    logfiles.removeOne("..");  // Игнорируем текущую и директорию выше (кьют
    // багованный в этом плане)

    auto maxLogCount = ApplicationSettings::getInstance()
                           .getGeneralConfig()
                           .getMaxLogFileCount();
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

LoggingMaster::LoggingMaster() {
    logfile.setFileName(DirectoryManager::getSystemDirectoryPath(DirectoryManager::DirectoryTypeSystem::Logs) +
                        getCurrentTimestampFormatted().c_str() + ".log");

    isWorking = true;
    logThread = std::thread([this]() {
        isThreadExited = false;
        while (isWorking) {
            waitForTasks();

            taskListMx.lock();
            while (!taskList.empty()) {
                auto nextTask = taskList.front();
                taskList.pop_front();
                taskListMx.unlock();

                nextTask();
                taskListMx.lock();
            }
            taskListMx.unlock();
        }
        isThreadExited = true;
    });

    clearExtraLogs();
}

LoggingMaster::~LoggingMaster() {
    isWorking = false;
    while (!isThreadExited) {
        taskAdded();
    }
    if (logThread.joinable()) {
        logThread.join();
    }
}

QString LoggingMaster::getCurrentLogfile() {
    return getInstance().logfile.fileName();
}

LoggingMaster& LoggingMaster::getInstance() {
    static LoggingMaster inst;
    return inst;
}

}  // namespace Logging
