#ifndef APPLICATIONSETTINGSHELPER_H
#define APPLICATIONSETTINGSHELPER_H

#include <QDir>
#include <QColor>
#include <QGradient>
#include <QSettings>
#include <QString>
#include <QStringList>

namespace ApplicationSettingsHelper
{
/**
 * @class GeneralConfiguration
 * @brief Содержит основные настройки приложения, не связанные с графическим интерфейсом.
 */
class GeneralConfiguration
{
public:
    /**
    * @brief The Theme enum Тема оформления
    */
    enum class Theme { System, Light, Dark };

    /**
     * @brief Загружает настройки из QSettings-файла
     * @param iFile - Файл настроек в формате QSettings
     * @note Формально объект iFile не изменяется, но метод не помечен const, поэтому ссылка не константная
     */
    void fromSettingsFile(QSettings &iFile);

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

    // Геттеры
    Theme getThemeType() const;          ///< Возвращает текущую тему оформления
    bool getNeedConfirmClose() const;    ///< Проверяет, требуется ли подтверждение при закрытии
    int getAutoSaveInterval() const;     ///< Возвращает интервал автосохранения (в секундах)
    bool getNeedRemoveMetadata() const;  ///< Проверяет, нужно ли удалять метаданные
    bool getNeedCleanupTempFiles() const;///< Проверяет, нужно ли очищать временные файлы
    unsigned getMaxLogFileCount() const; ///< Возвращает максимальное количество файлов логов
    bool getNeedMinimizeToTray() const;  ///< Проверяет, нужно ли сворачивать в трей
    QString getDateTimeFormat() const;   ///< Возвращает формат даты/времени

    // Сеттеры
    void setThemeType(Theme theme);             ///< Устанавливает тему оформления
    void setNeedConfirmClose(bool confirm);     ///< Включает/отключает подтверждение закрытия
    void setAutoSaveInterval(int seconds);      ///< Устанавливает интервал автосохранения
    void setNeedRemoveMetadata(bool remove);    ///< Включает/отключает удаление метаданных
    void setNeedCleanupTempFiles(bool cleanup); ///< Включает/отключает очистку временных файлов
    void setMaxLogFileCount(int maxFiles);      ///< Устанавливает лимит файлов логов
    void setMinimizeToTray(bool minimize);      ///< Включает/отключает сворачивание в трей
    void setDateTimeFormat(const QString& format); ///< Устанавливает формат даты/времени

private:
    Theme       m_themeType = Theme::System;
    bool        m_needConfirmSave = true;
    int         m_autoSaveIntervalSec = 300;  // 5 минут
    bool        m_removeMetadata = false;
    bool        m_cleanupTempFiles = false;
    unsigned    m_maxLogFiles = 10;
    bool        m_minimizeToTray = false;
    QString     m_dateTimeFormat = "yyyy-MM-dd HH:mm:ss";
};



/**
 * @class CanvasConfiguration
 * @brief Содержит настройки, связанные с визуальным представлением рабочего полотна.
 */
class CanvasConfiguration
{
public:
    /**
     * @brief Загружает настройки из QSettings-файла
     * @param iFile - Файл настроек в формате QSettings
     * @note Формально объект iFile не изменяется, но метод не помечен const, поэтому ссылка не константная
     */
    void fromSettingsFile(QSettings& iFile);

    /**
     * @brief addToSettingsFile Добавляет свойства структуры в QSettings-файл
     * @param iFile             Целевой файл
     */
    void addToSettingsFile(QSettings& iFile) const;


    // Геттеры
    QSize   getCanvasSize() const;          ///< Возвращает размер холста по умолчанию
    int     getCanvasOpacity() const;       ///< Возвращает прозрачность холста (0-100%)
    QColor  getBackgroundColor() const;     ///< Возвращает цвет фона холста
    QColor  getGridColor() const;           ///< Возвращает цвет сетки
    bool    getIsGridEnabled() const;       ///< Проверяет, включена ли сетка
    int     getGridSize() const;            ///< Возвращает размер ячейки сетки
    double  getGridLineWidth() const;       ///< Возвращает ширину линий сетки
    QColor  getCanvasColor() const;         ///< Возвращает цвет полотна

