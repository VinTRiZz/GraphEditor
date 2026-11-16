#include "utility.hpp"

#include <stdexcept>
#include <chrono>
#include <random>
#include <iomanip>
#include <sstream>

#ifdef __linux__
#include <signal.h>
#include <termios.h>
#else
#include <windows.h> // TODO: Check it, one day when i even start Windows again
#endif // Linux

#include <cstring>

#define BOOST_STACKTRACE_USE_ADDR2LINE
#include <boost/stacktrace.hpp>
#include <boost/core/demangle.hpp>

#include <Components/Logger/Logger.h>

namespace Common
{

void printStacktrace()
{
    COMPLOG_SYNC_EMPTY("STACK TRACE:");
    COMPLOG_SYNC_EMPTY("====================================================");
    boost::stacktrace::stacktrace stackTrace;
    int traceLayer {0};
    for (auto& line : stackTrace) {
        COMPLOG_SYNC_EMPTY(traceLayer++, to_string(line));
    }

    COMPLOG_SYNC_EMPTY("====================================================");
}

void printStacktraceNoLogger()
{
    std::cout << "================ STACK TRACE =================" << std::endl;
    std::cout << boost::stacktrace::stacktrace() << std::endl;
    std::cout << "==============================================" << std::endl;
}

static std::function<bool (int)> currentSignalProcessor;

void processSignal(int signo) {
    if (currentSignalProcessor && currentSignalProcessor(signo)) {
        return;
    }
    COMPLOG_SYNC_ERROR("SIGNAL:", signo, "(", strsignal(signo), ")");
    printStacktrace();
    ::signal(signo, SIG_DFL);
}

void setupBacktrace(std::function<bool (int)> &&signalProcessor)
{
    currentSignalProcessor = std::move(signalProcessor);
    setupBacktrace();
}


void setupBacktrace()
{
    ::signal(SIGSEGV, &processSignal);
    ::signal(SIGABRT, &processSignal);
    ::signal(SIGTERM, &processSignal);
    ::signal(SIGFPE, &processSignal);
}

void restartSelf() {
#ifdef __linux__
    std::cerr << "RESTART SELF CALLED" << std::endl;

    // Get self
    char path[1024];
    ssize_t len = readlink("/proc/self/exe", path, sizeof(path) - 1);
    if (len == -1) {
        std::cerr << "RESTART SELF FAILED: Binary not found" << std::endl;
        exit(1);
    }
    path[len] = '\0';

    // Fork self
    pid_t pid = fork();
    if (pid < 0) {
        std::cerr << "RESTART SELF FAILED: Fork failure, reason: " << std::strerror(errno) << std::endl;
        exit(1);
    }

    // Free resources of parent
    if (pid > 0) { exit(0); }

    // Replace with child
    char* args[] = {path, nullptr};
    execv(path, args);

    std::cerr << "RESTART SELF FAILED: Replace failure, reason: " << std::strerror(errno) << std::endl;
    exit(1);
#else
    // TODO: Implement behaviour
#error "Restart self is not implemented for Windows"
#endif
}

void terminalGotoXY(int x, int y)
{
#ifdef __linux__
    printf("%c[%d;%df",0x1B,y,x);
#else
    HANDLE hc = GetStdHandle(STD_OUTPUT_HANDLE);  // get console handle
    COORD cursor = { x, y };
    SetConsoleCursorPosition(hc, cursor);  // set new cursor position
#endif // __linux__
}

std::pair<unsigned, unsigned> terminalGetXY()
{
    std::pair<unsigned, unsigned> res;
#ifdef __linux__
    struct termios oldt, newt;
    tcgetattr(STDIN_FILENO, &oldt);
    newt = oldt;
    newt.c_lflag &= ~(ICANON | ECHO);
    tcsetattr(STDIN_FILENO, TCSANOW, &newt);

    // Ask and get pos
    std::cout << "\033[6n" << std::flush;
    char buf[32];
    int i = 0;
    while (i < sizeof(buf) - 1) {
        if (read(STDIN_FILENO, buf + i, 1) != 1) break;
        if (buf[i] == 'R') break;
        i++;
    }
    buf[i] = '\0';

    tcsetattr(STDIN_FILENO, TCSANOW, &oldt); // Восстанавливаем настройки

    sscanf(buf, "\033[%d;%d", &res.first, &res.second);
    return res;
#else
    HANDLE hc = GetStdHandle(STD_OUTPUT_HANDLE);  // get console handle
    CONSOLE_SCREEN_BUFFER_INFO csbi;        // screen buffer information
    GetConsoleScreenBufferInfo(hc, &csbi);      // Get screen info & size
    res.first = csbi.dwCursorPosition.X;
    res.second = csbi.dwCursorPosition.Y;
    return res;
#endif // __linux__
}

int createRandomNumber(int min, int max)
{
    if (min >= max) throw std::invalid_argument("createRandomNumber: max >= min");
    std::random_device rd;
    std::mt19937 rng(rd());
    std::uniform_int_distribution<int> uni(min, max);
    return uni(rng);
}

std::string createRandomString(unsigned int stringLength) {
    std::random_device rd;
    std::mt19937 gen(rd());

    const std::string characters =
            "0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz";
    std::uniform_int_distribution<> dis(0, characters.size() - 1);

    std::string result;
    result.reserve(stringLength);
    for (unsigned int i = 0; i < stringLength; ++i)
        result += characters[dis(gen)];

    return result;
}

uint64_t getEpoch()
{
    auto now = std::chrono::system_clock::now();
    auto duration = now.time_since_epoch();
    return std::chrono::duration_cast<std::chrono::seconds>(duration).count();
}

std::string getCurrentTimestampFormatted(const std::string& tsFmt) {
    auto now = std::chrono::system_clock::now();
    auto now_ms = std::chrono::time_point_cast<std::chrono::milliseconds>(now);

    std::time_t now_c = std::chrono::system_clock::to_time_t(now_ms);
    std::tm now_tm;

    localtime_r(&now_c, &now_tm);

    std::ostringstream oss;
    oss << std::put_time(&now_tm, tsFmt.c_str());

    return oss.str();
}

std::string getCurrentTimestampFilename(bool containTime)
{
    auto now = std::chrono::system_clock::now();
    auto now_ms = std::chrono::time_point_cast<std::chrono::milliseconds>(now);

    std::time_t now_c = std::chrono::system_clock::to_time_t(now_ms);
    std::tm now_tm;

    localtime_r(&now_c, &now_tm);

    std::ostringstream oss;
    if (containTime) {
        oss << std::put_time(&now_tm, "%Y%m%d_%H%M%S")
        << '.' << std::setfill('0') << std::setw(3)
        << (now_ms.time_since_epoch().count() % 1000);
    } else {
        oss << std::put_time(&now_tm, "%Y%m%d");
    }

    return oss.str();
}

}
