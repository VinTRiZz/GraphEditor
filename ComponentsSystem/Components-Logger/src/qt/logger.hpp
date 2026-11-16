#pragma once
#ifdef COMPONENTS_IS_ENABLED_QT

#include <QDebug>

#include "../instancebase.hpp"
#include "filewriter.hpp"

namespace LoggerQt {
using namespace Logger;

/**
 * @brief The Instance class Мастер вывода информации (логов). Синглетон
 */
class Instance : public InstanceBase {
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
            QDebug dbgStream((QIODevice*)nullptr);
            if constexpr (lt == Level::Error || lt == Level::Warning) {
                dbgStream = qCritical();
            } else {
                dbgStream = qDebug();
            }

            if constexpr (lt != Level::Empty && lt != Level::Error && lt != Level::Warning) {
                writeData(dbgStream, timestamp + " [" + createLogtypeColoredString<lt>() + "] ");
            }
            if constexpr (lt == Level::Error || lt == Level::Warning) {
                writeData(dbgStream, timestamp + " [" + createLogtypeColoredString<lt>() + "] ");
                (writeData(dbgStream, args), ...);
            } else {
                (writeData(dbgStream, args), ...);
            }

            if constexpr (lt != Level::Empty) {
                m_logfileWriter.log<lt>(timestamp + " [" + createLogtypeString<lt>() + "] ", args...);
            } else {
                m_logfileWriter.log<lt>(args...);
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
    FileWriter m_logfileWriter; //! Мастер записи данных в файл

    // InstanceBase interface
    void init(const std::string &logfileDir) override;
};

}

#endif // COMPONENTS_IS_ENABLED_QT
