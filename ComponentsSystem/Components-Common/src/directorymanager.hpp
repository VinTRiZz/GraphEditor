#pragma once

#include <Components/Logger/Logger.h>

#include <map>
#include <filesystem>

namespace Common {

/**
 * @brief The DirectoryType enum Directory types to get from manager
 */
enum DirectoryType : int
{
    Config = 0,     // .ini, .conf, etc.
    Data,           // Databases, resources, etc.
    Logs,           // App logs
    Plugins,        // App plugins
    Backup,         // Data, config backups, if required
    Temporary,      // Cache

    UserDefined = 100, // User-defined directories to register
};

/**
 * @brief The DirectoryManager class Directory manager to handle app data
 */
class DirectoryManager : public boost::noncopyable
{
protected:

public:
    static DirectoryManager& getInstance();

    /**
     * @brief setRootPath Set path of application data root
     * @param rootPath
     */
    void setRootPath(const std::filesystem::path& rootPath);

    /**
     * @brief getRootPath Get path of application data root
     * @return
     */
    std::filesystem::path getRootPath() const;

    /**
     * @brief init  Create subdirectories if not exist
     * @return      true if no error
     */
    bool init();

    /**
     * @brief getDirectory  Get directory, registered in manager
     * @param dtype         @ref DirectoryType enum or custom value
     * @return              Directory handle
     */
    std::filesystem::path getDirectory(int dtype) const;

    /**
     * @brief getDirectoryStatic    Static equ of @ref getDirectory()
     * @param dtype
     * @return
     */
    static std::filesystem::path getDirectoryStatic(int dtype);

    /**
     * @brief registerDirectory Зарегистрировать пользовательский тип директории
     * @param dtype             Тип директории
     * @param dirp              Путь до директории
     * @exception               Если dtype меньеш DirectoryType::UserDefined, исключение std::invalid_argument
     */
    void registerDirectory(int dtype, const std::filesystem::path& dirp);

private:
    std::filesystem::path                   m_rootdir;
    std::map<int, std::filesystem::path>    m_dirPaths;

    bool isDirectoryWritable(const std::filesystem::path& p) const;
};

} // namespace Common
