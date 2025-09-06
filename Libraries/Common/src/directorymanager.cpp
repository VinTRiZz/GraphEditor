#include "directorymanager.h"

#include <QDebug>
#include <QDir>

template <typename MapT, typename TypeT>
QString DirectoryManager::getDirectoryPath(const MapT& typeMap, TypeT dtype, bool withNativeSeparator) {
    if (!typeMap.count(dtype)) {
        throw std::invalid_argument(
            std::string("DirectoryManager: directory init failure! Type: ") +
            std::to_string(static_cast<int>(dtype)));
    }

    if (dtype == TypeT::Root) {
        return (withNativeSeparator
                    ? typeMap.at(dtype) + QDir::separator()
                    : typeMap.at(dtype));
    }

    return getDirectoryPath(typeMap, TypeT::Root, false) + QDir::separator() +
           (withNativeSeparator
                ? typeMap.at(dtype) + QDir::separator()
                : typeMap.at(dtype));
}

DirectoryManager::DirectoryManager(const QString& rootdir) {
    checkup(rootdir);
}

DirectoryManager& DirectoryManager::getInstance(const QString& rootdir) {
    static DirectoryManager inst(rootdir);
    return inst;
}

QString DirectoryManager::getDocumentsPath() {
    return QDir::homePath() + QDir::separator() +
           "Documents";  // TODO: Добавить для Windows
}

QDir DirectoryManager::getSystemDirectory(DirectoryTypeSystem dtype) {
    return QDir(getSystemDirectoryPath(dtype));
}

QString DirectoryManager::getSystemDirectoryPath(DirectoryTypeSystem dtype, bool withNativeSeparator)
{
    auto& inst = getInstance();
    return getDirectoryPath(inst.m_systemDirectoryPaths, dtype, withNativeSeparator);
}

QDir DirectoryManager::getTmpDirectory(DirectoryTypeTmp dtype) {
    return QDir(getTmpDirectoryPath(dtype));
}

QString DirectoryManager::getTmpDirectoryPath(DirectoryTypeTmp dtype, bool withNativeSeparator)
{
    auto& inst = getInstance();
    return getDirectoryPath(inst.m_tmpDirectoryPaths, dtype, withNativeSeparator);
}


void DirectoryManager::checkup(const QString& rootdir) {
    checkupSystem(rootdir);
    checkupTmp();
}

template <typename MapT>
void DirectoryManager::createDirectories(const MapT& dirsMap, const QString& rootdir, const QString& dirsType) {
    auto curdir = QDir::current();
    if (!QDir(rootdir).exists()) {
        if (!curdir.mkdir(rootdir)) {
            throw std::domain_error(
                std::string("DirectoryManager check: Error creating root directory. Type: ") + dirsType.toStdString());
        }
        qDebug() << "[  OK  ] DirectoryManager check: Root dir created. Type:" << dirsType;
    } else {
        qDebug() << "[  OK  ] DirectoryManager check: Root dir exist. Type:" << dirsType;
    }
    curdir.cd(rootdir);

    for (auto& [dirtype, dirpath] : dirsMap) {
        if (dirtype == MapT::key_type::Root) {
            continue;
        }

        if (QDir(rootdir + QDir::separator() + dirpath).exists()) {
            qDebug() << "[  OK  ] DirectoryManager check: Directory exist. Type:" << dirsType << "Path:"
                     << dirpath;
            continue;
        }

        if (!curdir.mkdir(dirpath)) {
            throw std::domain_error(
                std::string("DirectoryManager: Error creating directory. Type: ") + dirsType.toStdString() + "; Path: " +
                dirpath.toStdString());
        }
        qDebug() << "[  OK  ] DirectoryManager check: created directory. Type:" << dirsType << "Path:"
                 << dirpath;
    }
}

void DirectoryManager::checkupSystem(const QString &rootdir)
{
    if (rootdir.isEmpty()) {
        m_systemDirectoryPaths[DirectoryTypeSystem::Root] =
            qApp->applicationDirPath() + QDir::separator() + "GraphEditor";
    } else {
        auto rdir = QDir(rootdir);
        if (!rdir.exists() || !rdir.isReadable()) {
            throw std::invalid_argument(
                "Invalid directory (not exist or not readable)");
        }
        m_systemDirectoryPaths[DirectoryTypeSystem::Root] = rootdir;
    }

    m_systemDirectoryPaths[DirectoryTypeSystem::Logs]           = "logs";
    m_systemDirectoryPaths[DirectoryTypeSystem::Profiles]       = "profiles";
    m_systemDirectoryPaths[DirectoryTypeSystem::Sections]       = "sections";
    m_systemDirectoryPaths[DirectoryTypeSystem::Vertices]       = "vertices";
    m_systemDirectoryPaths[DirectoryTypeSystem::Connections]    = "connections";
    m_systemDirectoryPaths[DirectoryTypeSystem::Backup]         = "backups";

    auto& rootdirpath = m_systemDirectoryPaths[DirectoryTypeSystem::Root];

    createDirectories(m_systemDirectoryPaths, m_systemDirectoryPaths[DirectoryTypeSystem::Root], "Root");
}

void DirectoryManager::checkupTmp()
{
    m_tmpDirectoryPaths[DirectoryTypeTmp::Root] = m_systemDirectoryPaths[DirectoryTypeSystem::Root] + QDir::separator() + "tmp";

    m_tmpDirectoryPaths[DirectoryTypeTmp::Cache]            = "cache";
    m_tmpDirectoryPaths[DirectoryTypeTmp::Unsaved]          = "unsaved";
    m_tmpDirectoryPaths[DirectoryTypeTmp::ImportedImages]   = "images";

    createDirectories(m_tmpDirectoryPaths, m_tmpDirectoryPaths[DirectoryTypeTmp::Root], "Tmp");
}
