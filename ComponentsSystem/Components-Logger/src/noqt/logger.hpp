#pragma once
#ifndef COMPONENTS_IS_ENABLED_QT

#include <iostream>

#include "../instancebase.hpp"
#include "filewriter.hpp"

namespace LoggerNoQt {
using namespace Logger;

/**
 * @brief The Instance class Мастер вывода информации (логов). Синглетон
 */
class Instance final : public InstanceBase {

public:
    ~Instance();

    /**
     * @brief log Вывести данные в потоке логгирования. Для синхронного вывода
     * укажите isSync как true
     * @param args Данные на вывод
     */
    template<Level lt, bool isSync, typename... Args>
    void log(Args&&... args) {
        auto task = [=, this]() {
            auto timestamp = getTimestamp();
            if constexpr (lt != Level::Empty && lt != Level::Error && lt != Level::Warning) {
                writeData(std::cout, timestamp + " [" + createLogtypeColoredString<lt>() + "] ");
            }

            if constexpr (lt == Level::Error || lt == Level::Warning) {
                writeData(std::cerr, timestamp + " [" + createLogtypeColoredString<lt>() + "] ");
                (writeData(std::cerr, args), ...);
            } else {
                (writeData(std::cout, args), ...);
            }

            if constexpr (lt != Level::Empty) {
                m_logfileWriter.log<lt>(timestamp + " [" + createLogtypeString<lt>() + "] ", args...);
            } else {
                m_logfileWriter.log<lt>(args...);
            }

            if constexpr (lt == Level::Error || lt == Level::Warning) {
                std::endl(std::cerr);
            } else {
                std::endl(std::cout);
            }
        };

        if constexpr (isSync) {
            addTaskSync(std::move(task));
        } else {
            addTask(std::move(task));
        }
    }

    FileWriter& getFilewriter();
    
private:
    void init(const std::string& logfileDir) override;
    FileWriter m_logfileWriter; //! Мастер записи данных в файл
};

}

#endif // COMPONENTS_IS_ENABLED_QT