    // Сеттеры
    void setCanvasOpacity(int opacity);          ///< Устанавливает прозрачность холста
    void setBackgroundColor(const QColor &col);  ///< Устанавливает цвет фона
    void setGridColor(const QColor& col);        ///< Устанавливает цвет сетки
    void setIsGridEnabled(bool hasGrid);         ///< Включает/отключает сетку
    void setGridSize(int size);                  ///< Устанавливает размер ячейки сетки
    void setCanvasSize(const QSize& canvasSize); ///< Устанавливает размер холста
    void setGridLineWidth(double gridLineWidth); ///< Устанавливает ширину линий
    void setCanvasColor(const QColor& iCol);     ///< Устанавливает цвет полотна

private:
    QSize   m_canvasSize {QSize(2000, 1000)};
    int     m_canvasOpacity = 90;
    QColor  m_backgroundColor {QColor(170, 170, 170)};
    QColor  m_gridColor {QColor(15, 90, 180, 100)};
    bool    m_isGridEnabled = false;
    int     m_gridSize = 10;
    double  m_gridLineWidth {1};
    QColor  m_canvasColor {QColor(200, 200, 200)};
};



/**
 * @class ObjectsConfiguration
 * @brief Содержит настройки графических элементов: узлов, стрелок, линий и меток.
 */
class ObjectsConfiguration
{
public:
    /**
     * @brief Загружает настройки из QSettings-файла
     * @param iFile - Файл настроек в формате QSettings
     * @note Формально объект iFile не изменяется, но метод не помечен const, поэтому ссылка не константная
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

    // Геттеры
    NodeShape getDefaultNodeShape() const;       ///< Возвращает форму узлов по умолчанию
    NodeSize getNodeSize() const;                ///< Возвращает размер узлов
    int getLineThickness() const;                ///< Возвращает толщину линий
    ArrowStyle getArrowStyle() const;            ///< Возвращает стиль стрелок

    QColor getNodeMainColor() const;             ///< Возвращает основной цвет узлов
    QColor getNodeSecondColor() const;           ///< Возвращает дополнительный цвет узлов
    QColor getNodeSelectionColor() const;        ///< Возвращает цвет выделения узлов

    QColor getLabelTextColor() const;            ///< Возвращает цвет текста меток
    QColor getLabelBackgroundColor() const;      ///< Возвращает цвет фона меток

    QColor getLineMainColor() const;             ///< Возвращает основной цвет линий
    QColor getLineSecondColor() const;           ///< Возвращает дополнительный цвет линий
    QColor getLineSelectionColor() const;        ///< Возвращает цвет выделения линий

    // Сеттеры
    void setDefaultNodeShape(NodeShape shape);   ///< Устанавливает форму узлов
    void setNodeSize(NodeSize size);             ///< Устанавливает размер узлов
    void setLineThickness(int thickness);        ///< Устанавливает толщину линий
    void setArrowStyle(ArrowStyle style);        ///< Устанавливает стиль стрелок


    void setNodeMainColor(const QColor& iCol);      ///< Устанавливает основной цвет узлов
    void setNodeSecondColor(const QColor& iCol);    ///< Устанавливает доп. цвет узлов
    void setNodeSelectionColor(const QColor& iCol); ///< Устанавливает цвет выделения узлов

    void setLabelTextColor(const QColor& iCol);         ///< Устанавливает цвет текста меток
    void setLabelBackgroundColor(const QColor& iCol);   ///< Устанавливает цвет фона меток

    void setLineMainColor(const QColor& col);       ///< Устанавливает основной цвет линий
    void setLineSecondColor(const QColor& col);     ///< Устанавливает доп. цвет линий
    void setLineSelectionColor(const QColor& col);  ///< Устанавливает цвет выделения линий

private:
    NodeShape   m_defaultNodeShape = NodeShape::Circle;
    NodeSize    m_nodeSize = NodeSize::Medium;
    int         m_lineThickness = 2;
    ArrowStyle  m_arrowStyle = ArrowStyle::Triangle;

    QColor      m_defaultMainColor          {Qt::black};
    QColor      m_defaultSecondColor        {QColor("#ecd1a6")};
    QColor      m_defaultSelectionColor     {QColor("#5cff37")};

    QColor      m_defaultLabelTextColor         {Qt::black};
    QColor      m_defaultLabelBackgroundColor   {QColor("#fcf0f0")};

    QColor      m_defaultLineMainColor      {QColor("#2a8d7c")};
    QColor      m_defaultLineSecondColor    {QColor("#5aadac")};
    QColor      m_defaultLineSelectionColor {QColor("#ffbc20")};
};
}


#endif // APPLICATIONSETTINGSHELPER_H
