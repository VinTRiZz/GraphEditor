#ifndef LOGGING_HPP
#define LOGGING_HPP

#include <condition_variable>
#include <functional>
#include <list>
#include <mutex>
#include <thread>

#if __cplusplus < 201701UL
#include <boost/fusion/include/for_each.hpp>
#include <boost/fusion/include/make_tuple.hpp>
#endif  // C++ 17

#include <boost/noncopyable.hpp>

#include <QDebug>
#include <QDir>
#include <QFile>
#include <QPoint>
#include <QCoreApplication>

#include "applicationsettings.h"

namespace Logging {

/**
 * @brief The LoggingType enum Тип данных для вывода
 */
enum class LoggingType { Empty, Debug, Info, Warning, Error, Ok };

/**
 * @brief logTypeStringColored Получение выделенного цветом текста для логов
 * @return Строка с управляющими символами
 */
template<LoggingType LogType>
constexpr const char* logTypeStringColored() {
    switch (LogType) {
        case LoggingType::Info:
            return "\033[37m INFO \033[0m";
        case LoggingType::Warning:
            return "\033[33m WARN \033[0m";

        case LoggingType::Ok:
            return "\033[32m  OK  \033[0m";
        case LoggingType::Error:
            return "\033[31m FAIL \033[0m";

        case LoggingType::Debug:
            return "\033[35m DEBG \033[0m";
    }
    return "";
}

/**
 * @brief logTypeString Получение не выделенного цветом текста для логов
 * @return Строка без управляющих последовательностей
 */
template<LoggingType LogType>
constexpr const char* logTypeString() {
    switch (LogType) {
        case LoggingType::Info:
            return " INFO ";
        case LoggingType::Warning:
            return " WARN ";

        case LoggingType::Ok:
            return "  OK  ";
        case LoggingType::Error:
            return " FAIL ";

        case LoggingType::Debug:
            return " DEBG ";
    }
    return "";
}

/**
 * @brief The LoggingMaster class Мастер вывода информации (логов). Синглетон
 */
class LoggingMaster : public boost::noncopyable {

    QFile logfile;                        //! Логфайл
    QTextStream logfileStream{&logfile};  //! Поток ввода в файл данных

    std::mutex logfileMx;  //! Мьютекс для единоличной записи данных в файл

    /**
     * @brief The LoggingHelper class Помощник вывода данных в логфайл
     */
    class LoggingHelper {
        QDebug m_dbgStream{qDebug()};  //! Поток вывод (для Qt)
    public:
        /**
         * @brief fileWriteOnly Запись данных в файл
         * @param val данные
         */
        template<typename T>
        void fileWriteOnly(T val) {
            LoggingMaster::getInstance().logfileStream << val << " ";
        }

        /**
         * @brief ostreamWriteOnly Вывод данных в терминал
         * @param val данные
         */
        template<typename T>
        void ostreamWriteOnly(T val) {
            m_dbgStream << val;
        }

        /**
         * @brief operator () Оператор для вывода данных в терминал и записи в
         * файл
         * @param val данные
         */
        template<typename T>
        void operator()(T&& val) {
            fileWriteOnly(val);
            ostreamWriteOnly(val);
        }
    };

    bool isWorking{
        false};  //! Флаг для определения, обязан ли логгер работать дальше
    bool isThreadExited{true};  //! Флаг для определения, закончил ли выполнение
                                //! поток вывода логов
    std::thread logThread;      //! Поток вывода логов
    std::list<std::function<void()> >
        taskList;           //! Список задач по выводу данных в поток логов
    std::mutex taskListMx;  //! Мьютекс для получения данных из списка задач по
                            //! выводу данных
    std::condition_variable addTaskCV;  //! Переменная для ожидания добавления
                                        //! данных на вывод в поток логов
    std::mutex addTaskMx;  //! Мьютекс для ожидания добавления данных на вывод в
                           //! поток логов

    /**
     * @brief waitForTasks Ожидать сигнала о добавлении задачи на вывод
     */
    void waitForTasks() {
        std::unique_lock<std::mutex> lock(addTaskMx);
        addTaskCV.wait(lock);
    }

    /**
     * @brief taskAdded Уведомить о получении задачи на вывод
     */
    void taskAdded() {
        std::unique_lock<std::mutex> lock(addTaskMx);
        addTaskCV.notify_one();
    }

