#pragma once

#include <QSet>
#include <boost/noncopyable.hpp>

#include "subsettings.h"

#include <Components/Common/ApplicationSettings.h>

class GraphEditorSettings : public Common::ApplicationSettings {
public:
    const QString APPLICATION_SETTINGS_FILE_PATH{"default.ini"};

    // Работа с файлом настроек и классом
    static GraphEditorSettings& getInstance();

    SubSettings::GeneralConfiguration& getGeneralConfig();
    SubSettings::ObjectsConfiguration& getObjectsConfig();
    SubSettings::CanvasConfiguration& getCanvasConfig();

    // Работа с историей файлов
    void addRecentFile(
        const QString& path);  ///< Добавляет файл в список недавних
    QStringList getRecentOpenFiles()
        const;  ///< Возвращает список недавних файлов
    void removeRecentFile(
        const QString& path);  ///< Удаляет файл из списка недавних
    void clearRecentFiles();   ///< Удаляет все файлы из списка недавних

private:
    void readSettings(QSettings& settingsFile) override;
    void writeSettings(QSettings &settingsFile) const override;

    QSet<QString> m_recentFiles;

    SubSettings::GeneralConfiguration m_generalConfig;
    SubSettings::ObjectsConfiguration m_objectsConfig;
    SubSettings::CanvasConfiguration m_canvasConfig;
};
