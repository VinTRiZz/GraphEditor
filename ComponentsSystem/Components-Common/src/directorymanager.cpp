#include "directorymanager.hpp"

namespace Common {


DirectoryManager &DirectoryManager::getInstance() {
    static DirectoryManager inst;
    return inst;
}

bool DirectoryManager::init() {
    std::filesystem::create_directory(std::filesystem::current_path() / m_rootdir);
    auto curdir = std::filesystem::path(m_rootdir);
    if (    !std::filesystem::exists(curdir) ||
        !std::filesystem::is_directory(curdir) ||
        !isDirectoryWritable(curdir)) {
        std::cerr << "Root dir:       " << m_rootdir << std::endl;
        std::cerr << "Current dir:    " << std::filesystem::current_path() << std::endl;
        std::cerr << "Invalid directory (not exist or not readable)";
        return false;
    }

    for (auto& [dirtype, dirpath] : m_dirPaths) {
        if (std::filesystem::exists(dirpath.generic_string())) {
            std::cout << "[  OK  ] DirectoryManager check: Directory exist. Path: " << dirpath << std::endl;
            continue;
        }

        if (!std::filesystem::create_directory(dirpath)) {
            std::cerr << std::string(std::string("DirectoryManager: Error creating directory. Path: ") + dirpath.generic_string());
            return false;
        }
        std::cout << "[  OK  ] DirectoryManager check: created directory. Path: " << dirpath << std::endl;
    }
    return true;
}

void DirectoryManager::setRootPath(const std::filesystem::path &rootPath) {
    m_rootdir = rootPath.wstring();
    registerDirectory(DirectoryType::Config,    m_rootdir / "config");
    registerDirectory(DirectoryType::Data,      m_rootdir / "data");
    registerDirectory(DirectoryType::Logs,      m_rootdir / "log");
    registerDirectory(DirectoryType::Plugins,   m_rootdir / "plugins");
    registerDirectory(DirectoryType::Backup,    m_rootdir / "backup");
    registerDirectory(DirectoryType::Temporary, m_rootdir / "tmp");
    init();
}

std::filesystem::path DirectoryManager::getRootPath() const
{
    return m_rootdir;
}

std::filesystem::path DirectoryManager::getDirectory(int dtype) const {
    auto targetIt = m_dirPaths.find(dtype);
    if (targetIt == m_dirPaths.end()) {
        return {};
    }
    return targetIt->second;
}

std::filesystem::path DirectoryManager::getDirectoryStatic(int dtype) {
    auto& inst = getInstance();
    return inst.getDirectory(dtype);
}

void DirectoryManager::registerDirectory(int dtype, const std::filesystem::path &dirp) {
    m_dirPaths[dtype] = dirp;
}

bool DirectoryManager::isDirectoryWritable(const std::filesystem::path &p) const
{
    auto perms = std::filesystem::status(p).permissions();
    return (perms & std::filesystem::perms::owner_write) != std::filesystem::perms::none; // TODO: Check work
}

} // namespace Common