    std::string getCurrentTimestamp() const {
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

    std::string getCurrentTimestampFormatted() const {
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

    LoggingMaster() {
        auto logsDir = getLogsDir();
        logfile.setFileName(logsDir.absolutePath() + QDir::separator() +
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
    /**
     * @brief getLogsDir    Получить директорию логов
     * @return              Директория логов, созданная при отсутствии
     */
    static QDir getLogsDir() {
        auto logsDir = ApplicationSettings::getApplicationDataDir();
        logsDir.mkdir("logs");
        logsDir.cd("logs");
        return logsDir;
    }

    static QString getCurrentLogfile() {
        return getInstance().logfile.fileName();
    }

    /**
     * @brief getInstance Получить объект сигнлетона мастера логгирования
     * @return
     */
    static LoggingMaster& getInstance() {
        static LoggingMaster inst;
        return inst;
    }

    /**
     * @brief log Вывести данные в потоке логгирования. Для синхронного вывода
     * укажите isSync как true
     * @param args Данные на вывод
     */
    template<LoggingType lt, bool isSync, typename... Args>
    void log(Args... args) {
        auto timestamp = getCurrentTimestamp();

        auto task = [=]() {
            logfileMx.lock();

            logfile.open(QIODevice::Append);
            if (!logfile.isOpen()) {
                throw std::runtime_error(std::string("Error opening logfile (logfile path: ") + logfile.fileName().toStdString());
            }

            LoggingHelper logger;
            logger.ostreamWriteOnly(timestamp + " [" +
                                    logTypeStringColored<lt>() + "] ");
            logger.fileWriteOnly(timestamp + " [" + logTypeString<lt>() + "] ");

#if __cplusplus < 201701UL
            boost::fusion::for_each(boost::fusion::make_tuple(args...), logger);
#else
            (logger(args), ...);
#endif  // C++ 17

            logfileStream << Qt::endl;
            logfile.flush();
            logfile.close();
            logfileMx.unlock();
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

// ==============================================================================
// //
// ==============================================================================
// //
// =================================== FOR QT TYPES
// ============================= //
// ==============================================================================
// //
// ==============================================================================
// //

template<>
inline void LoggingMaster::LoggingHelper::ostreamWriteOnly(std::string val) {
    m_dbgStream << val.c_str();
}

template<>
inline void LoggingMaster::LoggingHelper::ostreamWriteOnly(QString val) {
    m_dbgStream << val.toUtf8().data();
}

template<>
inline void LoggingMaster::LoggingHelper::ostreamWriteOnly(QPoint val) {
    m_dbgStream << val;
}

template<>
inline void LoggingMaster::LoggingHelper::ostreamWriteOnly(QPointF val) {
    m_dbgStream << val;
}

template<>
inline void LoggingMaster::LoggingHelper::fileWriteOnly(std::string val) {
    LoggingMaster::getInstance().logfileStream << val.c_str() << " ";
}

template<>
inline void LoggingMaster::LoggingHelper::fileWriteOnly(const char* val) {
    LoggingMaster::getInstance().logfileStream << QByteArray(val) << " ";
}

template<>
inline void LoggingMaster::LoggingHelper::fileWriteOnly(QString val) {
    LoggingMaster::getInstance().logfileStream << val.toUtf8().data() << " ";
}

template<>
inline void LoggingMaster::LoggingHelper::fileWriteOnly(QPoint val) {
    LoggingMaster::getInstance().logfileStream
        << "{P " << QString::number(val.x()).toUtf8().data() << "; "
        << QString::number(val.y()).toUtf8().data() << "} ";
}

template<>
inline void LoggingMaster::LoggingHelper::fileWriteOnly(QPointF val) {
    LoggingMaster::getInstance().logfileStream
        << "{P " << QString::number(val.x()).replace(",", ".").toUtf8().data()
        << "; " << QString::number(val.y()).replace(",", ".").toUtf8().data()
        << "} ";
}

template<>
inline void LoggingMaster::LoggingHelper::fileWriteOnly(double val) {
    // Принудительная замена с игнором локали
    auto resv = QString::number(val);
    resv = resv.replace(",", ".");

    LoggingMaster::getInstance().logfileStream << resv.toUtf8().data() << " ";
}

template<>
inline void LoggingMaster::LoggingHelper::fileWriteOnly(QVariant val) {
    LoggingMaster::getInstance().logfileStream << val.toString().toUtf8().data()
                                               << " ";
}

}  // namespace Logging

// Параллельный логгер (макросы вывода данных через другой поток)
#define LOG_EMPTY(...)                                                         \
    Logging::LoggingMaster::getInstance()                                      \
        .log<Logging::LoggingType::Empty, false>(__VA_ARGS__)
#define LOG_DEBUG(...)                                                         \
    Logging::LoggingMaster::getInstance()                                      \
        .log<Logging::LoggingType::Debug, false>(__VA_ARGS__)
#define LOG_INFO(...)                                                          \
    Logging::LoggingMaster::getInstance()                                      \
        .log<Logging::LoggingType::Info, false>(__VA_ARGS__)
#define LOG_WARNING(...)                                                       \
    Logging::LoggingMaster::getInstance()                                      \
        .log<Logging::LoggingType::Warning, false>(__VA_ARGS__)
#define LOG_ERROR(...)                                                         \
    Logging::LoggingMaster::getInstance()                                      \
        .log<Logging::LoggingType::Error, false>(__VA_ARGS__)
#define LOG_OK(...)                                                            \
    Logging::LoggingMaster::getInstance()                                      \
        .log<Logging::LoggingType::Ok, false>(__VA_ARGS__)

// Синхронная версия логгера (макросы вывода данных через текущий поток)
#define LOG_EMPTY_SYNC(...)                                                    \
    Logging::LoggingMaster::getInstance()                                      \
        .log<Logging::LoggingType::Empty, true>(__VA_ARGS__)
#define LOG_DEBUG_SYNC(...)                                                    \
    Logging::LoggingMaster::getInstance()                                      \
        .log<Logging::LoggingType::Debug, true>(__VA_ARGS__)
#define LOG_INFO_SYNC(...)                                                     \
    Logging::LoggingMaster::getInstance()                                      \
        .log<Logging::LoggingType::Info, true>(__VA_ARGS__)
#define LOG_WARNING_SYNC(...)                                                  \
    Logging::LoggingMaster::getInstance()                                      \
        .log<Logging::LoggingType::Warning, true>(__VA_ARGS__)
#define LOG_ERROR_SYNC(...)                                                    \
    Logging::LoggingMaster::getInstance()                                      \
        .log<Logging::LoggingType::Error, true>(__VA_ARGS__)
#define LOG_OK_SYNC(...)                                                       \
    Logging::LoggingMaster::getInstance().log<Logging::LoggingType::Ok, true>( \
        __VA_ARGS__)

#endif  // LOGGING_HPP
