#include "applicationsettings.h"

#include <QDateTime>
#include <QFile>

const QString APPLICATION_SETTINGS_FILE_PATH{"GraphEditor.ini"};

ApplicationSettings::Theme ApplicationSettings::getThemeType() const {
    return m_themeType;
}

ApplicationSettings::NodeShape ApplicationSettings::getDefaultNodeShape()
    const {
    return m_defaultNodeShape;
}

ApplicationSettings::NodeSize ApplicationSettings::getNodeSize() const {
    return m_nodeSize;
}

int ApplicationSettings::getCanvasOpacity() const {
    return m_canvasOpacity;
}

int ApplicationSettings::getLineThickness() const {
    return m_lineThickness;
}

ApplicationSettings::ArrowStyle ApplicationSettings::getArrowStyle() const {
    return m_arrowStyle;
}

QGradient ApplicationSettings::getBackgroundGradient() const {
    return m_backgroundGradient;
}

bool ApplicationSettings::getIsGridEnabled() const {
    return m_isGridEnabled;
}

double ApplicationSettings::getGridSize() const {
    return m_gridSize;
}

bool ApplicationSettings::getNeedConfirmDeletion() const {
    return m_needConfirmDeletion;
}

int ApplicationSettings::getAutoSaveInterval() const {
    return m_autoSaveIntervalSec;
}

QStringList ApplicationSettings::getRecentOpenFiles() const {
    return m_recentFiles;
}

bool ApplicationSettings::getNeedRemoveMetadata() const {
    return m_removeMetadata;
}

bool ApplicationSettings::getNeedCleanupTempFiles() const {
    return m_cleanupTempFiles;
}

unsigned ApplicationSettings::getMaxLogFileCount() const {
    return m_maxLogFiles;
}

bool ApplicationSettings::getNeedMinimizeToTray() const {
    return m_minimizeToTray;
}

QString ApplicationSettings::getDateTimeFormat() const {
    return m_dateTimeFormat;
}

QSize ApplicationSettings::getCanvasSize() const {
    return m_canvasSize;
}

void ApplicationSettings::setNodeSize(NodeSize size) {
    m_nodeSize = size;
}

void ApplicationSettings::setCanvasOpacity(int opacity) {
    m_canvasOpacity = qBound(0, opacity, 100);
}

void ApplicationSettings::setLineThickness(int thickness) {
    m_lineThickness = qBound(1, thickness, 10);
}

void ApplicationSettings::setArrowStyle(ArrowStyle style) {
    m_arrowStyle = style;
}

void ApplicationSettings::setBackgroundGradient(const QGradient& gradient) {
    m_backgroundGradient = gradient;
}

void ApplicationSettings::setIsGridEnabled(bool hasGrid) {
    m_isGridEnabled = hasGrid;
}

void ApplicationSettings::setGridSize(double size) {
    m_gridSize = qBound(5.0, size, 100.0);
}

void ApplicationSettings::setNeedConfirmDeletion(bool confirm) {
    m_needConfirmDeletion = confirm;
}

void ApplicationSettings::setAutoSaveInterval(int seconds) {
    m_autoSaveIntervalSec = qMax(0, seconds);
}

void ApplicationSettings::setNeedRemoveMetadata(bool remove) {
    m_removeMetadata = remove;
}

void ApplicationSettings::setNeedCleanupTempFiles(bool cleanup) {
    m_cleanupTempFiles = cleanup;
}

void ApplicationSettings::setMaxLogFileCount(int maxFiles) {
    m_maxLogFiles = qMax(0, maxFiles);
}

void ApplicationSettings::setMinimizeToTray(bool minimize) {
    m_minimizeToTray = minimize;
}

void ApplicationSettings::setDateTimeFormat(const QString& format) {
    if (!format.isEmpty()) {
        m_dateTimeFormat = format;
    }
}

void ApplicationSettings::setCanvasSize(const QSize& canvasSize) {
    m_canvasSize = canvasSize;
}

void ApplicationSettings::setThemeType(Theme theme) {
    m_themeType = theme;
}

void ApplicationSettings::setDefaultNodeShape(NodeShape shape) {
    m_defaultNodeShape = shape;
}

void ApplicationSettings::addRecentFile(const QString& path) {
    m_recentFiles.removeAll(path);
    m_recentFiles.prepend(path);
    while (m_recentFiles.size() > 10) {
        m_recentFiles.removeLast();
    }
}

void ApplicationSettings::removeRecentFile(const QString& path) {
    m_recentFiles.removeOne(path);
}

ApplicationSettings& ApplicationSettings::getInstance() {
    static ApplicationSettings instance;
    return instance;
}

void ApplicationSettings::loadSettings() {
    return loadSettings(APPLICATION_SETTINGS_FILE_PATH);
}

void ApplicationSettings::saveSettings() const {
    return saveSettings(APPLICATION_SETTINGS_FILE_PATH);
}

ApplicationSettings::ApplicationSettings() {
    // Инициализация градиента по умолчанию
    m_backgroundGradient.setColorAt(0, QColor(50, 50, 50));
    m_backgroundGradient.setColorAt(1, QColor(50, 50, 50));
}

