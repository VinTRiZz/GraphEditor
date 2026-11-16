#pragma once

#ifdef COMPONENTS_IS_ENABLED_QT

#include <condition_variable>
#include <functional>
#include <list>
#include <mutex>
#include <thread>

#include <QDir>
#include <QTextStream>
#include <QFile>
#include <QPoint>

#include "../common.hpp"
#include "../filewriterbase.hpp"

#include <iostream>


namespace LoggerQt
{
using namespace Logger;

class FileWriter final : public FileWriterBase
{
    QFile m_logfile;                            //! Логфайл
    QTextStream m_logfileStream{&m_logfile};    //! Поток ввода в файл данных

    template <typename T>
    void writeToFile(const T& v) {
        m_logfileStream << v << " ";
    }

    void addEndline();

public:
    void setLogfile(const std::string& logfilePath) override;

    /**
    * @brief log Вывести данные в потоке логгирования. Для синхронного вывода
    * укажите isSync как true
    * @param args Данные на вывод
    */
    template<Level lt, typename... Args>
    void log(Args&&... args) {
        if (getLogfilePath().empty()) { return; }
        lockFile();
        if (!m_logfile.isOpen()) {
            throw std::runtime_error(
                        std::string("Error opening logfile (logfile path: ") +
                        getLogfilePath().data() + ")");
        }
        (writeToFile(args), ...);
        m_logfileStream << Qt::endl;
        m_logfile.flush();
        unlockFile();
    }
};

template <>
inline void FileWriter::writeToFile(const std::string& v) {
    m_logfileStream << v.c_str() << " ";
}

template <>
inline void FileWriter::writeToFile(const QVariant& v) {
    m_logfileStream << "QVariant(" << (v.isNull() ? "NULL" : (v.typeName() + v.toString())) << ")" << " ";
}

template <>
inline void FileWriter::writeToFile(const QPoint& v) {
    m_logfileStream << "{" << v.x() << "; " << v.y() << "} ";
}

template <>
inline void FileWriter::writeToFile(const QPointF& v) {
    m_logfileStream << "{" << v.x() << "; " << v.y() << "} ";
}

}

#endif // COMPONENTS_IS_ENABLED_QT
