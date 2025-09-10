#include <gtest/gtest.h>

#include <QColor>
#include <QCoreApplication>
#include <QSettings>
#include <QTemporaryFile>

#include "applicationsettingshelper.h"
#include "commonfunctions.h"

#define EXPECT_COLOR_EQ(colorA, colorB)                                        \
    EXPECT_EQ(CommonFunctions::encodeColor(colorA),                            \
              CommonFunctions::encodeColor(colorB))

using namespace ApplicationSettingsHelper;

TEST(GeneralConfigurationTest, ThemeConversion) {
    GeneralConfiguration config;

    EXPECT_EQ(config.themeToString(GeneralConfiguration::Theme::System), "System");
    EXPECT_EQ(config.themeToString(GeneralConfiguration::Theme::Light), "Light");
    EXPECT_EQ(config.themeToString(GeneralConfiguration::Theme::Dark), "Dark");

    EXPECT_EQ(config.stringToTheme("System"), GeneralConfiguration::Theme::System);
    EXPECT_EQ(config.stringToTheme("Light"), GeneralConfiguration::Theme::Light);
    EXPECT_EQ(config.stringToTheme("Dark"), GeneralConfiguration::Theme::Dark);
    EXPECT_EQ(config.stringToTheme("Invalid"), GeneralConfiguration::Theme::System);
    EXPECT_EQ(config.stringToTheme(""), GeneralConfiguration::Theme::System);
}

TEST(GeneralConfigurationTest, Serialization) {
    GeneralConfiguration original;
    original.m_themeType = GeneralConfiguration::Theme::Light;
    original.m_needConfirmSave = false;
    original.m_needBackwardCompatibility = false;
    original.m_autoSaveIntervalSec = 600;
    original.m_removeMetadata = true;
    original.m_cleanupTempFiles = false;
    original.m_maxLogFiles = 15;
    original.m_minimizeToTray = true;
    original.m_dateTimeFormat = "yyyy/MM/dd";

    QTemporaryFile tempFile;
    tempFile.open();
    QSettings settingsFile(tempFile.fileName(), QSettings::IniFormat);
    original.addToSettingsFile(settingsFile);
    settingsFile.sync();

    GeneralConfiguration loaded;
    loaded.fromSettingsFile(settingsFile);

    EXPECT_EQ(loaded.m_themeType, original.m_themeType);
    EXPECT_EQ(loaded.m_needConfirmSave, original.m_needConfirmSave);
    EXPECT_EQ(loaded.m_needBackwardCompatibility, original.m_needBackwardCompatibility);
    EXPECT_EQ(loaded.m_autoSaveIntervalSec, original.m_autoSaveIntervalSec);
    EXPECT_EQ(loaded.m_removeMetadata, original.m_removeMetadata);
    EXPECT_EQ(loaded.m_cleanupTempFiles, original.m_cleanupTempFiles);
    EXPECT_EQ(loaded.m_maxLogFiles, original.m_maxLogFiles);
    EXPECT_EQ(loaded.m_minimizeToTray, original.m_minimizeToTray);
    EXPECT_EQ(loaded.m_dateTimeFormat, original.m_dateTimeFormat);
}

TEST(CanvasConfigurationTest, Serialization) {
    CanvasConfiguration original;
    original.m_canvasSize = QSize(800, 600);
    original.m_canvasOpacity = 75;
    original.m_backgroundColor = QColor(255, 0, 0, 100);
    original.m_gridColor = QColor(0, 255, 0);
    original.m_isGridEnabled = true;
    original.m_gridSize = 15;
    original.m_gridLineWidth = 3;
    original.m_canvasColor = QColor(0, 0, 255);

    QTemporaryFile tempFile;
    tempFile.open();
    QSettings settingsFile(tempFile.fileName(), QSettings::IniFormat);
    original.addToSettingsFile(settingsFile);
    settingsFile.sync();

    CanvasConfiguration loaded;
    loaded.fromSettingsFile(settingsFile);

    EXPECT_EQ(loaded.m_canvasSize, original.m_canvasSize);
    EXPECT_EQ(loaded.m_canvasOpacity, original.m_canvasOpacity);
    EXPECT_COLOR_EQ(loaded.m_backgroundColor, original.m_backgroundColor);
    EXPECT_COLOR_EQ(loaded.m_gridColor, original.m_gridColor);
    EXPECT_EQ(loaded.m_isGridEnabled, original.m_isGridEnabled);
    EXPECT_EQ(loaded.m_gridSize, original.m_gridSize);
    EXPECT_EQ(loaded.m_gridLineWidth, original.m_gridLineWidth);
    EXPECT_COLOR_EQ(loaded.m_canvasColor, original.m_canvasColor);
}

TEST(ObjectsConfigurationTest, Serialization) {
    ObjectsConfiguration original;
    original.m_defaultNodeShape = ObjectsConfiguration::NodeShape::Triangle;
    original.m_nodeSize = ObjectsConfiguration::NodeSize::Small;
    original.m_lineThickness = 4;
    original.m_arrowStyle = ObjectsConfiguration::ArrowStyle::Diamond;
    original.m_defaultMainColor = QColor(10, 20, 30);
    original.m_defaultSecondColor = QColor(40, 50, 60);
    original.m_defaultSelectionColor = QColor(70, 80, 90);
    original.m_defaultLabelTextColor = QColor(100, 110, 120);
    original.m_defaultLabelBackgroundColor = QColor(130, 140, 150);
    original.m_defaultLineMainColor = QColor(160, 170, 180);
    original.m_defaultLineSecondColor = QColor(190, 200, 210);
    original.m_defaultLineSelectionColor = QColor(220, 230, 240);

    QTemporaryFile tempFile;
    tempFile.open();
    QSettings settingsFile(tempFile.fileName(), QSettings::IniFormat);
    original.addToSettingsFile(settingsFile);
    settingsFile.sync();

    ObjectsConfiguration loaded;
    loaded.fromSettingsFile(settingsFile);

    EXPECT_EQ(loaded.m_defaultNodeShape, original.m_defaultNodeShape);
    EXPECT_EQ(loaded.m_nodeSize, original.m_nodeSize);
    EXPECT_EQ(loaded.m_lineThickness, original.m_lineThickness);
    EXPECT_EQ(loaded.m_arrowStyle, original.m_arrowStyle);
    EXPECT_COLOR_EQ(loaded.m_defaultMainColor, original.m_defaultMainColor);
    EXPECT_COLOR_EQ(loaded.m_defaultSecondColor, original.m_defaultSecondColor);
    EXPECT_COLOR_EQ(loaded.m_defaultSelectionColor, original.m_defaultSelectionColor);
    EXPECT_COLOR_EQ(loaded.m_defaultLabelTextColor, original.m_defaultLabelTextColor);
    EXPECT_COLOR_EQ(loaded.m_defaultLabelBackgroundColor, original.m_defaultLabelBackgroundColor);
    EXPECT_COLOR_EQ(loaded.m_defaultLineMainColor, original.m_defaultLineMainColor);
    EXPECT_COLOR_EQ(loaded.m_defaultLineSecondColor, original.m_defaultLineSecondColor);
    EXPECT_COLOR_EQ(loaded.m_defaultLineSelectionColor, original.m_defaultLineSelectionColor);
}

int main(int argc, char** argv) {
    QCoreApplication app(argc, argv);
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
