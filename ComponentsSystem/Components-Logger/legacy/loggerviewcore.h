#ifndef LOGGERVIEWCORE_H
#define LOGGERVIEWCORE_H

#include <memory>

#include <QString>
#include <QVector>

#include <functional>

namespace LoggingOld
{

enum LogType
{
    COMPLOG_TYPE_UNKNOWN,

    // Qt logs
    COMPLOG_TYPE_DEBUG,
    COMPLOG_TYPE_INFO,
    COMPLOG_TYPE_WARNING,
    COMPLOG_TYPE_CRITICAL,
    COMPLOG_TYPE_FATAL,

    // Std
    COMPLOG_TYPE_STDOUT,
    COMPLOG_TYPE_STDERR
};

// Storage for messages
struct LogMessageStruct
{
    QString text;
    LogType type;

    QString functionstamp;
    QString filestamp;
    QString timestamp;

    static QString typeString(LogType t);
    static LogType typeFromString(const QString& t);
};


class LoggerViewCore
{
public:
    LoggerViewCore();
    ~LoggerViewCore();

    void setLogChannel(std::function<void(const QString&)> logger);

    // Setup log file name or get it
    void setLogFile(const QString& filename);
    QString logFileName() const;

    // First one parses set before file, second one parses inserted
    bool parseFile();
    bool parseFile(const QString& filename);

    // Date and time program started in current log period
    QString date() const;
    QString time() const;

    // Switch between program launches
    void resetDate();
    bool setPrevDate();
    bool setNextDate();
    size_t logDateCount() const;

    // Message index work
    size_t messageCount() const;
    void resetMessageIndex();
    size_t currentMessageIndex() const;

    // Iterate between messages. Unsafe (take care of indexes)
    bool setPrevMessage();    // Get past and index -1
    std::shared_ptr<LogMessageStruct> message();        // Current
    bool setNextMessage();    // Get next and index +1

private:
    struct LoggerViewCorePrivate;
    std::unique_ptr<LoggerViewCorePrivate> d;

    void parseLine(const QString& lineData);

    std::function<void(const QString&)> m_logger;
};

}
#endif // LOGGERVIEWCORE_H
