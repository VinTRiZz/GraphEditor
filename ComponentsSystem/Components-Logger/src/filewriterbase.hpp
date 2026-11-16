#pragma once

#include <string>
#include <memory>

#include "templates/helper_functions.hpp"

namespace Logger
{

/**
 * @brief The FileWriterBase class  Базовый класс для записывающей в файл части логгера
 */
class FileWriterBase
{
public:
    FileWriterBase();
    ~FileWriterBase();

    virtual void setLogfile(const std::string& filePath);
    virtual void setLogfile(const std::string_view& filePath);
    std::string_view getLogfilePath() const;

private:
    struct Impl;
    std::unique_ptr<Impl> d;
    std::string m_logfilePath;

protected:
    void lockFile();
    void unlockFile();
};

}
