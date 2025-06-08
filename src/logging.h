#ifndef LOGGING_HPP
#define LOGGING_HPP

#include <list>

#include <functional>

#include  <fstream>

#include <thread>
#include <condition_variable>
#include <mutex>

#include <boost/noncopyable.hpp>

#include <boost/fusion/include/for_each.hpp>
#include <boost/fusion/include/make_tuple.hpp>

#ifdef QT_CORE_LIB
#include <QDebug>
#include <QFile>

#include <QPoint>
#endif // QT_CORE_LIB

namespace Logging
{

enum class LoggingType
{
    Empty,
    Debug,
    Info,
    Warning,
    Error,
    Ok
};

template <LoggingType LogType> constexpr const char* logTypeStringColored()
{
    switch (LogType)
    {
    case LoggingType::Info: return "\033[0m INFO \033[0m";
    case LoggingType::Warning: return "\033[33m WARN \033[0m";

    case LoggingType::Ok: return "\033[32m  OK  \033[0m";
    case LoggingType::Error: return "\033[31m FAIL \033[0m";

    case LoggingType::Debug: return "\033[35m DEBG \033[0m";
    }
    return "";
}

template <LoggingType LogType> constexpr const char* logTypeString()
{
    switch (LogType)
    {
    case LoggingType::Info: return " INFO ";
    case LoggingType::Warning: return " WARN ";

    case LoggingType::Ok: return "  OK  ";
    case LoggingType::Error: return " FAIL ";

    case LoggingType::Debug: return " DEBG ";
    }
    return "";
}

const std::string LOGFILE_NAME {"app.log"};

#ifdef QT_CORE_LIB
static QFile logfile (LOGFILE_NAME.c_str());
#else
static std::fstream logfile;
#endif // QT_CORE_LIB

class LoggingMaster : boost::noncopyable
{
    class LoggingHelper
    {
#ifdef QT_CORE_LIB
        QDebug m_dbgStream {qDebug()};
#endif // QT_CORE_LIB

    public:
        template <typename T>
        void fileWriteOnly(T val) {
#ifdef QT_CORE_LIB
            logfile << val << " ";
#else
            logfile << val << " ";
#endif // QT_CORE_LIB
        }

        template <typename T>
        void ostreamWriteOnly(T val) {
#ifdef QT_CORE_LIB
            m_dbgStream << val;
#else
            std::cout << val << " ";
#endif // QT_CORE_LIB
        }

        template <typename T>
        void operator()(T val) {
            ostreamWriteOnly(val);
            fileWriteOnly(val);
        }
    };

    bool isWorking {false};
    bool isThreadExited {true};
    std::thread logThread;
    std::list< std::function<void()> > taskList;
    std::mutex taskListMx;

    std::condition_variable addTaskCV;
    std::mutex addTaskMx;
    void waitForTasks() {
        std::unique_lock<std::mutex> lock(addTaskMx);
        addTaskCV.wait(lock);
    }

    void taskAdded() {
        std::unique_lock<std::mutex> lock(addTaskMx);
        addTaskCV.notify_one();
    }

