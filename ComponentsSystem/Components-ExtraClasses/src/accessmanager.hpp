#pragma once

#include <thread>
#include <mutex>
#include <atomic>
#include <functional>
#include <vector>
#include <deque>

#include "utility/signaldecorator.hpp"

namespace ExtraClasses {

/**
 * @brief The AccessManager class Класс, реализующий решение проблемы "много читателей - один писатель"
 */
class AccessManager : protected SignalDecorator {
public:

    class ReaderResultAwaiter {
        std::shared_ptr<SignalDecorator> pResultSig;
    public:
        ReaderResultAwaiter() = delete;
        explicit ReaderResultAwaiter(const std::shared_ptr<SignalDecorator>& resSig);
        void wait();
    };

    // Для удобства работы с типом
    using accessFunction_t = std::function<void(void)>;

    AccessManager();
    ~AccessManager();

    /**
     * @brief addReader Добавить функтор-читатель
     * @param rder
     */
    ReaderResultAwaiter addReader(accessFunction_t&& rder);

    /**
     * @brief addWriter Добавить функтор-писатель. Запускает при первой же возможности
     * @param wrter
     * @note Перезаписывает ранее заданный, если он не был вызван
     */
    ReaderResultAwaiter addWriter(accessFunction_t&& wrter);

    /**
     * @brief start         Запуск менеджера
     * @param threadCount   Кол-во потоков. Передайте 0 для задания оптимального для ЦП числа
     */
    void start(unsigned threadCount = 0);

    /**
     * @brief stop  Остановить менеджер. Остановка не мгновенная, после выполнения записи и после - чтения
     * @note Если менеджер уже остановлен, ничего не делает
     */
    void stop();

    /**
     * @brief isWorking Проверка, что менеджер запущен
     * @return          true если менеджер ожидает / выполняет задачи
     */
    bool isWorking() const;

private:
    // "писатель"
    std::atomic<bool>       m_isWriterInited {false};
    std::recursive_mutex    m_writerMx;
    accessFunction_t        m_writer;

    // "читатели"
    std::deque<accessFunction_t>    m_readers;
    std::mutex                      m_readersMx;

    // Обработка потоками и их завершение
    std::vector <std::thread>   m_threads;
    std::atomic<bool>           m_isWorking {false};
};

}