void ApplicationSettings::fixErrors() {
    // Проверка перечислений
    const int themeMax = static_cast<int>(Theme::Dark);
    if (static_cast<int>(m_themeType) < 0 ||
        static_cast<int>(m_themeType) > themeMax) {
        m_themeType = Theme::System;
    }

    const int shapeMax = static_cast<int>(NodeShape::RoundedRect);
    if (static_cast<int>(m_defaultNodeShape) < 0 ||
        static_cast<int>(m_defaultNodeShape) > shapeMax) {
        m_defaultNodeShape = NodeShape::Circle;
    }

    const int sizeMax = static_cast<int>(NodeSize::ExtraLarge);
    if (static_cast<int>(m_nodeSize) < 0 ||
        static_cast<int>(m_nodeSize) > sizeMax) {
        m_nodeSize = NodeSize::Medium;
    }

    const int arrowMax = static_cast<int>(ArrowStyle::Circle);
    if (static_cast<int>(m_arrowStyle) < 0 ||
        static_cast<int>(m_arrowStyle) > arrowMax) {
        m_arrowStyle = ArrowStyle::Triangle;
    }

    // Проверка числовых значений
    m_canvasOpacity = qBound(0, m_canvasOpacity, 100);
    m_lineThickness = qBound(1, m_lineThickness, 10);
    m_gridSize = qBound(5.0, m_gridSize, 100.0);
    m_autoSaveIntervalSec = qMax(0, m_autoSaveIntervalSec);
    m_maxLogFiles = std::max(unsigned{0}, m_maxLogFiles);

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
        defaultGradient.setColorAt(1, QColor(235, 235, 235));
        m_backgroundGradient = defaultGradient;
    }

    // Проверка списка файлов
    for (auto it = m_recentFiles.begin(); it != m_recentFiles.end();) {
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

    m_canvasSize.setWidth(std::clamp(m_canvasSize.width(), 500, 100000));
    m_canvasSize.setHeight(std::clamp(m_canvasSize.height(), 500, 100000));
}

void ApplicationSettings::loadSettings(const QString& configPath) {
    QSettings settings(configPath, QSettings::IniFormat);

    // Общие настройки
    settings.beginGroup("General");
    m_themeType = stringToTheme(settings.value("theme", "System").toString());
    m_autoSaveIntervalSec = settings.value("autosave_interval", 300).toInt();
    m_minimizeToTray = settings.value("minimize_to_tray", true).toBool();
    m_dateTimeFormat =
        settings.value("datetime_format", "yyyy-MM-dd HH:mm:ss").toString();
    settings.endGroup();

    // Внешний вид
    settings.beginGroup("Appearance");
    m_canvasSize = settings.value("canvas_size").toSize();
    m_defaultNodeShape =
        static_cast<NodeShape>(settings.value("node_shape", 0).toInt());
    m_nodeSize = static_cast<NodeSize>(settings.value("node_size", 1).toInt());
    m_canvasOpacity = settings.value("canvas_opacity", 90).toInt();
    m_lineThickness = settings.value("line_thickness", 2).toInt();
    m_arrowStyle =
        static_cast<ArrowStyle>(settings.value("arrow_style", 1).toInt());
    m_isGridEnabled = settings.value("show_grid", true).toBool();
    m_gridSize = settings.value("grid_size", 20.0).toDouble();

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
    m_needConfirmDeletion = settings.value("confirm_deletion", true).toBool();
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
    settings.setValue("theme", themeToString(m_themeType));
    settings.setValue("autosave_interval", m_autoSaveIntervalSec);
    settings.setValue("minimize_to_tray", m_minimizeToTray);
    settings.setValue("datetime_format", m_dateTimeFormat);
    settings.endGroup();

    // Внешний вид
    settings.beginGroup("Appearance");
    settings.setValue("canvas_size", m_canvasSize);
    settings.setValue("node_shape", static_cast<int>(m_defaultNodeShape));
    settings.setValue("node_size", static_cast<int>(m_nodeSize));
    settings.setValue("canvas_opacity", m_canvasOpacity);
    settings.setValue("line_thickness", m_lineThickness);
    settings.setValue("arrow_style", static_cast<int>(m_arrowStyle));
    settings.setValue("show_grid", m_isGridEnabled);
    settings.setValue("grid_size", m_gridSize);

    // Сохранение градиента
    // Здесь должна быть конвертация QGradient в строку
    QString gradientStr =
        QString("linear:%1:%2")
            .arg(m_backgroundGradient.stops().first().second.name())
            .arg(m_backgroundGradient.stops().last().second.name());
    settings.setValue("background_gradient", gradientStr);
    settings.endGroup();

    // Поведение
    settings.beginGroup("Behavior");
    settings.setValue("confirm_deletion", m_needConfirmDeletion);
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
    switch (theme) {
        case Theme::Light:
            return "Light";
        case Theme::Dark:
            return "Dark";
        default:
            return "System";
    }
}

ApplicationSettings::Theme ApplicationSettings::stringToTheme(
    const QString& str) const {
    if (str == "Light")
        return Theme::Light;
    if (str == "Dark")
        return Theme::Dark;
    return Theme::System;
}
