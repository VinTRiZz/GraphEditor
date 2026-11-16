#pragma once

/**
 * @file instancebase.hpp Файл с определением базового класса для логгера
 */

#if __has_include(<boost/noncopyable.hpp>)
#include <boost/noncopyable.hpp>
#endif // has <boost/noncopyable.hpp>

#include <memory>
#include <functional>
#include <string>

#include "templates/helper_functions.hpp"

namespace Logger {

/**
 * @brief InstanceBase Базовый класс инстанции логгера
 */
#if __has_include(<boost/noncopyable.hpp>)
class InstanceBase : public boost::noncopyable
#else
class InstanceBase
#endif
{
#if  !__has_include(<boost/noncopyable.hpp>)
    InstanceBase(const InstanceBase&) = delete;
    InstanceBase(InstanceBase&&) = delete;
    InstanceBase& operator =(const InstanceBase&) = delete;
    InstanceBase& operator =(InstanceBase&&) = delete;
#endif // has <boost/noncopyable.hpp>

protected:
    InstanceBase();

public:
    ~InstanceBase();

    /**
     *  @brief createInstance   Создать независимую инстанцию логгера
     *  @param logfileDir       Директория для сохранения логфайлов
     *  @note                   Желательно не указывать ту же директорию, что у глобальной инстанции (это не регулируется)
     */
    template <typename DerivedInstanceT>
    static std::shared_ptr<DerivedInstanceT> createInstance(const std::string &logfileDir) {
        auto inst = std::make_shared<DerivedInstanceT>();
        if (!logfileDir.empty()) {
            inst->callInit(logfileDir);
        }
        return inst;
    }

    /**
     *  @brief createInstance   Запросить глобальную инстанцию логгера
     */
    template <typename DerivedInstanceT>
    static DerivedInstanceT& getInstance() {
        static DerivedInstanceT inst;
        return inst;
    }

    /**
     *  @brief createInstance   Запросить глобальную инстанцию логгера с инициализацией
     *  @param logfileDir       Директория для сохранения логфайлов. При NULL std::string игнорируется
     */
    template <typename DerivedInstanceT>
    static DerivedInstanceT& getInstance(const std::string &logfileDir) {
        auto& inst = getInstance<DerivedInstanceT>();
        if (!logfileDir.empty()) {
            inst.callInit(logfileDir);
        }
        return inst;
    }

private:
    struct Impl;
    std::unique_ptr<Impl> d;

    void callInit(const std::string &logfileDir);

protected:
    virtual void init(const std::string& logfileDir) = 0;
    void deinit();

    using task_t = std::function<void()>;
    void addTask(task_t&& tsk);
    void addTaskSync(task_t&& tsk);
};

} // namespace Logger
