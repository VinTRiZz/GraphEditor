#include "filewriter.hpp"

#ifdef COMPONENTS_IS_ENABLED_QT

namespace LoggerQt
{

void FileWriter::setLogfile(const std::string &logfilePath)
{
    m_logfile.setFileName(logfilePath.c_str());
    m_logfile.open(QIODevice::Append | QIODevice::Truncate);
    FileWriterBase::setLogfile(logfilePath);
}

}

#endif // COMPONENTS_IS_ENABLED_QT
