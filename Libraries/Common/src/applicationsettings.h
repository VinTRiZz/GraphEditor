#ifndef APPLICATIONSETTINGS_H
#define APPLICATIONSETTINGS_H

#include <boost/noncopyable.hpp>
#include <QSettings>
#include <QString>
#include <QStringList>
#include <QGradient>
#include <QColor>

class ApplicationSettings : public boost::noncopyable
{
public:
    /**
     * @brief The Theme enum Тема оформления
     */
    enum class Theme {
        System,
        Light,
        Dark
    };

    /**
     * @brief The NodeShape enum Тип фигуры вершины по умолчанию
     */
    enum class NodeShape
    {
        Circle,
        Square,
        Hexagon,
        Triangle,
        RoundedRect
    };

    /**
     * @brief The NodeSize enum Размер вершин
     */
    enum class NodeSize
    {
        Small,
        Medium,
        Large,
        ExtraLarge
    };

    /**
     * @brief The ArrowStyle enum Стиль стрелок
     */
    enum class ArrowStyle
    {
        None,
        Triangle,
        Diamond,
        Arrow,
        Circle
    };

    // Работа с классом
    static ApplicationSettings& getInstance();
    void loadSettings();
    void saveSettings() const;

    // Свойства
    Theme getTheme() const;
    NodeShape getNodeShape() const;
    NodeSize getNodeSize() const;
    int getCanvasOpacity() const;
    int getLineThickness() const;
    ArrowStyle getArrowStyle() const;
    QGradient getBackgroundGradient() const;
    bool hasGrid() const;
    double getGridSize() const;
    bool snapToGrid() const;
    bool confirmDeletion() const;
    int getAutoSaveInterval() const;
    QStringList getRecentFiles() const;
    bool removeMetadata() const;
    bool cleanupTempFiles() const;
    int getMaxLogFiles() const;
    bool minimizeToTray() const;
    QString getDateTimeFormat() const;

    void setTheme(Theme theme);
    void setNodeShape(NodeShape shape);

    void addRecentFile(const QString& path);
    void removeRecentFile(const QString& path);

private:
    ApplicationSettings();
    ~ApplicationSettings() = default;

    void loadSettings(const QString& configPath);
    void saveSettings(const QString& configPath) const;

    void fixErrors();


    // Преобразования для сохранения и загрузки
    QString themeToString(Theme theme) const;
    Theme stringToTheme(const QString& str) const;

    // Настройки приложения
    Theme m_theme = Theme::System;
    NodeShape m_nodeShape = NodeShape::Circle;
    NodeSize m_nodeSize = NodeSize::Medium;
    int m_canvasOpacity = 90;
    int m_lineThickness = 2;
    ArrowStyle m_arrowStyle = ArrowStyle::Triangle;
    QGradient m_backgroundGradient = QLinearGradient(0, 0, 100, 100);
    bool m_hasGrid = true;
    double m_gridSize = 20.0;
    bool m_snapToGrid = true;
    bool m_confirmDeletion = true;
    int m_autoSaveInterval = 300; // 5 минут
    QStringList m_recentFiles;
    bool m_removeMetadata = false;
    bool m_cleanupTempFiles = true;
    int m_maxLogFiles = 10;
    bool m_minimizeToTray = true;
    QString m_dateTimeFormat = "yyyy-MM-dd HH:mm:ss";
};

#endif // APPLICATIONSETTINGS_H
