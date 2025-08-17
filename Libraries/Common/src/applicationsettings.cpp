#include "applicationsettings.h"

#include <QDateTime>
#include <QFile>

const QString APPLICATION_SETTINGS_FILE_PATH{"GraphEditor.ini"};

ApplicationSettings &ApplicationSettings::getInstance()
{
    static ApplicationSettings inst;
    return inst;
}

QDir ApplicationSettings::getApplicationDataDir()
{
    auto appDir = QDir::current();
    if (!QFileInfo(appDir.absolutePath() + QDir::separator() + "GraphEditor").exists()) {
        appDir.mkdir("GraphEditor");
    }
    appDir.cd("GraphEditor");
    return appDir;
}

void ApplicationSettings::loadSettings()
{
    return loadSettings(getApplicationDataDir().absolutePath() + QDir::separator() + APPLICATION_SETTINGS_FILE_PATH);
}

void ApplicationSettings::saveSettings() const
{
    return saveSettings(getApplicationDataDir().absolutePath() + QDir::separator() + APPLICATION_SETTINGS_FILE_PATH);
}

void ApplicationSettings::loadSettings(const QString& configPath) {
    LOG_INFO("Loading settings from file:", configPath);
    QSettings settings(configPath, QSettings::IniFormat);

    settings.beginGroup("APPSETTINGS");
    m_generalConfig = {};
    m_generalConfig.fromSettingsFile(settings);
    settings.endGroup();

    settings.beginGroup("CANVAS");
    m_canvasConfig = {};
    m_canvasConfig.fromSettingsFile(settings);
    settings.endGroup();

    settings.beginGroup("OBJECTS");
    m_objectsConfig = {};
    m_objectsConfig.fromSettingsFile(settings);
    settings.endGroup();

    // История файлов
    settings.beginGroup("ETC");
    m_recentFiles.clear();
    for (const auto& recentFile : settings.value("recent_open_files", "").toString().split(",")) {
        if (QFileInfo(recentFile).exists()) {
            m_recentFiles.insert(recentFile);
        }
    }
    settings.endGroup();
    LOG_OK("Setgings loaded");
}

void ApplicationSettings::saveSettings(const QString& configPath) const {
    LOG_INFO("Saving settings to file:", configPath);

    QSettings settings(configPath, QSettings::IniFormat);
    settings.clear();

    settings.beginGroup("APPSETTINGS");
    m_generalConfig.addToSettingsFile(settings);
    settings.endGroup();

    settings.beginGroup("CANVAS");
    m_canvasConfig.addToSettingsFile(settings);
    settings.endGroup();

    settings.beginGroup("OBJECTS");
    m_objectsConfig.addToSettingsFile(settings);
    settings.endGroup();

    // История файлов
    settings.beginGroup("ETC");
    settings.setValue("recent_open_files", m_recentFiles.values().join(","));
    settings.endGroup();
    settings.sync();

    LOG_OK("Setgings saved");
}

ApplicationSettingsHelper::GeneralConfiguration &ApplicationSettings::getGeneralConfig()
{
    return m_generalConfig;
}

ApplicationSettingsHelper::ObjectsConfiguration &ApplicationSettings::getObjectsConfig()
{
    return m_objectsConfig;
}

ApplicationSettingsHelper::CanvasConfiguration &ApplicationSettings::getCanvasConfig()
{
    return m_canvasConfig;
}

void ApplicationSettings::addRecentFile(const QString &path)
{
    m_recentFiles.insert(path);
}

QStringList ApplicationSettings::getRecentOpenFiles() const
{
    return m_recentFiles.values();
}

void ApplicationSettings::removeRecentFile(const QString &path)
{
    m_recentFiles.remove(path);
}

void ApplicationSettings::clearRecentFiles()
{
    m_recentFiles.clear();
}

ApplicationSettings::ApplicationSettings()
{

}

ApplicationSettings::~ApplicationSettings()
{

}
