#include "filewriterbase.hpp"

#include <mutex>
#include <filesystem>

namespace Logger
{

struct FileWriterBase::Impl
{
    std::mutex writeMx;
};

FileWriterBase::FileWriterBase() :
    d {new Impl}
{

}

FileWriterBase::~FileWriterBase()
{

}

void FileWriterBase::setLogfile(const std::string &filePath)
{
    m_logfilePath = std::filesystem::absolute(filePath);
}

void FileWriterBase::setLogfile(const std::string_view &filePath)
{
    m_logfilePath = std::filesystem::absolute(std::string(filePath));
}

std::string_view FileWriterBase::getLogfilePath() const
{
    return m_logfilePath;
}

void FileWriterBase::lockFile()
{
    d->writeMx.lock();
}

void FileWriterBase::unlockFile()
{
    d->writeMx.unlock();
}

}
