#include "logger.hpp"

#ifndef COMPONENTS_IS_ENABLED_QT

#include <filesystem>

namespace LoggerNoQt {

Instance::~Instance()
{
    deinit();
}

FileWriter &Instance::getFilewriter()
{
    return m_logfileWriter;
}

void Instance::init(const std::string &logfileDir)
{
    m_logfileWriter.setLogfile(logfileDir + std::filesystem::path::preferred_separator + createLogfileName());
}

}  // namespace Logging

#endif // COMPONENTS_IS_ENABLED_QT
