#ifndef LOGGER_H
#define LOGGER_H

#include <memory>
#include <QString>
#include <sys/signal.h>

namespace LoggingOld
{

namespace Defines
{

const QString COMPLOG_FILE_PATH {"./testlog.txt"};

const QString COMPLOG_DELIMETER {
    "-------------------------------------------------------------------------------\n"
    "-------------------------------------------------------------------------------\n"
};

}

class Logger
{
public:
    static Logger& instance();

    // Set path to file as a logfile. Creates if not exist, do not work if error occurs
    void setLogFile(const QString& filename);

    // File descriptor of log file
    int logfile_fd() const;

    // Log info (used to connect to qDebug()-like macros)
    void log(QtMsgType logLevel, const QMessageLogContext &ctx, const QString& msg);

private:
    void setup();

    Logger();
    ~Logger();

    struct LoggerPrivate;
    std::unique_ptr<LoggerPrivate> d;
};

//static Logger &staticLogger = Logger::instance();

void logFunction(QtMsgType logLevel, const QMessageLogContext &ctx, const QString& msg);
void signalHandler(int _signo);

}

#endif // LOGGER_H
