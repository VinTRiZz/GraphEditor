#ifndef APPLICATIONSETTINGS_H
#define APPLICATIONSETTINGS_H

#include <boost/noncopyable.hpp>
#include "applicationsettingshelper.h"

#include <QSet>

class ApplicationSettings : public boost::noncopyable {
public:
    // Работа с файлом настроек и классом
    static ApplicationSettings& getInstance();
    static QDir getApplicationDataDir();
    void loadSettings();
    void saveSettings() const;

    ApplicationSettingsHelper::GeneralConfiguration&    getGeneralConfig();
    ApplicationSettingsHelper::ObjectsConfiguration&    getObjectsConfig();
    ApplicationSettingsHelper::CanvasConfiguration&     getCanvasConfig();

    // Работа с историей файлов
    void addRecentFile(const QString& path);     ///< Добавляет файл в список недавних
    QStringList getRecentOpenFiles() const;      ///< Возвращает список недавних файлов
    void removeRecentFile(const QString& path);  ///< Удаляет файл из списка недавних
    void clearRecentFiles();                     ///< Удаляет все файлы из списка недавних

private:
    ApplicationSettings();
    ~ApplicationSettings();

    void loadSettings(const QString& configPath);
    void saveSettings(const QString& configPath) const;

    QSet<QString> m_recentFiles;

    ApplicationSettingsHelper::GeneralConfiguration m_generalConfig;
    ApplicationSettingsHelper::ObjectsConfiguration m_objectsConfig;
    ApplicationSettingsHelper::CanvasConfiguration  m_canvasConfig;
};

#endif  // APPLICATIONSETTINGS_H
