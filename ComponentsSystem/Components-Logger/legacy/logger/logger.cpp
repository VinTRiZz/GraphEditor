#include "logger.h"

#include <QDebug>
#include <QFile>
#include <QTextStream>
#include <QDateTime>

#include <iostream>
#include <fstream>
#include <sstream>

#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <sys/signal.h>
#include <sys/resource.h>
#include <execinfo.h>

#include <chrono>

#include "coutbuffer.h"

namespace LoggingOld
{

struct Logger::LoggerPrivate
{
    QString m_logFileName {Defines::LOG_FILE_PATH};

    QFile m_logfile;
    QTextStream m_logstream;
    std::fstream m_logfileStd;

    int64_t m_maxPos = 4096; // Max size of log in bytes

    CoutBuffer m_cbuff;
    CoutBuffer m_crbuff;

    bool m_stdInited {false};
};

Logger::Logger() :
    d {new LoggerPrivate()}
{
    qInstallMessageHandler(logFunction);

    setup();
}

Logger::~Logger()
{
    d->m_logfileStd.close();
}

Logger &Logger::instance()
{
    static Logger loggerInstance;
    return loggerInstance;
}

void Logger::setLogFile(const QString &filename)
{
    d->m_logFileName = filename;
    d->m_logfile.close();
    d->m_logfileStd.close();
    setup();
}

void Logger::setup()
{
    // Setup Qt
    d->m_logfile.setFileName(d->m_logFileName);
    d->m_logfile.open(QIODevice::WriteOnly | QIODevice::Append);

    if (!d->m_logfile.isOpen())
    {
        qFatal("Can not open log file");
        return;
    }

    auto currentTime = QDateTime::currentDateTime();
    auto timestamp = currentTime.toString("[dd.MM.yyyy hh:mm:ss] ");

    d->m_logstream.setDevice(&d->m_logfile);
    d->m_logstream << Defines::LOG_DELIMETER;
    d->m_logstream << "----------------------- Launch time: " << timestamp << Qt::endl;
    d->m_logstream << Defines::LOG_DELIMETER;

    // Setup std::cout
    d->m_cbuff.setFile(d->m_logfile);

    d->m_cbuff.setName("STDOUT");
    d->m_crbuff.setName("STDERR");

    signal(SIGSEGV, signalHandler);
}

int Logger::logfile_fd() const
{
    return d->m_logfile.handle();
}

void Logger::log(QtMsgType logLevel, const QMessageLogContext &ctx, const QString &msg)
{
    if (d->m_logfile.pos() > d->m_maxPos)
    {
        d->m_logfile.close();
        d->m_logfile.open(QIODevice::Truncate | QIODevice::ReadWrite);
    }

    if (!d->m_stdInited)
    {        
        std::cout.rdbuf(&d->m_cbuff);
        std::cerr.rdbuf(&d->m_crbuff);

        d->m_stdInited = true;
    }

    auto currentTime = QDateTime::currentDateTime();
    auto timestamp = currentTime.toString("[dd.MM.yyyy hh:mm:ss] ");

    auto contextString = QString(" [") + ctx.file + " : " + QString::number(ctx.line) + "] [" + ctx.function + "] ";

    QString logLevelString;
    switch (logLevel)
    {
    case QtDebugMsg:
        logLevelString = "[DEBUG]";
        break;

    case QtInfoMsg:
        logLevelString = "[INFO]";
        break;

    case QtWarningMsg:
        logLevelString = "[WARNING]";
        break;

    case QtCriticalMsg:
        logLevelString = "[CRITICAL]";
        break;

    case QtFatalMsg:
        logLevelString = "[FATAL]";
        break;
    }

    auto outputString = timestamp + logLevelString + contextString + msg;
    d->m_logstream << outputString << Qt::endl;
    qDebug() << outputString.toUtf8().data() << Qt::endl;
}

void logFunction(QtMsgType logLevel, const QMessageLogContext &ctx, const QString &msg)
{
//    staticLogger.log(logLevel, ctx, msg);
}

void signalHandler(int _signo)
{
    if (_signo == SIGSEGV)
    {
        const size_t backtraceSize = 100;
        void* tempArray[backtraceSize];
        qDebug() << "SEGMENTATION FAULT";
//        backtrace_symbols_fd(tempArray, backtraceSize, staticLogger.logfile_fd());
        exit(-1);
    }
}

}
