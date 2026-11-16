#include "signaldecorator.hpp"

namespace ExtraClasses
{

void SignalDecorator::waitSignal() {
    std::unique_lock<std::mutex> lock(m_waitMx);
    m_signalCv.wait(lock);
}

void SignalDecorator::emitSignal() {
    std::unique_lock<std::mutex> lock(m_waitMx);
    m_signalCv.notify_one();
}

void SignalDecorator::emitForAll() {
    std::unique_lock<std::mutex> lock(m_waitMx);
    m_signalCv.notify_all();
}

}
