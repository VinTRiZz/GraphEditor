#ifndef DIRECTORYMANAGER_H
#define DIRECTORYMANAGER_H

#include <QDir>
#include <QString>
#include <boost/noncopyable.hpp>
#include <map>

class DirectoryManager : public boost::noncopyable {
    DirectoryManager(const QString& rootdir);

public:
    static DirectoryManager& getInstance(const QString& rootdir = {});

    static QString getDocumentsPath();

    /**
     * @brief The DirectoryTypeSystem enum  Необходимые для работы приложения
     * директории
     */
    enum class DirectoryTypeSystem : int {
        Root,
        Logs,         // Логи приложения
        Profiles,     // Профили работы (конфиги)
        Backup,       // Бэкапы файлов
        Sections,     // Предустановленные тематические разделы
        Vertices,     // Предустановленные виды вершин
        Connections,  // Предустановленные виды соединений
    };
    static QDir getSystemDirectory(DirectoryTypeSystem dtype);
    static QString getSystemDirectoryPath(DirectoryTypeSystem dtype,
                                          bool withNativeSeparator = true);

    /**
     * @brief The DirectoryTypeTmp enum Директории с кэшем
     */
    enum class DirectoryTypeTmp : int {
        Root,
        ImportedImages,  // Импортированные изображения (для восст. истории)
        Unsaved,         // Изменения в файлах до сохранения
        Cache,           // Прочие объекты кэша
    };
    static QDir getTmpDirectory(DirectoryTypeTmp dtype);
    static QString getTmpDirectoryPath(DirectoryTypeTmp dtype,
                                       bool withNativeSeparator = true);

private:
    void checkup(const QString& rootdir);

    template<typename MapT, typename TypeT>
    static QString getDirectoryPath(const MapT& typeMap, TypeT dtype,
                                    bool withNativeSeparator = true);

    template<typename MapT>
    void createDirectories(const MapT& dirsMap, const QString& rootdir,
                           const QString& dirsType);

    void checkupSystem(const QString& rootdir);
    std::map<DirectoryTypeSystem, QString> m_systemDirectoryPaths;

    void checkupTmp();
    std::map<DirectoryTypeTmp, QString> m_tmpDirectoryPaths;
};

#endif  // DIRECTORYMANAGER_H
