#include "accessmanager.hpp"

#include <Components/Logger/Logger.h>

namespace ExtraClasses
{

AccessManager::ReaderResultAwaiter::ReaderResultAwaiter(const std::shared_ptr<SignalDecorator>& resSig) :
    pResultSig {resSig}
{

}

void AccessManager::ReaderResultAwaiter::wait()
{
    if (pResultSig) {
        pResultSig->waitSignal();
    }
}



AccessManager::AccessManager()
{

}

AccessManager::~AccessManager() {
    if (isWorking()) {
        stop();
    }
}

AccessManager::ReaderResultAwaiter AccessManager::addReader(accessFunction_t &&rder) {
    std::lock_guard<std::mutex> lock(m_readersMx);

    auto pOperationSignaller = std::make_shared<SignalDecorator>();
    AccessManager::ReaderResultAwaiter res(pOperationSignaller);
    m_readers.emplace_back([rder_l = std::move(rder), pOperationSignaller](){
        rder_l();
        pOperationSignaller->emitSignal();
    });

    emitSignal();
    return res;
}

AccessManager::ReaderResultAwaiter AccessManager::addWriter(accessFunction_t &&wrter) {
    std::lock_guard<std::recursive_mutex> lock(m_writerMx);

    auto pOperationSignaller = std::make_shared<SignalDecorator>();
    AccessManager::ReaderResultAwaiter res(pOperationSignaller);
    m_isWriterInited.store(bool(wrter));
    m_writer = [wrter_l = std::move(wrter), pOperationSignaller](){
        wrter_l();
        pOperationSignaller->emitSignal();
    };

    emitSignal();
    return res;
}

void AccessManager::start(unsigned int threadCount) {
    m_isWorking.store(true);
    auto rtcnt = (threadCount > 0 ? threadCount : std::thread::hardware_concurrency());

    m_threads.reserve(rtcnt);
    for (int i = 0; i < rtcnt; ++i) {
        m_threads.emplace_back([this](){
            accessFunction_t nextTask {};
            while (isWorking()) {
                if (!isWorking()) [[unlikely]] {
                    return;
                }

                m_writerMx.lock();
                if (m_isWriterInited.load()) [[unlikely]]  {
                    m_writer();
                    m_writer = {};
                    m_isWriterInited.store(false);
                }
                m_writerMx.unlock();

                if (!isWorking()) [[unlikely]] {
                    return;
                }

                m_readersMx.lock();
                if (!m_readers.empty()) [[likely]]  {
                    nextTask = m_readers.front();
                    m_readers.pop_front();
                } else {
                    nextTask = {};
                }
                m_readersMx.unlock();

                if (nextTask) [[likely]]  {
                    std::lock_guard<std::recursive_mutex> lock(m_writerMx);
                    nextTask();
                } else {
                    waitSignal();
                }
            }
        });
    }
}

void AccessManager::stop() {
    m_isWorking.store(false);

    bool isStopped {false};
    while (!isStopped) {
        emitForAll();
        m_writerMx.lock();
        isStopped = (!m_writer && m_readers.empty());
        m_writerMx.unlock();
    }

    for (auto& th : m_threads) {
        if (th.joinable()) {
            emitForAll();
            th.join();
        }
    }
    m_threads.clear();
}

bool AccessManager::isWorking() const {
    return m_isWorking.load();
}

}
