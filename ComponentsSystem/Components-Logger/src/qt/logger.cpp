#include "logger.hpp"

#ifdef COMPONENTS_IS_ENABLED_QT

#include <filesystem>

namespace LoggerQt {

Instance::~Instance()
{
    deinit();
}

LoggerQt::FileWriter &LoggerQt::Instance::getFilewriter()
{
    return m_logfileWriter;
}

void Instance::init(const std::string &logfileDir)
{
    m_logfileWriter.setLogfile(logfileDir + QDir::separator().toLatin1() + createLogfileName());
}

}  // namespace Logging

#endif // COMPONENTS_IS_ENABLED_QT

