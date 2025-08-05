#include "applicationsettingshelper.h"

#include "commonfunctions.h"
using namespace CommonFunctions;

namespace ApplicationSettingsHelper
{

void GeneralConfiguration::fromSettingsFile(QSettings& iFile) {
    m_themeType = stringToTheme(iFile.value("theme", "System").toString());
    m_autoSaveIntervalSec = iFile.value("autosave_interval", 300).toInt();
    m_minimizeToTray = iFile.value("minimize_to_tray", true).toBool();
    m_dateTimeFormat =
        iFile.value("datetime_format", "yyyy-MM-dd HH:mm:ss").toString();

    m_needConfirmSave = iFile.value("confirm_deletion", true).toBool();
    m_removeMetadata = iFile.value("remove_metadata", false).toBool();
    m_cleanupTempFiles = iFile.value("cleanup_temp_files", true).toBool();
    m_maxLogFiles = iFile.value("max_log_files", 10).toInt();
}

void GeneralConfiguration::addToSettingsFile(QSettings &iFile) const {
    iFile.setValue("theme", themeToString(m_themeType));
    iFile.setValue("autosave_interval", m_autoSaveIntervalSec);
    iFile.setValue("minimize_to_tray", m_minimizeToTray);
    iFile.setValue("datetime_format", m_dateTimeFormat);

    iFile.setValue("remove_metadata", m_removeMetadata);
    iFile.setValue("cleanup_temp_files", m_cleanupTempFiles);
    iFile.setValue("max_log_files", m_maxLogFiles);

    iFile.setValue("confirm_deletion", m_needConfirmSave);
}


QString GeneralConfiguration::themeToString(Theme theme) const {
    switch (theme) {
        case Theme::Light:
            return "Light";
        case Theme::Dark:
            return "Dark";
        default:
            return "System";
    }
}

GeneralConfiguration::Theme GeneralConfiguration::stringToTheme(
    const QString& str) const {
    if (str == "Light")
        return Theme::Light;
    if (str == "Dark")
        return Theme::Dark;
    return Theme::System;
}

GeneralConfiguration::Theme GeneralConfiguration::getThemeType() const {
    return m_themeType;
}

bool GeneralConfiguration::getNeedConfirmClose() const {
    return m_needConfirmSave;
}

int GeneralConfiguration::getAutoSaveInterval() const {
    return m_autoSaveIntervalSec;
}

bool GeneralConfiguration::getNeedRemoveMetadata() const {
    return m_removeMetadata;
}

bool GeneralConfiguration::getNeedCleanupTempFiles() const {
    return m_cleanupTempFiles;
}

unsigned GeneralConfiguration::getMaxLogFileCount() const {
    return m_maxLogFiles;
}

bool GeneralConfiguration::getNeedMinimizeToTray() const {
    return m_minimizeToTray;
}

QString GeneralConfiguration::getDateTimeFormat() const {
    return m_dateTimeFormat;
}

void GeneralConfiguration::setThemeType(Theme theme) {
    m_themeType = theme;
}

void GeneralConfiguration::setNeedConfirmClose(bool confirm) {
    m_needConfirmSave = confirm;
}

void GeneralConfiguration::setAutoSaveInterval(int seconds) {
    m_autoSaveIntervalSec = seconds;
}

void GeneralConfiguration::setNeedRemoveMetadata(bool remove) {
    m_removeMetadata = remove;
}

void GeneralConfiguration::setNeedCleanupTempFiles(bool cleanup) {
    m_cleanupTempFiles = cleanup;
}

void GeneralConfiguration::setMaxLogFileCount(int maxFiles) {
    m_maxLogFiles = maxFiles;
}

void GeneralConfiguration::setMinimizeToTray(bool minimize) {
    m_minimizeToTray = minimize;
}

void GeneralConfiguration::setDateTimeFormat(const QString& format) {
    m_dateTimeFormat = format;
}


// ================================================================ //

void CanvasConfiguration::fromSettingsFile(QSettings &iFile) {
    auto setColorIfExist = [&](QColor& targetColor, const QString& valueName){
        if (iFile.contains(valueName)) {
            targetColor = decodeColor(iFile.value(valueName).toString());
        }
    };
    setColorIfExist(m_backgroundColor, "background_color");
    setColorIfExist(m_gridColor, "grid_color");
    setColorIfExist(m_canvasColor, "canvas_color");

    m_canvasSize = iFile.value("canvas_size").toSize();
    m_canvasOpacity = iFile.value("canvas_opacity", 90).toInt();
    m_isGridEnabled = iFile.value("show_grid", true).toBool();
    m_gridSize = iFile.value("grid_size", 20.0).toDouble();
}

void CanvasConfiguration::addToSettingsFile(QSettings &iFile) const {
    iFile.setValue("canvas_size", m_canvasSize);
    iFile.setValue("canvas_opacity", m_canvasOpacity);
    iFile.setValue("show_grid", m_isGridEnabled);
    iFile.setValue("grid_size", m_gridSize);

    auto setColorValue = [&](const QColor& targetColor, const QString& valueName){
        iFile.setValue(valueName, encodeColor(targetColor));
    };
    setColorValue(m_backgroundColor, "background_color");
    setColorValue(m_gridColor, "grid_color");
    setColorValue(m_canvasColor, "canvas_color");
}



QSize CanvasConfiguration::getCanvasSize() const {
    return m_canvasSize;
}

int CanvasConfiguration::getCanvasOpacity() const {
    return m_canvasOpacity;
}

QColor CanvasConfiguration::getBackgroundColor() const {
    return m_backgroundColor;
}

QColor CanvasConfiguration::getGridColor() const {
    return m_gridColor;
}

bool CanvasConfiguration::getIsGridEnabled() const {
    return m_isGridEnabled;
}

double CanvasConfiguration::getGridSize() const {
    return m_gridSize;
}

void CanvasConfiguration::setCanvasOpacity(int opacity) {
    m_canvasOpacity = opacity;
}

void CanvasConfiguration::setBackgroundColor(const QColor &col) {
    m_backgroundColor = col;
}

void CanvasConfiguration::setGridColor(const QColor& col) {
    m_gridColor = col;
}

void CanvasConfiguration::setIsGridEnabled(bool hasGrid) {
    m_isGridEnabled = hasGrid;
}

void CanvasConfiguration::setGridSize(double size) {
    m_gridSize = size;
}

void CanvasConfiguration::setCanvasSize(const QSize& canvasSize) {
    m_canvasSize = canvasSize;
}

QColor CanvasConfiguration::getCanvasColor() const {
    return m_canvasColor;
}

void CanvasConfiguration::setCanvasColor(const QColor& iCol) {
    m_canvasColor = iCol;
}





// ================================================================ //

void ObjectsConfiguration::fromSettingsFile(QSettings& iFile) {
    auto setColorIfExist = [&](QColor& targetColor, const QString& valueName){
        if (iFile.contains(valueName)) {
            targetColor = decodeColor(iFile.value(valueName).toString());
        }
    };
    setColorIfExist(m_defaultMainColor, "default_main_color");
    setColorIfExist(m_defaultSecondColor, "default_second_color");
    setColorIfExist(m_defaultSelectionColor, "default_selection_color");

    setColorIfExist(m_defaultLineMainColor, "default_line_main_color");
    setColorIfExist(m_defaultLineSecondColor, "default_line_second_color");
    setColorIfExist(m_defaultLineSelectionColor, "default_line_selection_color");

    setColorIfExist(m_defaultLabelTextColor, "default_label_text_color");
    setColorIfExist(m_defaultLabelBackgroundColor, "default_label_bgr_color");

    m_defaultNodeShape =
        static_cast<NodeShape>(iFile.value("node_shape", 0).toInt());
    m_nodeSize = static_cast<NodeSize>(iFile.value("node_size", 1).toInt());
    m_lineThickness = iFile.value("line_thickness", 2).toInt();
    m_arrowStyle =
        static_cast<ArrowStyle>(iFile.value("arrow_style", 1).toInt());
}

void ObjectsConfiguration::addToSettingsFile(QSettings &iFile) const {
    iFile.setValue("node_shape", static_cast<int>(m_defaultNodeShape));
    iFile.setValue("node_size", static_cast<int>(m_nodeSize));
    iFile.setValue("line_thickness", m_lineThickness);
    iFile.setValue("arrow_style", static_cast<int>(m_arrowStyle));

    auto setColorValue = [&](const QColor& targetColor, const QString& valueName){
        iFile.setValue(valueName, encodeColor(targetColor));
    };
    setColorValue(m_defaultMainColor, "default_main_color");
    setColorValue(m_defaultSecondColor, "default_second_color");
    setColorValue(m_defaultSelectionColor, "default_selection_color");

    setColorValue(m_defaultLineMainColor, "default_line_main_color");
    setColorValue(m_defaultLineSecondColor, "default_line_second_color");
    setColorValue(m_defaultLineSelectionColor, "default_line_selection_color");

    setColorValue(m_defaultLabelTextColor, "default_label_text_color");
    setColorValue(m_defaultLabelBackgroundColor, "default_label_bgr_color");
}


ObjectsConfiguration::NodeShape ObjectsConfiguration::getDefaultNodeShape() const {
    return m_defaultNodeShape;
}

ObjectsConfiguration::NodeSize ObjectsConfiguration::getNodeSize() const {
    return m_nodeSize;
}

int ObjectsConfiguration::getLineThickness() const {
    return m_lineThickness;
}

ObjectsConfiguration::ArrowStyle ObjectsConfiguration::getArrowStyle() const {
    return m_arrowStyle;
}

QColor ObjectsConfiguration::getNodeMainColor() const {
    return m_defaultMainColor;
}

QColor ObjectsConfiguration::getNodeSecondColor() const {
    return m_defaultSecondColor;
}

QColor ObjectsConfiguration::getNodeSelectionColor() const {
    return m_defaultSelectionColor;
}

QColor ObjectsConfiguration::getLabelTextColor() const {
    return m_defaultLabelTextColor;
}

QColor ObjectsConfiguration::getLabelBackgroundColor() const {
    return m_defaultLabelBackgroundColor;
}

QColor ObjectsConfiguration::getLineMainColor() const {
    return m_defaultLineMainColor;
}

QColor ObjectsConfiguration::getLineSecondColor() const {
    return m_defaultLineSecondColor;
}

QColor ObjectsConfiguration::getLineSelectionColor() const {
    return m_defaultLineSelectionColor;
}

void ObjectsConfiguration::setDefaultNodeShape(NodeShape shape) {
    m_defaultNodeShape = shape;
}

void ObjectsConfiguration::setNodeSize(NodeSize size) {
    m_nodeSize = size;
}

void ObjectsConfiguration::setLineThickness(int thickness) {
    m_lineThickness = thickness;
}

void ObjectsConfiguration::setArrowStyle(ArrowStyle style) {
    m_arrowStyle = style;
}

void ObjectsConfiguration::setNodeMainColor(const QColor& iCol) {
    m_defaultMainColor = iCol;
}

void ObjectsConfiguration::setNodeSecondColor(const QColor& iCol) {
    m_defaultSecondColor = iCol;
}

void ObjectsConfiguration::setNodeSelectionColor(const QColor& iCol) {
    m_defaultSelectionColor = iCol;
}

void ObjectsConfiguration::setLabelTextColor(const QColor& iCol) {
    m_defaultLabelTextColor = iCol;
}

void ObjectsConfiguration::setLabelBackgroundColor(const QColor& iCol) {
    m_defaultLabelBackgroundColor = iCol;
}

void ObjectsConfiguration::setLineMainColor(const QColor& col) {
    m_defaultLineMainColor = col;
}

void ObjectsConfiguration::setLineSecondColor(const QColor& col) {
    m_defaultLineSecondColor = col;
}

void ObjectsConfiguration::setLineSelectionColor(const QColor& col) {
    m_defaultLineSelectionColor = col;
}


}
