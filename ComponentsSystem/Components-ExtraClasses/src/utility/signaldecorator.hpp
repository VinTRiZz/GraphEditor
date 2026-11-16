#pragma once

#include <mutex>
#include <condition_variable>

namespace ExtraClasses
{

/**
 * @brief The SignalDecorator class Добавляет функционал ожидания сигнала в класс
 */
class SignalDecorator
{
public:
    void waitSignal();
    void emitSignal();
    void emitForAll();

private:
    std::condition_variable m_signalCv;
    std::mutex              m_waitMx;
};

}
