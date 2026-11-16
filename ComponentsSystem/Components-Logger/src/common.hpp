#pragma once

#include <string>

#include <iostream>
#include <iomanip>
#include <chrono>
#include <sstream>

namespace Logger
{


/**
 * @brief The LoggingType enum Тип данных для вывода
 */
enum class Level { Empty, Debug, Info, Warning, Error, Ok };

/**
 * @brief createLogtypeColoredString    Получение выделенного цветом текста для логов
 * @return                              Строка с управляющими символами
 */
template<Level LogType>
constexpr const char* createLogtypeColoredString() {
    switch (LogType) {
        case Level::Info:
            return "\033[37m INFO \033[0m";
        case Level::Warning:
            return "\033[33m WARN \033[0m";

        case Level::Ok:
            return "\033[32m  OK  \033[0m";
        case Level::Error:
            return "\033[31m FAIL \033[0m";

        case Level::Debug:
            return "\033[35m DEBG \033[0m";
    }
    return "";
}

/**
 * @brief logTypeString Получение не выделенного цветом текста для логов
 * @return              Строка без управляющих последовательностей
 */
template<Level LogType>
constexpr const char* createLogtypeString() {
    switch (LogType) {
        case Level::Info:
            return " INFO ";
        case Level::Warning:
            return " WARN ";

        case Level::Ok:
            return "  OK  ";
        case Level::Error:
            return " FAIL ";

        case Level::Debug:
            return " DEBG ";
    }
    return "";
}

/**
 * @brief createLofgileName Функция создания названия для логфайла
 * @return                  Строка названия. Пример: 2026-12-31_23-59-59.log
 */
static std::string createLogfileName() {
    auto now = std::chrono::system_clock::now();
    auto now_ms = std::chrono::time_point_cast<std::chrono::milliseconds>(now);

    std::time_t now_c = std::chrono::system_clock::to_time_t(now_ms);
    std::tm now_tm;

    // Use localtime_s for thread safety (Windows)
#ifdef _WIN32
    localtime_s(&now_tm, &now_c);
#else
    // Use localtime_r for thread safety (POSIX)
    localtime_r(&now_c, &now_tm);
#endif

    std::ostringstream oss;
    oss << std::put_time(&now_tm, "%Y-%m-%d_%H-%M-%S.log");

    return oss.str();
}


/**
 * @brief getTimestamp  Функция получения текущего момента времени
 * @return              Момент времени в стандартном формате
 */
static std::string getTimestamp() {
    auto now = std::chrono::system_clock::now();
    auto now_ms = std::chrono::time_point_cast<std::chrono::milliseconds>(now);

    std::time_t now_c = std::chrono::system_clock::to_time_t(now_ms);
    std::tm now_tm;

    // Use localtime_s for thread safety (Windows)
#ifdef _WIN32
    localtime_s(&now_tm, &now_c);
#else
    // Use localtime_r for thread safety (POSIX)
    localtime_r(&now_c, &now_tm);
#endif

    std::ostringstream oss;
    oss << std::put_time(&now_tm, "%Y-%m-%dT%H:%M:%S") // Unified format
        << '.' << std::setfill('0') << std::setw(3)
        << (now_ms.time_since_epoch().count() % 1000);

    return oss.str();
}

}
