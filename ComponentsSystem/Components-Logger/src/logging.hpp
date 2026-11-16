#pragma once

// Здесь макрос сугубо чтобы не было варнингов инклудов
#ifdef COMPONENTS_IS_ENABLED_QT
#include "qt/logger.hpp"
#else
#include "noqt/logger.hpp"
#endif // COMPONENTS_IS_ENABLED_QT

// Для вывода информации о типе объекта
#if __has_include(<boost/core/demangle.hpp>)
#include <boost/core/demangle.hpp>
#endif // has <boost/core/demangle.hpp>


namespace Logger
{

// Работа с Qt версией и без него
#ifdef COMPONENTS_IS_ENABLED_QT
using Instance = LoggerQt::Instance;
#else
using Instance = LoggerNoQt::Instance;
#endif // COMPONENTS_IS_ENABLED_QT

}

// Logger instance
#define COMPLOG_GET_INSTANCE() \
    Logger::Instance::getInstance<Logger::Instance>()

// Задание рабочей директории
#define COMPLOG_SET_LOGSDIR(logsBasedir) \
    Logger::Instance::getInstance<Logger::Instance>(logsBasedir)
#define COMPLOG_GET_LOGFILE() \
    Logger::Instance::getInstance<Logger::Instance>().getFilewriter().getLogfilePath()


// Базовый макрос для COMPLOG_*
#define COMPLOG_PRIVATE_LOG_BASE(logLevel, logIsSync, ...)   \
    Logger::Instance::getInstance<Logger::Instance>()       \
        .log<Logger::Level::logLevel, logIsSync>(__VA_ARGS__)


// Параллельный логгер (макросы вывода данных через другой поток)
#if __has_include(<boost/core/demangle.hpp>)
#define COMPLOG_TYPENAME(Logger_LOGITEM) \
    COMPLOG_PRIVATE_LOG_BASE(Debug, false, "Type of object: [", #Logger_LOGITEM, "] is: [", boost::core::demangle(typeid(Logger_LOGITEM).name()), "]")
#else
#define COMPLOG_TYPENAME(Logger_LOGITEM) #error "No boost::core::demangle found. Install it before calling this macro"
#endif // has (<boost/core/demangle.hpp>)
#define COMPLOG_EMPTY(...)     COMPLOG_PRIVATE_LOG_BASE(Empty,   false, __VA_ARGS__)
#define COMPLOG_DEBUG(...)     COMPLOG_PRIVATE_LOG_BASE(Debug,   false, __VA_ARGS__)
#define COMPLOG_INFO(...)      COMPLOG_PRIVATE_LOG_BASE(Info,    false, __VA_ARGS__)
#define COMPLOG_WARNING(...)   COMPLOG_PRIVATE_LOG_BASE(Warning, false, __VA_ARGS__)
#define COMPLOG_ERROR(...)     COMPLOG_PRIVATE_LOG_BASE(Error,   false, __VA_ARGS__)
#define COMPLOG_OK(...)        COMPLOG_PRIVATE_LOG_BASE(Ok,      false, __VA_ARGS__)


// Синхронная версия логгера (макросы вывода данных через текущий поток)
#if __has_include(<boost/core/demangle.hpp>)
#define COMPLOG_SYNC_TYPENAME(Logger_LOGITEM) \
    COMPLOG_PRIVATE_LOG_BASE(Debug, true, "Type of object: [", #Logger_LOGITEM, "] is: [", boost::core::demangle(typeid(Logger_LOGITEM).name()), "]")
#else
#define COMPLOG_SYNC_TYPENAME(Logger_LOGITEM) #error "No boost::core::demangle found. Install it before calling this macro"
#endif // has (<boost/core/demangle.hpp>)
#define COMPLOG_SYNC_EMPTY(...)     COMPLOG_PRIVATE_LOG_BASE(Empty,   true, __VA_ARGS__)
#define COMPLOG_SYNC_DEBUG(...)     COMPLOG_PRIVATE_LOG_BASE(Debug,   true, __VA_ARGS__)
#define COMPLOG_SYNC_INFO(...)      COMPLOG_PRIVATE_LOG_BASE(Info,    true, __VA_ARGS__)
#define COMPLOG_SYNC_WARNING(...)   COMPLOG_PRIVATE_LOG_BASE(Warning, true, __VA_ARGS__)
#define COMPLOG_SYNC_ERROR(...)     COMPLOG_PRIVATE_LOG_BASE(Error,   true, __VA_ARGS__)
#define COMPLOG_SYNC_OK(...)        COMPLOG_PRIVATE_LOG_BASE(Ok,      true, __VA_ARGS__)
