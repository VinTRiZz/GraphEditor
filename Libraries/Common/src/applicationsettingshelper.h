#ifndef APPLICATIONSETTINGSHELPER_H
#define APPLICATIONSETTINGSHELPER_H

#include <QColor>
#include <QGradient>
#include <QSettings>
#include <QString>
#include <QStringList>

namespace ApplicationSettingsHelper {
/**
 * @class GeneralConfiguration
 * @brief Содержит основные настройки приложения, не связанные с графическим
 * интерфейсом.
 */
class GeneralConfiguration {
public:
    /**
     * @brief The Theme enum Тема оформления
     */
    enum class Theme { System, Light, Dark };

    /**
     * @brief Загружает настройки из QSettings-файла
     * @param iFile - Файл настроек в формате QSettings
     * @note Формально объект iFile не изменяется, но метод не помечен const,
     * поэтому ссылка не константная
     */
    void fromSettingsFile(QSettings& iFile);

    /**
     * @brief addToSettingsFile Добавляет свойства структуры в QSettings-файл
     * @param iFile             Целевой файл
     */
    void addToSettingsFile(QSettings& iFile) const;

    /**
     * @brief Преобразует значение Theme в строковое представление
     * @param theme - Значение перечисления Theme
     * @return Строковый идентификатор темы
     */
    QString themeToString(Theme theme) const;

    /**
     * @brief Преобразует строку в значение Theme
     * @param str - Строковый идентификатор темы
     * @return Значение перечисления Theme (Theme::System по умолчанию)
     */
    Theme stringToTheme(const QString& str) const;

    Theme m_themeType = Theme::System;
    bool m_needConfirmSave = true;
    bool m_needBackwardCompatibility = false;
    int m_autoSaveIntervalSec = 300;  // 5 минут
    bool m_removeMetadata = false;
    bool m_cleanupTempFiles = false;
    unsigned m_maxLogFiles = 10;
    bool m_minimizeToTray = false;
    QString m_dateTimeFormat = "yyyy-MM-dd HH:mm:ss";
};

/**
 * @class CanvasConfiguration
 * @brief Содержит настройки, связанные с визуальным представлением рабочего
 * полотна.
 */
class CanvasConfiguration {
public:
    /**
     * @brief Загружает настройки из QSettings-файла
     * @param iFile - Файл настроек в формате QSettings
     * @note Формально объект iFile не изменяется, но метод не помечен const,
     * поэтому ссылка не константная
     */
    void fromSettingsFile(QSettings& iFile);

    /**
     * @brief addToSettingsFile Добавляет свойства структуры в QSettings-файл
     * @param iFile             Целевой файл
     */
    void addToSettingsFile(QSettings& iFile) const;

    QSize m_canvasSize{QSize(2000, 1000)};
    int m_canvasOpacity = 90;
    QColor m_backgroundColor{QColor(170, 170, 170)};
    QColor m_gridColor{QColor(15, 90, 180, 100)};
    bool m_isGridEnabled = false;
    int m_gridSize = 10;
    double m_gridLineWidth{1};
    QColor m_canvasColor{QColor(200, 200, 200)};
};

/**
 * @class ObjectsConfiguration
 * @brief Содержит настройки графических элементов: узлов, стрелок, линий и
 * меток.
 */
class ObjectsConfiguration {
public:
    /**
     * @brief Загружает настройки из QSettings-файла
     * @param iFile - Файл настроек в формате QSettings
     * @note Формально объект iFile не изменяется, но метод не помечен const,
     * поэтому ссылка не константная
     */
    void fromSettingsFile(QSettings& iFile);

    /**
     * @brief addToSettingsFile Добавляет свойства структуры в QSettings-файл
     * @param iFile             Целевой файл
     */
    void addToSettingsFile(QSettings& iFile) const;

    /**
     * @brief The NodeShape enum Тип фигуры вершины по умолчанию
     */
    enum class NodeShape {
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
    enum class NodeSize { Small, Medium, Large, ExtraLarge };

    /**
     * @brief The ArrowStyle enum Стиль стрелок
     */
    enum class ArrowStyle {
        None,
        Triangle,
        Diamond,
        Arrow,
        Circle,
    };

    NodeShape m_defaultNodeShape = NodeShape::Circle;
    NodeSize m_nodeSize = NodeSize::Medium;
    int m_lineThickness = 2;
    ArrowStyle m_arrowStyle = ArrowStyle::Triangle;

    QColor m_defaultMainColor{Qt::black};
    QColor m_defaultSecondColor{QColor("#ecd1a6")};
    QColor m_defaultSelectionColor{QColor("#5cff37")};

    QColor m_defaultLabelTextColor{Qt::black};
    QColor m_defaultLabelBackgroundColor{QColor("#fcf0f0")};

    QColor m_defaultLineMainColor{QColor("#2a8d7c")};
    QColor m_defaultLineSecondColor{QColor("#5aadac")};
    QColor m_defaultLineSelectionColor{QColor("#ffbc20")};
};
}  // namespace ApplicationSettingsHelper

#endif  // APPLICATIONSETTINGSHELPER_H
