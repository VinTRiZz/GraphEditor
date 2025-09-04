#include "directorymanager.h"

#include <QDir>
#include <QDebug>

DirectoryManager::DirectoryManager()
{
    checkup();
}

DirectoryManager &DirectoryManager::getInstance()
{
    static DirectoryManager inst;
    return inst;
}

QDir DirectoryManager::getDirectory(DirectoryType dtype)
{
    return QDir(getDirectoryPath(dtype));
}

QString DirectoryManager::getDirectoryPath(DirectoryType dtype, bool withNativeSeparator)
{
    auto& inst = getInstance();

    if (!inst.m_directoryPaths.count(dtype)) {
        throw std::invalid_argument(std::string("DirectoryManager: directory init failure! Type: ") + std::to_string(static_cast<int>(dtype)));
    }

    if (dtype == DirectoryType::Root) {
        return (withNativeSeparator ?
                    inst.m_directoryPaths.at(dtype) + QDir::separator() :
                    inst.m_directoryPaths.at(dtype));
    }

    return inst.getDirectoryPath(DirectoryType::Root) + (withNativeSeparator ?
                inst.m_directoryPaths.at(dtype) + QDir::separator() :
                inst.m_directoryPaths.at(dtype));
}

void DirectoryManager::checkup()
{
    m_directoryPaths[DirectoryType::Root] = qApp->applicationDirPath() + QDir::separator() + "GraphEditor";
    m_directoryPaths[DirectoryType::Config] = "config";
    m_directoryPaths[DirectoryType::Logs] = "logs";
    m_directoryPaths[DirectoryType::Documents] = QDir::homePath() + QDir::separator() + "Documents";

    auto curdir = QDir::current();
    auto& rootdirpath = m_directoryPaths[DirectoryType::Root];
    if (!QDir(rootdirpath).exists()) {
        if (!curdir.mkdir(rootdirpath)) {
            throw std::domain_error("DirectoryManager check: Error creating root directory");
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
            qDebug() << "[  OK  ] DirectoryManager check: Directory exist:" << dirpath;
            continue;
        }

        if (!curdir.mkdir(dirpath)) {
            throw std::domain_error(std::string("DirectoryManager: Error creating directory: ") + dirpath.toStdString());
        }
        qDebug() << "[  OK  ] DirectoryManager check: created system directory:" << dirpath;
    }
}
