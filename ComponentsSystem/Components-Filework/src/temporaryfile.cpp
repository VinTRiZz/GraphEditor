#include "temporaryfile.hpp"

#include <filesystem>
#include <random>
#include <algorithm>

namespace Filework
{

static const auto OPENFLAGS {std::ios_base::trunc | std::ios_base::in | std::ios_base::out | std::ios_base::binary};

std::string TemporaryFile::createRandomString(unsigned int stringLength) {
    std::random_device rd;
    std::mt19937 gen(rd());

    const std::string characters =
        "0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz";
    std::uniform_int_distribution<> dis(0, characters.size() - 1);

    std::string result;
    result.reserve(stringLength);
    std::generate_n(std::back_inserter(result), stringLength, [&](){
        return characters[dis(gen)];
    });

    return result;
}

TemporaryFile::TemporaryFile(const std::string &targetFile, bool copyContents) noexcept(false)
{
    init();
    setTargetFile(targetFile, copyContents);
}

TemporaryFile::TemporaryFile() noexcept(false) {
    init();
}

TemporaryFile::~TemporaryFile() {

}

bool TemporaryFile::setTargetFile(const std::string &targetFile, bool copyContents)
{
    m_targetFilePath = targetFile;
    if (m_targetFilePath.empty()) {
        // Reset file path is allowed
        wipeData();
        return true;
    }
    auto tgtFilePath = std::filesystem::path(m_targetFilePath);

    const bool isNewFile = !std::filesystem::exists(tgtFilePath);
    const bool isDirectory = std::filesystem::is_directory(tgtFilePath);

    auto expectedRights = (std::filesystem::perms::owner_read | std::filesystem::perms::owner_write);
    const bool isEditable = expectedRights == (expectedRights & std::filesystem::status(tgtFilePath).permissions());

    if (isDirectory || (!isNewFile && !isEditable)) {
        throw std::runtime_error(
            std::string("Invalid target file:") +
            (isNewFile ? " new" : " existing") +
            (isEditable ? " ReadWrite" : " not ReadWrite") +
            (isDirectory ? " directory" : " file")
            );
    }
    if (copyContents && !m_targetFilePath.empty() && std::filesystem::exists(m_targetFilePath)) {
        std::filesystem::copy_file(m_targetFilePath, m_tmpFilePath, std::filesystem::copy_options::overwrite_existing);
    }
    return true;
}

void TemporaryFile::setSecureMode(bool isEnabled)
{
    m_isSecureMode = isEnabled;
}

void TemporaryFile::accept() noexcept(false) {
    if (m_targetFilePath.empty()) { return; }
    m_file.flush();
    m_file.close();
    std::filesystem::remove(m_targetFilePath);
    std::filesystem::rename(m_tmpFilePath, m_targetFilePath);
    openTmpFile();
}

std::string_view TemporaryFile::getFilename() const
{
    return m_tmpFilePath;
}

std::string_view TemporaryFile::getTargetFilename() const
{
    return m_targetFilePath;
}

bool TemporaryFile::seekg(std::size_t pos, const std::ios_base::seekdir& moveDir)
{
    if (!m_file.is_open()) {
        return false;
    }
    m_file.seekg(pos, moveDir);
    return true;
}

std::size_t TemporaryFile::tellg()
{
    return m_file.tellg();
}

bool TemporaryFile::eof() const
{
    return m_file.eof();
}

bool TemporaryFile::bad() const
{
    return m_file.bad();
}

bool TemporaryFile::fail() const
{
    return m_file.fail();
}

void TemporaryFile::clearBadbit()
{
    m_file.clear();
}

void TemporaryFile::flush()
{
    m_file.flush();
}

std::streamsize TemporaryFile::readsome(char *output, const std::streamsize byteCount)
{
    return m_file.readsome(output, byteCount);
}

TemporaryFile &TemporaryFile::write(const char *input, const std::streamsize byteCount) {
    m_file.write(input, byteCount);
    return *this;
}

bool TemporaryFile::empty() const
{
    return (0 == size());
}

std::size_t TemporaryFile::size() const
{
    return std::filesystem::file_size(m_tmpFilePath);
}

void TemporaryFile::clearFileData() noexcept(false)
{
    wipeData();
    openTmpFile();
}

void TemporaryFile::init() noexcept(false)
{
    m_fileDeleter = {reinterpret_cast<bool*>(1),
        [this](auto*) {
            wipeData();
            if (m_file.is_open()) {
                m_file.close();
            }
            if (std::filesystem::exists(m_tmpFilePath)) {
                std::filesystem::remove(m_tmpFilePath);
            }
        }
    };
    wipeData();
    openTmpFile();
}

void TemporaryFile::wipeData()
{
    if (!std::filesystem::exists(m_tmpFilePath)) {
        return;
    }

    if (!m_isSecureMode) {
        if (m_file.is_open()) {
            m_file.close();
        }
        std::filesystem::resize_file(m_tmpFilePath, 0);
        return;
    }

    openTmpFile();
    const auto BUFF_SIZE {4096};
    char buffer[BUFF_SIZE];
    std::fill_n(buffer, BUFF_SIZE, '\0');
    for (std::size_t fSize = 0; fSize < this->size(); fSize+=BUFF_SIZE) {
        m_file.write(buffer, BUFF_SIZE);
        m_file.flush();
    }
    m_file.close();

    std::filesystem::resize_file(m_tmpFilePath, 0);
}

void TemporaryFile::openTmpFile() noexcept(false)
{
    if (m_file.is_open()) { return; }
    try {
        if (std::filesystem::exists(m_tmpFilePath)) {
            m_file.open(m_tmpFilePath, OPENFLAGS & ~std::ios_base::trunc);
        } else {
            m_file.open(m_tmpFilePath, OPENFLAGS);
        }
    } catch (const std::ios_base::failure& e) {
        throw std::runtime_error(std::string("Temporary file open error: ") + e.what());
    }
}

}
