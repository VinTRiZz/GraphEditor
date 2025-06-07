#ifndef LOGGING_HPP
#define LOGGING_HPP

#include <cstring>
#include <string>

#include <fstream>  // For logfile
#include <iostream> // Terminal printing
#include <mutex>    // To print one message per output

// C++ 14 problems
#if (__cplusplus > 201402L)
#include <filesystem>
namespace stdfs = std::filesystem;
#else
#include <experimental/filesystem>
namespace stdfs = std::experimental::filesystem;
#endif // C++ 14 problems

#include <boost/fusion/include/for_each.hpp>
#include <boost/fusion/include/make_tuple.hpp>

namespace Logging
{

inline std::string APP_LOGFILE_PATH {"ManagementServer.log"};
inline std::fstream logfile;

static std::mutex logMutex;

enum class MessageType : uint8_t
{
    MESSAGE_TYPE_EMPTY,   // To print just text, without data, place etc. labels
    MESSAGE_TYPE_DEBUG,   // Debug messages

    // App function results
    MESSAGE_TYPE_INFO,
    MESSAGE_TYPE_WARNING,
    MESSAGE_TYPE_ERROR,

    // For operations like getting photo and processing
    MESSAGE_TYPE_OK,
};

template <MessageType LogType> constexpr const char* setupLogTypeTextColored()
{
    switch (LogType)
    {
    case MessageType::MESSAGE_TYPE_INFO: return "\033[0m INFO \033[0m";
    case MessageType::MESSAGE_TYPE_WARNING: return "\033[33m WARN\033[0m";

    case MessageType::MESSAGE_TYPE_OK: return "\033[32m  OK  \033[0m";
    case MessageType::MESSAGE_TYPE_ERROR: return "\033[31m FAIL \033[0m";

    case MessageType::MESSAGE_TYPE_DEBUG: return "\033[35m DEBG \033[0m";
    }
    return "";
}

template <MessageType LogType> constexpr const char* setupLogTypeText()
{
    switch (LogType)
    {
    case MessageType::MESSAGE_TYPE_INFO: return "INFO";
    case MessageType::MESSAGE_TYPE_WARNING: return "WARN";

    case MessageType::MESSAGE_TYPE_ERROR: return "FAIL";
    case MessageType::MESSAGE_TYPE_OK: return "  OK  ";

    case MessageType::MESSAGE_TYPE_DEBUG: return " DEBG ";
    }
    return "";
}

struct Logger
{
    template <typename T>
    struct is_optional : std::false_type {};

    template <typename T>
    void operator ()(T val) {
        std::cout << val << " ";
        logfile << val << " ";
    }
};

template <>
inline void Logger::operator ()(bool val) {
    std::cout << (val ? "true" : "false") << " ";
    logfile << (val ? "true" : "false") << " ";
}

template <>
inline void Logger::operator ()(std::string val) {
    std::cout << (val.empty() ? "[EMPTY_STRING]" : val) << " ";
    logfile << (val.empty() ? "[EMPTY_STRING]" : val) << " ";
}

// Print and save message
template <MessageType LogType, typename...Args> void log(Args&&... args)
{
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

    std::lock_guard<std::mutex> lock(logMutex);
    logfile.open(APP_LOGFILE_PATH, std::ios_base::app | std::ios_base::out);
    if (!logfile.is_open())
        throw std::runtime_error(
            std::string("Error opening log file (") +
            strerror(errno) + "). Logfile path: " + APP_LOGFILE_PATH);

    if constexpr (LogType != MessageType::MESSAGE_TYPE_EMPTY) {
        constexpr const char* logTypeTextColored = setupLogTypeTextColored<LogType>();
        std::cout << timestamp << " [" << logTypeTextColored << "] ";

        constexpr const char* logTypeText = setupLogTypeText<LogType>();
        logfile << timestamp << " [" << logTypeText << "] ";
    }

    Logger logger;
    boost::fusion::for_each(boost::fusion::make_tuple(args...), logger);
    std::cout << std::endl;
    logfile << std::endl;
    logfile.close();
}

// Main logging
#define LOG_INFO(...)                                                          \
    Logging::log<Logging::MessageType::MESSAGE_TYPE_INFO>(__VA_ARGS__)
#define LOG_WARNING(...)                                                       \
    Logging::log<Logging::MessageType::MESSAGE_TYPE_WARNING>(__VA_ARGS__)
#define LOG_ERROR(...)                                                         \
    Logging::log<Logging::MessageType::MESSAGE_TYPE_ERROR>(__VA_ARGS__)
#define LOG_CRITICAL(...)                                                      \
    Logging::log<Logging::MessageType::MESSAGE_TYPE_CRITICAL>(__VA_ARGS__)

// Operation result logging
#define LOG_OK(...)                                                            \
    Logging::log<Logging::MessageType::MESSAGE_TYPE_OK>(__VA_ARGS__)

// Etc logging
#define LOG_EMPTY(...)                                                         \
    Logging::log<Logging::MessageType::MESSAGE_TYPE_EMPTY>(__VA_ARGS__)
#define LOG_MESSAGE(...)                                                       \
    Logging::log<Logging::MessageType::MESSAGE_TYPE_MESSAGE>(__VA_ARGS__)
#define LOG_DEBUG(...)                                                         \
    Logging::log<Logging::MessageType::MESSAGE_TYPE_DEBUG>(__VA_ARGS__)

} // namespace Logging

#endif // LOGGING_HPP
