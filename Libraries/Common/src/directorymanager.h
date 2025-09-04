#ifndef DIRECTORYMANAGER_H
#define DIRECTORYMANAGER_H

#include <boost/noncopyable.hpp>

#include <QString>
#include <QDir>

#include <map>

class DirectoryManager : public boost::noncopyable
{
    DirectoryManager();
public:
    enum class DirectoryType : int {
        Root,
        Config,
        Logs,
        Documents,
        Temporary,
        Backup,
    };

    static DirectoryManager& getInstance();

    static QString getDocumentsPath();

    static QDir getDirectory(DirectoryType dtype);
    static QString getDirectoryPath(DirectoryType dtype, bool withNativeSeparator = true);

private:
    void checkup();
    std::map<DirectoryType, QString> m_directoryPaths;
};

#endif // DIRECTORYMANAGER_H
