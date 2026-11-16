#pragma once

#include <string>
#include <memory>
#include <functional>
#include <fstream>

namespace Filework
{

/**
 * @brief The TemporaryFile class   Simple temporary file handling object, temp file removed in destructor
 */
class TemporaryFile
{
    TemporaryFile(TemporaryFile&&) = delete;
    TemporaryFile(const TemporaryFile&) = delete;
    TemporaryFile& operator=(const TemporaryFile&) = delete;
    TemporaryFile& operator=(TemporaryFile&&) = delete;

public:

    // Constructor-set of a target has no warranty,
    // that file will be set
    TemporaryFile(const std::string& targetFile, bool copyContents = true) noexcept(false);

    TemporaryFile() noexcept(false);
    ~TemporaryFile();

    /**
     * @brief setTargetFile Set file to save data into on accept()
     * @param targetFile    Target file path
     * @return              true if target exist and writable or creatable
     */
    bool setTargetFile(const std::string& targetFile, bool copyContents = true);

    /**
     * @brief setSecureMode   Enable security features to safely write data
     * @param isEnabled
     * @note Mostly for security purposes
     */
    void setSecureMode(bool isEnabled);

    /**
     * @brief accept Write changes into a target file
     * @throws Same errors as std::filesystem::copy()
     */
    void accept() noexcept(false);

    /**
     * @brief getFilename   Get path to a temporary file
     * @return
     */
    std::string_view getFilename() const;

    /**
     * @brief getTargetFilename Get path to a target file
     * @return
     */
    std::string_view getTargetFilename() const;

    // std::fstream interface copy
    bool seekg(std::size_t pos, const std::ios_base::seekdir &moveDir = std::ios_base::cur);
    std::size_t tellg();
    bool eof() const;
    bool bad() const;
    bool fail() const;
    void clearBadbit();
    void flush();
    std::streamsize readsome(char* output, const std::streamsize byteCount);
    TemporaryFile& write(const char* input, const std::streamsize byteCount);

    template <typename InputT>
    TemporaryFile& operator <<(InputT&& val) {
        m_file << val;
        return *this;
    }

    template <typename OutputT>
    TemporaryFile& operator >>(OutputT& val) {
        m_file >> val;
        return *this;
    }

    /**
     * @brief empty Return true if file size is 0
     * @return
     */
    bool empty() const;

    /**
     * @brief size Return size of file
     * @return
     */
    std::size_t size() const;

    /**
     * @brief clearFileData Wipes all data written to file (reopen it using TRUNCATE)
     */
    void clearFileData() noexcept(false);

private:
    std::string     m_targetFilePath    {};
    std::string     m_tmpFilePath       {createRandomString(16)};
    bool            m_isSecureMode      {false};
    std::fstream    m_file;
    std::unique_ptr<bool, std::function<void(bool*)> > m_fileDeleter; // Deletes file

    // Utility
    static std::string createRandomString(unsigned int stringLength);

    void init() noexcept(false);
    void wipeData();
    void openTmpFile() noexcept(false);
};

}
