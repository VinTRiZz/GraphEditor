#pragma once

#include <string>
#include <stdint.h>
#include <functional>

namespace Common {

void printStacktrace();
void printStacktraceNoLogger();

/**
 * @brief setupBacktrace    Set processor for common application error signal handling
 * @param signalProcessor   If processor proceed signal, must return true
 * @note    Common usage - stop application to exit gracefully
 */
void setupBacktrace(std::function<bool(int)>&& signalProcessor);
void setupBacktrace();

/**
 * @brief restartSelf Function used to restart the binary completely
 */
void restartSelf();

/**
 * @brief terminalGotoXY    Move cursor in terminal to coordinates
 * @param x
 * @param y
 */
void terminalGotoXY(int x,int y);

/**
 * @brief terminalGetXY Get cursor coordinates in terminal
 * @return
 * @note Not tested properly
 */
std::pair<unsigned, unsigned> terminalGetXY();

/**
 * @brief createRandomNumber    Generates number using MT19937
 * @param min
 * @param max
 * @return
 * @throws std::invalid_argument if min >= max
 */
int createRandomNumber(int min, int max);

/**
 * @brief createRandomString    Generates string from a-z, A-Z, 0-9
 * @param stringSize            Length of result
 * @return
 */
std::string createRandomString(unsigned int stringLength);

/**
 * @brief getEpoch Get epoch time in seconds (since 1 Jan 1970)
 * @return
 */
uint64_t getEpoch();

/**
 * @brief getCurrentTimestampFormatted  Get timestamp in specified format
 * @param tsFmt                         Format of a timestamp
 * @return
 * @note Format of tsFmt must be compatible to std::put_time (for example, "%Y-%m-%d %H:%M:%S")
 */
std::string getCurrentTimestampFormatted(const std::string& tsFmt = "%Y-%m-%d %H:%M:%S");
std::string getCurrentTimestampFilename(bool containTime);

}
