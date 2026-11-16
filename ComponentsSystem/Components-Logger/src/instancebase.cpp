#include "instancebase.hpp"

#include <condition_variable>
#include <functional>
#include <deque>
#include <mutex>
#include <thread>
#include <atomic>
#include <future>

#include <filesystem>

namespace Logger {

struct InstanceBase::Impl {
    std::atomic<bool>       isWorking {false};
    std::future<void>       threadFut;
    std::deque<task_t>      taskDeq;
    std::condition_variable notifyCV;
    std::mutex              notifyMx;
    std::mutex              outputMx; // Just for printing in right order
};

InstanceBase::InstanceBase() :
    d {new Impl}
{
    d->isWorking.store(true, std::memory_order_release);
    std::packaged_task<void()> task([this]() {
        task_t nextTask;

        while (d->isWorking.load(std::memory_order_acquire)) {
            std::unique_lock<std::mutex> lock(d->notifyMx);
            while (!d->taskDeq.empty()) {
                nextTask = d->taskDeq.front();
                d->taskDeq.pop_front();
                lock.unlock();

                {
                    std::lock_guard<std::mutex> lockg(d->outputMx);
                    nextTask();
                }

                lock.lock();
            }
            d->notifyCV.wait(lock, [this]() -> bool {
                // Spurious wakeup workaround
                return !d->taskDeq.empty() || !d->isWorking.load(std::memory_order_acquire);
            });
        }
    });
    d->threadFut = task.get_future();
    std::thread(std::move(task)).detach();
}

InstanceBase::~InstanceBase()
{
    if (d->threadFut.valid()) {
        deinit();
    }
}

void InstanceBase::callInit(const std::string &logfileDir)
{
    this->init(logfileDir);
}

void InstanceBase::deinit()
{
    d->isWorking.store(false, std::memory_order_release);
    while (d->threadFut.wait_for(std::chrono::microseconds(1)) != std::future_status::ready) {
        std::unique_lock<std::mutex> lock(d->notifyMx);
        d->notifyCV.notify_one();
    }

    for (auto& task : d->taskDeq) {
        task();
    }
    d->taskDeq.clear();
}

void InstanceBase::addTask(task_t &&tsk)
{
    std::unique_lock<std::mutex> lock(d->notifyMx);
    d->taskDeq.emplace_back(std::move(tsk));
    d->notifyCV.notify_one();
}

void InstanceBase::addTaskSync(task_t &&tsk)
{
    std::lock_guard<std::mutex> lock(d->outputMx);
    tsk();
}

} // namespace Logger
