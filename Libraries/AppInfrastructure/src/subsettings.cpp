#include "subsettings.h"

#include <Components/Common/CommonFunctions.h>

using namespace SubSettings;
using namespace CommonFunctions;

namespace SubSettings {

void GeneralConfiguration::fromSettingsFile(QSettings& iFile) {
    m_autoSaveIntervalSec =
        iFile.value("autosave_interval", m_autoSaveIntervalSec).toInt();
    m_cleanupTempFiles =
        iFile.value("cleanup_temp_files", m_cleanupTempFiles).toBool();
    m_needConfirmSave =
        iFile.value("confirm_deletion", m_needConfirmSave).toBool();
    m_dateTimeFormat =
        iFile.value("datetime_format", "yyyy-MM-dd HH:mm:ss").toString();

    m_maxLogFiles = iFile.value("max_log_files", m_maxLogFiles).toInt();
    m_minimizeToTray =
        iFile.value("minimize_to_tray", m_minimizeToTray).toBool();
    m_needBackwardCompatibility =
        iFile.value("need_compatibility", m_needBackwardCompatibility).toBool();
    m_removeMetadata =
        iFile.value("remove_metadata", m_removeMetadata).toBool();
    m_themeType = stringToTheme(iFile.value("theme").toString());
}

void GeneralConfiguration::addToSettingsFile(QSettings& iFile) const {
    iFile.setValue("autosave_interval", m_autoSaveIntervalSec);
    iFile.setValue("cleanup_temp_files", m_cleanupTempFiles);
    iFile.setValue("confirm_deletion", m_needConfirmSave);
    iFile.setValue("datetime_format", m_dateTimeFormat);
    iFile.setValue("max_log_files", m_maxLogFiles);
    iFile.setValue("minimize_to_tray", m_minimizeToTray);
    iFile.setValue("need_compatibility", m_needBackwardCompatibility);
    iFile.setValue("theme", themeToString(m_themeType));
    iFile.setValue("remove_metadata", m_removeMetadata);
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

// ================================================================ //

void CanvasConfiguration::fromSettingsFile(QSettings& iFile) {
    auto setColorIfExist = [&](QColor& targetColor, const QString& valueName) {
        if (iFile.contains(valueName)) {
            targetColor = decodeColor(iFile.value(valueName).toString());
        }
    };
    setColorIfExist(m_backgroundColor, "background_color");
    setColorIfExist(m_gridColor, "grid_color");
    setColorIfExist(m_canvasColor, "canvas_color");

    m_canvasSize = iFile.value("canvas_size", m_canvasSize).toSize();
    m_canvasOpacity = iFile.value("canvas_opacity", m_canvasOpacity).toInt();
    m_isGridEnabled = iFile.value("show_grid", m_isGridEnabled).toBool();
    m_gridSize = iFile.value("grid_size", m_gridSize).toInt();
    m_gridLineWidth =
        iFile.value("grid_line_width", m_gridLineWidth).toDouble();
}

void CanvasConfiguration::addToSettingsFile(QSettings& iFile) const {
    iFile.setValue("canvas_size", m_canvasSize);
    iFile.setValue("canvas_opacity", m_canvasOpacity);
    iFile.setValue("show_grid", m_isGridEnabled);
    iFile.setValue("grid_size", m_gridSize);
    iFile.setValue("grid_line_width", m_gridLineWidth);

    auto setColorValue = [&](const QColor& targetColor,
                             const QString& valueName) {
        iFile.setValue(valueName, encodeColor(targetColor));
    };
    setColorValue(m_backgroundColor, "background_color");
    setColorValue(m_gridColor, "grid_color");
    setColorValue(m_canvasColor, "canvas_color");
}

// ================================================================ //

void ObjectsConfiguration::fromSettingsFile(QSettings& iFile) {
    auto setColorIfExist = [&](QColor& targetColor, const QString& valueName) {
        if (iFile.contains(valueName)) {
            targetColor = decodeColor(iFile.value(valueName).toString());
        }
    };
    setColorIfExist(m_defaultMainColor, "default_main_color");
    setColorIfExist(m_defaultSecondColor, "default_second_color");
    setColorIfExist(m_defaultSelectionColor, "default_selection_color");

    setColorIfExist(m_defaultLineMainColor, "default_line_main_color");
    setColorIfExist(m_defaultLineSecondColor, "default_line_second_color");
    setColorIfExist(m_defaultLineSelectionColor,
                    "default_line_selection_color");

    setColorIfExist(m_defaultLabelTextColor, "default_label_text_color");
    setColorIfExist(m_defaultLabelBackgroundColor, "default_label_bgr_color");

    m_defaultNodeShape =
        static_cast<NodeShape>(iFile.value("node_shape").toInt());
    m_nodeSize = static_cast<NodeSize>(iFile.value("node_size").toInt());
    m_lineThickness = iFile.value("line_thickness", m_lineThickness).toInt();
    m_arrowStyle = static_cast<ArrowStyle>(iFile.value("arrow_style").toInt());
}

void ObjectsConfiguration::addToSettingsFile(QSettings& iFile) const {
    iFile.setValue("node_shape", static_cast<int>(m_defaultNodeShape));
    iFile.setValue("node_size", static_cast<int>(m_nodeSize));
    iFile.setValue("line_thickness", m_lineThickness);
    iFile.setValue("arrow_style", static_cast<int>(m_arrowStyle));

    auto setColorValue = [&](const QColor& targetColor,
                             const QString& valueName) {
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

}  // namespace SubSettings
