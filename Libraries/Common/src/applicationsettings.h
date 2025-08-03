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
        Ellipse,
        Circle,
        Square,
        Triangle,
        Rectangle,
        RoundedRect,
        Hexagon,
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
        Circle,
    };

    // Работа с файлом настроек и классом
    static ApplicationSettings& getInstance();
    void loadSettings();
    void saveSettings() const;

    // ======================================================= //
    // Основные свойства
    Theme       getThemeType() const;
    bool        getNeedConfirmDeletion() const;
    int         getAutoSaveInterval() const;
    bool        getNeedRemoveMetadata() const;
    bool        getNeedCleanupTempFiles() const;
    unsigned    getMaxLogFileCount() const;
    bool        getNeedMinimizeToTray() const;
    QString     getDateTimeFormat() const;

    void    setThemeType(Theme theme);
    void    setNeedConfirmDeletion(bool confirm);
    void    setAutoSaveInterval(int seconds);
    void    setNeedRemoveMetadata(bool remove);
    void    setNeedCleanupTempFiles(bool cleanup);
    void    setMaxLogFileCount(int maxFiles);
    void    setMinimizeToTray(bool minimize);
    void    setDateTimeFormat(const QString& format);


    // ======================================================= //
    // Полотно
    QSize       getCanvasSize() const;
    int         getCanvasOpacity() const;
    QGradient   getBackgroundGradient() const;
    bool        getIsGridEnabled() const;
    double      getGridSize() const;

    void    setCanvasOpacity(int opacity);
    void    setBackgroundGradient(const QGradient& gradient);
    void    setIsGridEnabled(bool hasGrid);
    void    setGridSize(double size);
    void    setCanvasSize(const QSize& canvasSize);

    // ======================================================= //
    // Свойства объектов
    NodeShape   getDefaultNodeShape() const;
    NodeSize    getNodeSize() const;
    int         getLineThickness() const;
    ArrowStyle  getArrowStyle() const;

    void    setDefaultNodeShape(NodeShape shape);
    void    setNodeSize(NodeSize size);
    void    setLineThickness(int thickness);
    void    setArrowStyle(ArrowStyle style);

    // Сохранённое состояние
    void        addRecentFile(const QString& path);
    QStringList getRecentOpenFiles() const;
    void        removeRecentFile(const QString& path);

private:
    ApplicationSettings();
    ~ApplicationSettings() = default;

    void loadSettings(const QString& configPath);
    void saveSettings(const QString& configPath) const;

    void fixErrors();

    // Преобразования для сохранения и загрузки
    QString themeToString(Theme theme) const;
    Theme   stringToTheme(const QString& str) const;

    // Основные свойства
    Theme       m_themeType             = Theme::System;
    bool        m_needConfirmDeletion   = true;
    int         m_autoSaveIntervalSec   = 300; // 5 минут
    bool        m_removeMetadata        = false;
    bool        m_cleanupTempFiles      = false;
    unsigned    m_maxLogFiles           = 10;
    bool        m_minimizeToTray        = false;
    QString     m_dateTimeFormat        = "yyyy-MM-dd HH:mm:ss";

    // Полотно
    QSize       m_canvasSize;
    int         m_canvasOpacity         = 90;
    QGradient   m_backgroundGradient    = QLinearGradient(0, 100, 0, 100);
    bool        m_isGridEnabled         = false;
    double      m_gridSize              = 20.0;

    // Свойства объектов
    NodeShape   m_defaultNodeShape      = NodeShape::Circle;
    NodeSize    m_nodeSize              = NodeSize::Medium;
    int         m_lineThickness         = 2;
    ArrowStyle  m_arrowStyle            = ArrowStyle::Triangle;

    // Сохранённое состояние
    QStringList m_recentFiles;
};

#endif // APPLICATIONSETTINGS_H
