#include "applicationsettings.h"

#include <QFile>
#include <QDateTime>

const QString APPLICATION_SETTINGS_FILE_PATH {"GraphEditor.ini"};

ApplicationSettings::Theme ApplicationSettings::getTheme() const { return m_theme; }

ApplicationSettings::NodeShape ApplicationSettings::getNodeShape() const { return m_nodeShape; }

ApplicationSettings::NodeSize ApplicationSettings::getNodeSize() const { return m_nodeSize; }

int ApplicationSettings::getCanvasOpacity() const { return m_canvasOpacity; }

int ApplicationSettings::getLineThickness() const { return m_lineThickness; }

ApplicationSettings::ArrowStyle ApplicationSettings::getArrowStyle() const { return m_arrowStyle; }

QGradient ApplicationSettings::getBackgroundGradient() const { return m_backgroundGradient; }

bool ApplicationSettings::hasGrid() const { return m_hasGrid; }

double ApplicationSettings::getGridSize() const { return m_gridSize; }

bool ApplicationSettings::snapToGrid() const { return m_snapToGrid; }

bool ApplicationSettings::confirmDeletion() const { return m_confirmDeletion; }

int ApplicationSettings::getAutoSaveInterval() const { return m_autoSaveInterval; }

QStringList ApplicationSettings::getRecentFiles() const { return m_recentFiles; }

bool ApplicationSettings::removeMetadata() const { return m_removeMetadata; }

bool ApplicationSettings::cleanupTempFiles() const { return m_cleanupTempFiles; }

int ApplicationSettings::getMaxLogFiles() const { return m_maxLogFiles; }

bool ApplicationSettings::minimizeToTray() const { return m_minimizeToTray; }

QString ApplicationSettings::getDateTimeFormat() const { return m_dateTimeFormat; }

void ApplicationSettings::setTheme(Theme theme) { m_theme = theme; }

void ApplicationSettings::setNodeShape(NodeShape shape) { m_nodeShape = shape; }

void ApplicationSettings::addRecentFile(const QString &path) {
    m_recentFiles.removeAll(path);
    m_recentFiles.prepend(path);
    while (m_recentFiles.size() > 10) {
        m_recentFiles.removeLast();
    }
}

void ApplicationSettings::removeRecentFile(const QString &path)
{
    m_recentFiles.removeOne(path);
}

ApplicationSettings& ApplicationSettings::getInstance() {
    static ApplicationSettings instance;
    return instance;
}

void ApplicationSettings::loadSettings()
{
    return loadSettings(APPLICATION_SETTINGS_FILE_PATH);
}

void ApplicationSettings::saveSettings() const
{
    return saveSettings(APPLICATION_SETTINGS_FILE_PATH);
}

ApplicationSettings::ApplicationSettings() {
    // Инициализация градиента по умолчанию
    m_backgroundGradient.setColorAt(0, QColor(240, 240, 240));
    m_backgroundGradient.setColorAt(1, QColor(200, 200, 200));
}

void ApplicationSettings::fixErrors() {
    // Проверка перечислений
    const int themeMax = static_cast<int>(Theme::Dark);
    if (static_cast<int>(m_theme) < 0 || static_cast<int>(m_theme) > themeMax) {
        m_theme = Theme::System;
    }

    const int shapeMax = static_cast<int>(NodeShape::RoundedRect);
    if (static_cast<int>(m_nodeShape) < 0 || static_cast<int>(m_nodeShape) > shapeMax) {
        m_nodeShape = NodeShape::Circle;
    }

    const int sizeMax = static_cast<int>(NodeSize::ExtraLarge);
    if (static_cast<int>(m_nodeSize) < 0 || static_cast<int>(m_nodeSize) > sizeMax) {
        m_nodeSize = NodeSize::Medium;
    }

    const int arrowMax = static_cast<int>(ArrowStyle::Circle);
    if (static_cast<int>(m_arrowStyle) < 0 || static_cast<int>(m_arrowStyle) > arrowMax) {
        m_arrowStyle = ArrowStyle::Triangle;
    }

    // Проверка числовых значений
    m_canvasOpacity = qBound(0, m_canvasOpacity, 100);
    m_lineThickness = qBound(1, m_lineThickness, 10);
    m_gridSize = qBound(5.0, m_gridSize, 100.0);
    m_autoSaveInterval = qMax(0, m_autoSaveInterval);
    m_maxLogFiles = qMax(0, m_maxLogFiles);

    // Проверка формата даты
    auto isValidFormat = [](const QString& format) {
        QDateTime test = QDateTime::currentDateTime();
        QString testStr = test.toString(format);
        QDateTime parsed = QDateTime::fromString(testStr, format);
        return parsed.isValid() && (qAbs(parsed.secsTo(test)) < 2);
    };

    if (!isValidFormat(m_dateTimeFormat)) {
        m_dateTimeFormat = "yyyy-MM-dd HH:mm:ss";
    }

    // Проверка градиента
    if (m_backgroundGradient.stops().isEmpty()) {
        QLinearGradient defaultGradient(0, 0, 100, 100);
        defaultGradient.setColorAt(0, QColor(240, 240, 240));
        defaultGradient.setColorAt(1, QColor(200, 200, 200));
        m_backgroundGradient = defaultGradient;
    }

    // Проверка списка файлов
    for (auto it = m_recentFiles.begin(); it != m_recentFiles.end(); ) {
        if (it->isEmpty()) {
            it = m_recentFiles.erase(it);
        } else {
            // Нормализация путей
            *it = QFileInfo(*it).absoluteFilePath();
            ++it;
        }
    }

    // Удаление дубликатов в истории файлов
    QStringList uniqueFiles;
    for (const auto& file : qAsConst(m_recentFiles)) {
        if (!uniqueFiles.contains(file)) {
            uniqueFiles.append(file);
        }
    }
    m_recentFiles = uniqueFiles;
}

