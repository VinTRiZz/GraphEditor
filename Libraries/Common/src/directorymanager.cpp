#include "directorymanager.h"

#include <QDebug>
#include <QDir>

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

QDir DirectoryManager::getDirectory(DirectoryType dtype) {
    return QDir(getDirectoryPath(dtype));
}

QString DirectoryManager::getDirectoryPath(DirectoryType dtype,
                                           bool withNativeSeparator) {
    auto& inst = getInstance();

    if (!inst.m_directoryPaths.count(dtype)) {
        throw std::invalid_argument(
            std::string("DirectoryManager: directory init failure! Type: ") +
            std::to_string(static_cast<int>(dtype)));
    }

    if (dtype == DirectoryType::Root) {
        return (withNativeSeparator
                    ? inst.m_directoryPaths.at(dtype) + QDir::separator()
                    : inst.m_directoryPaths.at(dtype));
    }

    return inst.getDirectoryPath(DirectoryType::Root) +
           (withNativeSeparator
                ? inst.m_directoryPaths.at(dtype) + QDir::separator()
                : inst.m_directoryPaths.at(dtype));
}

void DirectoryManager::checkup(const QString& rootdir) {
    qDebug() << "CALLED CHECKUP";
    if (rootdir.isEmpty()) {
        m_directoryPaths[DirectoryType::Root] =
            qApp->applicationDirPath() + QDir::separator() + "GraphEditor";
    } else {
        auto rdir = QDir(rootdir);
        if (!rdir.exists() || !rdir.isReadable()) {
            throw std::invalid_argument(
                "Invalid directory (not exist or not readable)");
        }
        m_directoryPaths[DirectoryType::Root] = rootdir;
    }

    m_directoryPaths[DirectoryType::Config] = "config";
    m_directoryPaths[DirectoryType::Logs] = "logs";
    m_directoryPaths[DirectoryType::Documents] = "documents";
    m_directoryPaths[DirectoryType::Temporary] = "tmp";
    m_directoryPaths[DirectoryType::Backup] = "backups";

    auto curdir = QDir::current();
    auto& rootdirpath = m_directoryPaths[DirectoryType::Root];
    if (!QDir(rootdirpath).exists()) {
        if (!curdir.mkdir(rootdirpath)) {
            throw std::domain_error(
                "DirectoryManager check: Error creating root directory");
        }
        qDebug() << "[  OK  ] DirectoryManager check: Root dir created";
    } else {
        qDebug() << "[  OK  ] DirectoryManager check: Root dir exist";
    }
    curdir.cd(m_directoryPaths[DirectoryType::Root]);

    for (auto& [dirtype, dirpath] : m_directoryPaths) {
        if (dirtype == DirectoryType::Root) {
            continue;
        }

        if (QDir(rootdirpath + QDir::separator() + dirpath).exists()) {
            qDebug() << "[  OK  ] DirectoryManager check: Directory exist:"
                     << dirpath;
            continue;
        }

        if (!curdir.mkdir(dirpath)) {
            throw std::domain_error(
                std::string("DirectoryManager: Error creating directory: ") +
                dirpath.toStdString());
        }
        qDebug() << "[  OK  ] DirectoryManager check: created system directory:"
                 << dirpath;
    }
}