    LoggingMaster() {
        isWorking = true;
        logThread = std::thread([this](){
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
    }

    ~LoggingMaster() {
        isWorking = false;
        while (!isThreadExited) {
            taskAdded();
        }
        if (logThread.joinable()) {
            logThread.join();
        }
    }

public:
    static LoggingMaster& getInstance() {
        static LoggingMaster inst;
        return inst;
    }

    template <LoggingType lt, bool isSync, typename...Args>
    void log(Args...args) {

        // System time value
        auto timeValue = std::time(nullptr);
        auto locTime = std::localtime(&timeValue);

        auto formattedNumber = [](uint8_t iNum) -> std::string {
            return (iNum > 9 ? std::to_string(iNum) : std::string("0") + std::to_string(iNum));
        };

        std::string timestamp {
            formattedNumber(locTime->tm_mday) + "." + formattedNumber(locTime->tm_mon) + ".20" + formattedNumber(locTime->tm_year - 100) + " " +
            formattedNumber(locTime->tm_hour) + ":" + formattedNumber(locTime->tm_min) + ":" + formattedNumber(locTime->tm_sec)
        };

        auto task = [=](){
#ifdef QT_CORE_LIB
            logfile.open(QIODevice::WriteOnly | QIODevice::Append);
#else
            logfile.open(LOGFILE_NAME, std::ios_base::out | std::iosios_base::app);
#endif // QT_CORE_LIB

            LoggingHelper logger;
            logger.ostreamWriteOnly(timestamp + " [" + logTypeStringColored<lt>() + "] ");
            logger.fileWriteOnly(timestamp + " [" + logTypeString<lt>() + "] ");
            {
                boost::fusion::for_each(boost::fusion::make_tuple(args...), logger);
            }

#ifdef QT_CORE_LIB
            logfile.write("\n");
            logfile.flush();
            logfile.close();
#else
            logfile << std::endl;
#endif // QT_CORE_LIB
        };

        if constexpr (isSync) {
            task();
            return;
        }

        taskListMx.lock();
        taskList.push_back(task);
        taskListMx.unlock();
        taskAdded();
    }
};


// ============================================================================== //
// ============================================================================== //
// =================================== FOR QT TYPES ============================= //
// ============================================================================== //
// ============================================================================== //

#ifdef QT_CORE_LIB
template <>
inline void LoggingMaster::LoggingHelper::fileWriteOnly(const char* val) {
    qDebug() << "\nWRITING:" << val << "   \n";
    logfile.write(val);
    logfile.write(" ");
}

template <>
inline void LoggingMaster::LoggingHelper::fileWriteOnly(std::string val) {
    logfile.write(val.c_str());
    logfile.write(" ");
}

template <>
inline void LoggingMaster::LoggingHelper::fileWriteOnly(QString val) {
    logfile.write(val.toUtf8().data());
    logfile.write(" ");
}

template <>
inline void LoggingMaster::LoggingHelper::fileWriteOnly(double val) {
    logfile.write(QString::number(val).toUtf8().data());
    logfile.write(" ");
}

template <>
inline void LoggingMaster::LoggingHelper::fileWriteOnly(QPoint val) {
    logfile.write("[POINT ");
    logfile.write(QString::number(val.x()).toUtf8().data());
    logfile.write(QString::number(val.y()).toUtf8().data());
    logfile.write("] ");
}

template <>
inline void LoggingMaster::LoggingHelper::fileWriteOnly(QPointF val) {
    logfile.write("[POINT ");
    logfile.write(QString::number(val.x()).toUtf8().data());
    logfile.write(QString::number(val.y()).toUtf8().data());
    logfile.write("] ");
}

// ============================================================================== //

template <>
inline void LoggingMaster::LoggingHelper::ostreamWriteOnly(std::string val) {
    m_dbgStream << val.c_str();
}

template <>
inline void LoggingMaster::LoggingHelper::ostreamWriteOnly(QString val) {
    m_dbgStream << val.toUtf8().data();
}

template <>
inline void LoggingMaster::LoggingHelper::ostreamWriteOnly(QPoint val) {
    m_dbgStream << val;
}

template <>
inline void LoggingMaster::LoggingHelper::ostreamWriteOnly(QPointF val) {
    m_dbgStream << val;
}

#endif // QT_CORE_LIB


} // namespace Logging


// Параллельный логгер
#define LOG_EMPTY(...)      Logging::LoggingMaster::getInstance().log<Logging::LoggingType::Empty, false>(__VA_ARGS__)
#define LOG_DEBUG(...)      Logging::LoggingMaster::getInstance().log<Logging::LoggingType::Debug, false>(__VA_ARGS__)
#define LOG_INFO(...)       Logging::LoggingMaster::getInstance().log<Logging::LoggingType::Info, false>(__VA_ARGS__)
#define LOG_WARNING(...)    Logging::LoggingMaster::getInstance().log<Logging::LoggingType::Warning, false>(__VA_ARGS__)
#define LOG_ERROR(...)      Logging::LoggingMaster::getInstance().log<Logging::LoggingType::Error, false>(__VA_ARGS__)
#define LOG_OK(...)         Logging::LoggingMaster::getInstance().log<Logging::LoggingType::Ok, false>(__VA_ARGS__)


// Синхронная версия логгера
#define LOG_EMPTY_SYNC(...)     Logging::LoggingMaster::getInstance().log<Logging::LoggingType::Empty, true>(__VA_ARGS__)
#define LOG_DEBUG_SYNC(...)     Logging::LoggingMaster::getInstance().log<Logging::LoggingType::Debug, true>(__VA_ARGS__)
#define LOG_INFO_SYNC(...)      Logging::LoggingMaster::getInstance().log<Logging::LoggingType::Info, true>(__VA_ARGS__)
#define LOG_WARNING_SYNC(...)   Logging::LoggingMaster::getInstance().log<Logging::LoggingType::Warning, true>(__VA_ARGS__)
#define LOG_ERROR_SYNC(...)     Logging::LoggingMaster::getInstance().log<Logging::LoggingType::Error, true>(__VA_ARGS__)
#define LOG_OK_SYNC(...)        Logging::LoggingMaster::getInstance().log<Logging::LoggingType::Ok, true>(__VA_ARGS__)

#endif // LOGGING_HPP