void ApplicationSettings::loadSettings(const QString& configPath) {
    QSettings settings(configPath, QSettings::IniFormat);

    // Общие настройки
    settings.beginGroup("General");
    m_theme = stringToTheme(settings.value("theme", "System").toString());
    m_autoSaveInterval = settings.value("autosave_interval", 300).toInt();
    m_minimizeToTray = settings.value("minimize_to_tray", true).toBool();
    m_dateTimeFormat = settings.value("datetime_format", "yyyy-MM-dd HH:mm:ss").toString();
    settings.endGroup();

    // Внешний вид
    settings.beginGroup("Appearance");
    m_nodeShape = static_cast<NodeShape>(settings.value("node_shape", 0).toInt());
    m_nodeSize = static_cast<NodeSize>(settings.value("node_size", 1).toInt());
    m_canvasOpacity = settings.value("canvas_opacity", 90).toInt();
    m_lineThickness = settings.value("line_thickness", 2).toInt();
    m_arrowStyle = static_cast<ArrowStyle>(settings.value("arrow_style", 1).toInt());
    m_hasGrid = settings.value("show_grid", true).toBool();
    m_gridSize = settings.value("grid_size", 20.0).toDouble();
    m_snapToGrid = settings.value("snap_to_grid", true).toBool();

    // Загрузка градиента
    QString gradientStr = settings.value("background_gradient").toString();
    if (!gradientStr.isEmpty()) {
        QStringList colors = gradientStr.split(':');
        if (colors.size() == 3 && colors[0] == "linear") {
            m_backgroundGradient = QLinearGradient(0, 0, 100, 100);
            m_backgroundGradient.setColorAt(0, QColor(colors[1]));
            m_backgroundGradient.setColorAt(1, QColor(colors[2]));
        }
    }
    settings.endGroup();

    // Поведение
    settings.beginGroup("Behavior");
    m_confirmDeletion = settings.value("confirm_deletion", true).toBool();
    settings.endGroup();

    // Безопасность
    settings.beginGroup("Security");
    m_removeMetadata = settings.value("remove_metadata", false).toBool();
    m_cleanupTempFiles = settings.value("cleanup_temp_files", true).toBool();
    m_maxLogFiles = settings.value("max_log_files", 10).toInt();
    settings.endGroup();

    // История файлов
    settings.beginGroup("RecentFiles");
    m_recentFiles = settings.value("file_list").toStringList();
    settings.endGroup();

    fixErrors();
}

void ApplicationSettings::saveSettings(const QString& configPath) const {
    QSettings settings(configPath, QSettings::IniFormat);

    // Общие настройки
    settings.beginGroup("General");
    settings.setValue("theme", themeToString(m_theme));
    settings.setValue("autosave_interval", m_autoSaveInterval);
    settings.setValue("minimize_to_tray", m_minimizeToTray);
    settings.setValue("datetime_format", m_dateTimeFormat);
    settings.endGroup();

    // Внешний вид
    settings.beginGroup("Appearance");
    settings.setValue("node_shape", static_cast<int>(m_nodeShape));
    settings.setValue("node_size", static_cast<int>(m_nodeSize));
    settings.setValue("canvas_opacity", m_canvasOpacity);
    settings.setValue("line_thickness", m_lineThickness);
    settings.setValue("arrow_style", static_cast<int>(m_arrowStyle));
    settings.setValue("show_grid", m_hasGrid);
    settings.setValue("grid_size", m_gridSize);
    settings.setValue("snap_to_grid", m_snapToGrid);

    // Сохранение градиента
    // Здесь должна быть конвертация QGradient в строку
    QString gradientStr = QString("linear:%1:%2")
        .arg(m_backgroundGradient.stops().first().second.name())
        .arg(m_backgroundGradient.stops().last().second.name());
    settings.setValue("background_gradient", gradientStr);
    settings.endGroup();

    // Поведение
    settings.beginGroup("Behavior");
    settings.setValue("confirm_deletion", m_confirmDeletion);
    settings.endGroup();

    // Безопасность
    settings.beginGroup("Security");
    settings.setValue("remove_metadata", m_removeMetadata);
    settings.setValue("cleanup_temp_files", m_cleanupTempFiles);
    settings.setValue("max_log_files", m_maxLogFiles);
    settings.endGroup();

    // История файлов
    settings.beginGroup("RecentFiles");
    settings.setValue("file_list", m_recentFiles);
    settings.endGroup();
}

// Конвертеры для Theme
QString ApplicationSettings::themeToString(Theme theme) const {
    switch(theme) {
        case Theme::Light: return "Light";
        case Theme::Dark: return "Dark";
        default: return "System";
    }
}

ApplicationSettings::Theme ApplicationSettings::stringToTheme(const QString& str) const {
    if (str == "Light") return Theme::Light;
    if (str == "Dark") return Theme::Dark;
    return Theme::System;
}
