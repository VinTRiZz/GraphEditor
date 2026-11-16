#pragma once

#ifndef COMPONENTS_IS_ENABLED_QT

#include <fstream>

#include "../common.hpp"
#include "../filewriterbase.hpp"

namespace LoggerNoQt
{

using namespace Logger;

class FileWriter final : public FileWriterBase
{
public:
    using FileWriterBase::FileWriterBase;

    /**
    * @brief log Вывести данные в потоке логгирования. Для синхронного вывода
    * укажите isSync как true
    * @param args Данные на вывод
    */
    template<Level lt, typename... Args>
    void log(Args&&... args) {
        if (getLogfilePath().empty()) { return; }
        lockFile();
        m_logfile.open(getLogfilePath().data(), std::ios_base::out | std::ios_base::app);
        if (!m_logfile.is_open()) {
            throw std::runtime_error(
                        std::string("Error opening logfile (logfile path: ") +
                        getLogfilePath().data() + ")");
        }
        (writeData(m_logfile, args), ...);
        m_logfile << std::endl;

        m_logfile.flush();
        m_logfile.close();
        unlockFile();
    }

private:
    std::fstream m_logfile;
};

}

#endif // COMPONENTS_IS_ENABLED_QT
