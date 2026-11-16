#include "loggerviewcore.h"

#include <QFile>
#include <QFileInfo>

#include <QRegularExpression>

#include <QDebug>

#define LOGGER_CORE_LOG(what) \
{ \
    if (m_logger) \
        m_logger(what); \
    else \
        qDebug() << what; \
}

namespace LoggingOld
{

// Storage for every launch log messages
struct SessionLogStruct
{
    QString date;
    QString time;

    std::vector<std::shared_ptr<LogMessageStruct>> messages;
};

struct LoggerViewCore::LoggerViewCorePrivate
{
    std::vector<std::shared_ptr<SessionLogStruct>> m_sessions;
    size_t m_currentSessionIndex {0};

    std::shared_ptr<SessionLogStruct> m_currentSessionLog;
    size_t m_currentMessageIndex {0};

    QFile m_logFile;
    quint64 m_currentFilePos;
};

LoggerViewCore::LoggerViewCore() :
    d {new LoggerViewCorePrivate}
{

}

LoggerViewCore::~LoggerViewCore()
{

}

void LoggerViewCore::setLogChannel(std::function<void (const QString &)> logger)
{
    m_logger = logger;
}

void LoggerViewCore::setLogFile(const QString &filename)
{
    if (d->m_logFile.isOpen())
        d->m_logFile.close();

    d->m_logFile.setFileName(filename);
}

QString LoggerViewCore::logFileName() const
{
    return d->m_logFile.fileName();
}

bool LoggerViewCore::parseFile()
{
    if (!d->m_logFile.exists() || QFileInfo(d->m_logFile.fileName()).isDir())
    {
        LOGGER_CORE_LOG(QString("Invalid filename: ") + d->m_logFile.fileName());
        return false;
    }

    d->m_sessions.clear();
    d->m_currentSessionIndex = 0;
    d->m_currentMessageIndex = 0;

    d->m_logFile.open(QIODevice::ReadOnly);

    constexpr int64_t bufferSize = 1024 * 1024;
    char dataBuffer[bufferSize];
    memset(dataBuffer, '\0', bufferSize);

    while (d->m_logFile.readLine(dataBuffer, bufferSize) > 0)
    {
        parseLine(dataBuffer);
    }

    resetDate();
    resetMessageIndex();

    LOGGER_CORE_LOG(QString("Parsed ") + QString::number(d->m_sessions.size()) + " session(s)");

    return true;
}

bool LoggerViewCore::parseFile(const QString &filename)
{
    this->setLogFile(filename);
    return this->parseFile();
}

QString LoggerViewCore::date() const
{
    return d->m_currentSessionLog->date;
}

QString LoggerViewCore::time() const
{
    return d->m_currentSessionLog->time;
}

void LoggerViewCore::resetDate()
{
    d->m_currentMessageIndex = 0;
    d->m_currentSessionIndex = 0;

    if (!d->m_sessions.size())
        return;
    d->m_currentSessionLog = d->m_sessions[d->m_currentSessionIndex];
}

bool LoggerViewCore::setPrevDate()
{
    if (!d->m_sessions.size())
        return false;

    if (d->m_currentSessionIndex == 0)
        return false;

    d->m_currentMessageIndex = 0;
    d->m_currentSessionIndex--;
    d->m_currentSessionLog = d->m_sessions[d->m_currentSessionIndex];
    return true;
}

bool LoggerViewCore::setNextDate()
{
    if (d->m_sessions.size() <= (d->m_currentSessionIndex + 1))
        return false;

    d->m_currentMessageIndex = 0;
    d->m_currentSessionIndex++;
    d->m_currentSessionLog = d->m_sessions[d->m_currentSessionIndex];
    return true;
}

size_t LoggerViewCore::logDateCount() const
{
    return d->m_sessions.size();
}

size_t LoggerViewCore::messageCount() const
{
    return d->m_currentSessionLog->messages.size();
}

void LoggerViewCore::resetMessageIndex()
{
    d->m_currentMessageIndex = 0;
}

size_t LoggerViewCore::currentMessageIndex() const
{
    return d->m_currentMessageIndex;
}

bool LoggerViewCore::setPrevMessage()
{
    if (d->m_currentMessageIndex == 0)
        return {};

    d->m_currentMessageIndex--;
    return true;
}

std::shared_ptr<LogMessageStruct> LoggerViewCore::message()
{
    return d->m_currentSessionLog->messages[d->m_currentMessageIndex];
}

bool LoggerViewCore::setNextMessage()
{
    if ((d->m_currentMessageIndex + 1) >= d->m_currentSessionLog->messages.size())
        return {};

    d->m_currentMessageIndex++;
    return true;
}

void LoggerViewCore::parseLine(const QString &lineData)
{
    if (!lineData.contains(QRegularExpression("\\[[0-9]{2}.[0-9]{2}.[0-9]{4} [0-9]{2}:[0-9]{2}:[0-9]{2}\\]")))
    {
//        LOGGER_CORE_LOG(QString("Skipping line: ") + lineData);
        return;
    }
//    LOGGER_CORE_LOG(QString("Parsing line: ") + lineData);

    QRegularExpression dateExp("[0-9]{2}.[0-9]{2}.[0-9]{4}");
    QRegularExpression timeExp("[0-9]{2}:[0-9]{2}:[0-9]{2}");

    if (lineData.contains("Launch time"))
    {
        d->m_currentSessionLog = std::shared_ptr<SessionLogStruct>(new SessionLogStruct(), std::default_delete<SessionLogStruct>());
        d->m_sessions.push_back(d->m_currentSessionLog);

        auto match = dateExp.match(lineData);
        if (match.hasMatch())
            d->m_currentSessionLog->date = match.captured(0);

        match = timeExp.match(lineData);
        if (match.hasMatch())
            d->m_currentSessionLog->time = match.captured(0);

//        LOGGER_CORE_LOG(QString("Found session: ") + d->m_currentSessionLog->time + d->m_currentSessionLog->date);
        return;
    }

    std::shared_ptr<LogMessageStruct> currentLogMessage = std::shared_ptr<LogMessageStruct>(new LogMessageStruct(), std::default_delete<LogMessageStruct>());

    QRegularExpression timestampMatch(QString("%1 %2").arg(dateExp.pattern(), timeExp.pattern()));
    auto match = timestampMatch.match(lineData);
    if (!match.hasMatch())
        return;

    currentLogMessage->timestamp = match.captured(0);
//    LOGGER_CORE_LOG(QString("Timestamp: ") + currentLogMessage->timestamp);

    QRegularExpression filestampMatch("([\\-\\_\\.A-Za-z0-9]+[/]{0,1}){1,} : [0-9]+");
    match = filestampMatch.match(lineData);
    if (!match.hasMatch())
        return;

    currentLogMessage->filestamp = match.captured(0);
//    LOGGER_CORE_LOG(QString("File: ") + currentLogMessage->filestamp);

    QRegularExpression logTypeMatch("\\[(DEBUG|INFO|WARNING|CRITICAL|FATAL)\\]");
    match = logTypeMatch.match(lineData);
    if (!match.hasMatch())
        return;

    if (lineData.contains("STDOUT"))
        currentLogMessage->type = LogType::LOG_TYPE_STDOUT;
    else if (lineData.contains("STDERR"))
        currentLogMessage->type = LogType::LOG_TYPE_STDERR;
    else
    {
        QString type = match.captured(0);
        if (type.size() < 2)
            return;

        type.remove(0, 1);
        currentLogMessage->type = LogMessageStruct::typeFromString(type);
    }

    const QString wordRegExp("[:]{0,2}[A-Za-z0-9&*<>]+");
    const QString argRegExp = QString("(%1){1,}( %1){0,}(\\, ){0,1}").arg(wordRegExp);
    QRegularExpression functionMatch(QString("(%1 ){1,}(%1)+\\((%2){0,}\\)").arg(wordRegExp, argRegExp));
    match = functionMatch.match(lineData);
    if (!match.hasMatch())
    {
        LOGGER_CORE_LOG(QString("Function read error:") + lineData);
        return;
    }

    currentLogMessage->functionstamp = match.captured(0);
//    LOGGER_CORE_LOG(QString("Function is: ") + currentLogMessage->functionstamp);


    int endpos = match.capturedEnd(0);
    currentLogMessage->text = lineData;
    currentLogMessage->text.remove(0, endpos + 1);
//    LOGGER_CORE_LOG(QString("Text is: ") + currentLogMessage->text);

//    LOGGER_CORE_LOG(QString("Found log:")
//             + currentLogMessage->timestamp + " "
//             + currentLogMessage->filestamp + " "
//             + currentLogMessage->functionstamp + " "
//             + LogMessageStruct::typeString(currentLogMessage->type) + " "
//             + currentLogMessage->text)
//    ;

    d->m_currentSessionLog->messages.push_back(currentLogMessage);
}

QString LogMessageStruct::typeString(LogType t)
{
    switch (t)
    {
    case LogType::LOG_TYPE_DEBUG:
        return "DEBUG";
        break;

    case LogType::LOG_TYPE_INFO:
        return "INFO";

    case LogType::LOG_TYPE_WARNING:
        return "WARNING";

    case LogType::LOG_TYPE_CRITICAL:
        return "CRITICAL";

    case LogType::LOG_TYPE_FATAL:
        return "FATAL";

    case LogType::LOG_TYPE_STDOUT:
        return "STDOUT";

    case LogType::LOG_TYPE_STDERR:
        return "STDERR";

    default:
        return "UNKNOWN";
    }
}

LogType LogMessageStruct::typeFromString(const QString &t)
{
    switch (t[0].toLatin1())
    {
    case 'D':
        return LogType::LOG_TYPE_DEBUG;

    case 'I':
        return LogType::LOG_TYPE_INFO;

    case 'W':
        return LogType::LOG_TYPE_WARNING;

    case 'C':
        return LogType::LOG_TYPE_CRITICAL;

    case 'F':
        return LogType::LOG_TYPE_FATAL;
    }

    if (t == "STDOUT")
        return LogType::LOG_TYPE_STDOUT;

    if (t == "STDERR")
        return LogType::LOG_TYPE_STDERR;

    return LogType::LOG_TYPE_UNKNOWN;
}

}
