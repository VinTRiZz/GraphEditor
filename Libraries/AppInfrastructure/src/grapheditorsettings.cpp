#include "grapheditorsettings.h"

#include <QDateTime>
#include <QFile>

#include <Components/Logger/Logger.h>

GraphEditorSettings &GraphEditorSettings::getInstance()
{
    static GraphEditorSettings inst;
    return inst;
}

SubSettings::GeneralConfiguration&
GraphEditorSettings::getGeneralConfig() {
    return m_generalConfig;
}

SubSettings::ObjectsConfiguration&
GraphEditorSettings::getObjectsConfig() {
    return m_objectsConfig;
}

SubSettings::CanvasConfiguration&
GraphEditorSettings::getCanvasConfig() {
    return m_canvasConfig;
}

void GraphEditorSettings::addRecentFile(const QString& path) {
    m_recentFiles.insert(path);
}

QStringList GraphEditorSettings::getRecentOpenFiles() const {
    return m_recentFiles.values();
}

void GraphEditorSettings::removeRecentFile(const QString& path) {
    m_recentFiles.remove(path);
}

void GraphEditorSettings::clearRecentFiles() {
    m_recentFiles.clear();
}

void GraphEditorSettings::readSettings(QSettings &settingsFile)
{
    settingsFile.beginGroup("APPSETTINGS");
    m_generalConfig = {};
    m_generalConfig.fromSettingsFile(settingsFile);
    settingsFile.endGroup();

    settingsFile.beginGroup("CANVAS");
    m_canvasConfig = {};
    m_canvasConfig.fromSettingsFile(settingsFile);
    settingsFile.endGroup();

    settingsFile.beginGroup("OBJECTS");
    m_objectsConfig = {};
    m_objectsConfig.fromSettingsFile(settingsFile);
    settingsFile.endGroup();

    // История файлов
    settingsFile.beginGroup("ETC");
    m_recentFiles.clear();
    for (const auto& recentFile :
         settingsFile.value("recent_open_files", "").toString().split(",")) {
        if (QFileInfo(recentFile).exists()) {
            m_recentFiles.insert(recentFile);
        }
    }
    settingsFile.endGroup();
}

void GraphEditorSettings::writeSettings(QSettings &settingsFile) const
{
    settingsFile.beginGroup("APPSETTINGS");
    m_generalConfig.addToSettingsFile(settingsFile);
    settingsFile.endGroup();

    settingsFile.beginGroup("CANVAS");
    m_canvasConfig.addToSettingsFile(settingsFile);
    settingsFile.endGroup();

    settingsFile.beginGroup("OBJECTS");
    m_objectsConfig.addToSettingsFile(settingsFile);
    settingsFile.endGroup();

    // История файлов
    settingsFile.beginGroup("ETC");
    settingsFile.setValue("recent_open_files", m_recentFiles.values().join(","));
    settingsFile.endGroup();
}
