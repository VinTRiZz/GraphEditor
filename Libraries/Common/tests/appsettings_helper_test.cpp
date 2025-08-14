#include <gtest/gtest.h>
#include <QCoreApplication>
#include <QSettings>
#include <QTemporaryFile>
#include <QColor>
#include "applicationsettingshelper.h"
#include "commonfunctions.h"

#define EXPECT_COLOR_EQ(colorA, colorB) \
    EXPECT_EQ(CommonFunctions::encodeColor(colorA), CommonFunctions::encodeColor(colorB))

using namespace ApplicationSettingsHelper;

// Тест преобразования темы
TEST(GeneralConfigurationTest, ThemeConversion) {
    GeneralConfiguration config;

    // Проверка преобразования всех значений
    EXPECT_EQ(config.themeToString(GeneralConfiguration::Theme::System), "System");
    EXPECT_EQ(config.themeToString(GeneralConfiguration::Theme::Light), "Light");
    EXPECT_EQ(config.themeToString(GeneralConfiguration::Theme::Dark), "Dark");

    // Проверка обратного преобразования
    EXPECT_EQ(config.stringToTheme("System"), GeneralConfiguration::Theme::System);
    EXPECT_EQ(config.stringToTheme("Light"), GeneralConfiguration::Theme::Light);
    EXPECT_EQ(config.stringToTheme("Dark"), GeneralConfiguration::Theme::Dark);

    // Проверка неверных значений
    EXPECT_EQ(config.stringToTheme("Invalid"), GeneralConfiguration::Theme::System);
    EXPECT_EQ(config.stringToTheme(""), GeneralConfiguration::Theme::System);
}

// Тест сериализации
TEST(GeneralConfigurationTest, Serialization) {
    GeneralConfiguration original;
    original.setThemeType(GeneralConfiguration::Theme::Light);
    original.setNeedConfirmClose(false);
    original.setAutoSaveInterval(600);
    original.setNeedRemoveMetadata(true);
    original.setNeedCleanupTempFiles(false);
    original.setMaxLogFileCount(15);
    original.setMinimizeToTray(true);
    original.setDateTimeFormat("yyyy/MM/dd");

    // Сохраняем настройки
    QSettings settingsFile("test.ini", QSettings::IniFormat);
    original.addToSettingsFile(settingsFile);
    settingsFile.sync();

    // Загружаем в новый объект
    GeneralConfiguration loaded;
    loaded.fromSettingsFile(settingsFile);

    // Сравниваем
    EXPECT_EQ(loaded.getThemeType(), original.getThemeType());
    EXPECT_EQ(loaded.getNeedConfirmClose(), original.getNeedConfirmClose());
    EXPECT_EQ(loaded.getNeedBackwardCompatible(), original.getNeedBackwardCompatible());
    EXPECT_EQ(loaded.getAutoSaveInterval(), original.getAutoSaveInterval());
    EXPECT_EQ(loaded.getNeedRemoveMetadata(), original.getNeedRemoveMetadata());
    EXPECT_EQ(loaded.getNeedCleanupTempFiles(), original.getNeedCleanupTempFiles());
    EXPECT_EQ(loaded.getMaxLogFileCount(), original.getMaxLogFileCount());
    EXPECT_EQ(loaded.getNeedMinimizeToTray(), original.getNeedMinimizeToTray());
    EXPECT_EQ(loaded.getDateTimeFormat(), original.getDateTimeFormat());
}

// Тест сериализации
TEST(CanvasConfigurationTest, Serialization) {
    const QColor bgColor(255, 0, 0, 100);
    const QColor gridColor(0, 255, 0);
    const QColor canvasColor(0, 0, 255);
    const QSize canvasSize(800, 600);
    const int gridLineWidth {3};
    const int gridSize {15};
    const int canvasOpacity {75};

    CanvasConfiguration original;
    original.setCanvasSize(canvasSize);
    original.setCanvasOpacity(canvasOpacity);
    original.setBackgroundColor(bgColor);
    original.setGridColor(gridColor);
    original.setIsGridEnabled(true);
    original.setGridSize(gridSize);
    original.setCanvasColor(canvasColor);
    original.setGridLineWidth(gridLineWidth);

    // Сохраняем настройки
    QSettings settingsFile("test.ini", QSettings::IniFormat);
    original.addToSettingsFile(settingsFile);
    settingsFile.sync();

    // Загружаем в новый объект
    CanvasConfiguration loaded;
    loaded.fromSettingsFile(settingsFile);

    // Проверка значений
#define TST_CHECK_LOADED_EQ(checkmethod, classMethod) \
    checkmethod(original.classMethod(), loaded.classMethod());

    TST_CHECK_LOADED_EQ(EXPECT_COLOR_EQ, getBackgroundColor);
    TST_CHECK_LOADED_EQ(EXPECT_COLOR_EQ, getGridColor);
    TST_CHECK_LOADED_EQ(EXPECT_COLOR_EQ, getCanvasColor);

    TST_CHECK_LOADED_EQ(EXPECT_EQ, getCanvasOpacity);
    TST_CHECK_LOADED_EQ(EXPECT_EQ, getCanvasSize);
    TST_CHECK_LOADED_EQ(EXPECT_EQ, getIsGridEnabled);
    TST_CHECK_LOADED_EQ(EXPECT_EQ, getGridSize);
    TST_CHECK_LOADED_EQ(EXPECT_EQ, getGridLineWidth);
#undef TST_CHECK_LOADED_EQ
}

// Тест геттеров/сеттеров
TEST(ObjectsConfigurationTest, GettersSetters) {
    ObjectsConfiguration config;

    // Установка значений
    QColor nodeMain(255, 0, 0);
    QColor nodeSecond(0, 255, 0);
    QColor nodeSelection(0, 0, 255);
    QColor labelText(128, 128, 0);
    QColor labelBg(0, 128, 128);
    QColor lineMain(64, 128, 192);
    QColor lineSecond(192, 128, 64);
    QColor lineSelection(128, 64, 192);

    config.setDefaultNodeShape(ObjectsConfiguration::NodeShape::Hexagon);
    config.setNodeSize(ObjectsConfiguration::NodeSize::ExtraLarge);
    config.setLineThickness(3);
    config.setArrowStyle(ObjectsConfiguration::ArrowStyle::Circle);

    config.setNodeMainColor(nodeMain);
    config.setNodeSecondColor(nodeSecond);
    config.setNodeSelectionColor(nodeSelection);

    config.setLabelTextColor(labelText);
    config.setLabelBackgroundColor(labelBg);

    config.setLineMainColor(lineMain);
    config.setLineSecondColor(lineSecond);
    config.setLineSelectionColor(lineSelection);

    // Проверка значений
    EXPECT_EQ(config.getDefaultNodeShape(), ObjectsConfiguration::NodeShape::Hexagon);
    EXPECT_EQ(config.getNodeSize(), ObjectsConfiguration::NodeSize::ExtraLarge);
    EXPECT_EQ(config.getLineThickness(), 3);
    EXPECT_EQ(config.getArrowStyle(), ObjectsConfiguration::ArrowStyle::Circle);

    EXPECT_COLOR_EQ(config.getNodeMainColor(), nodeMain);
    EXPECT_COLOR_EQ(config.getNodeSecondColor(), nodeSecond);
    EXPECT_COLOR_EQ(config.getNodeSelectionColor(), nodeSelection);

    EXPECT_COLOR_EQ(config.getLabelTextColor(), labelText);
    EXPECT_COLOR_EQ(config.getLabelBackgroundColor(), labelBg);

    EXPECT_COLOR_EQ(config.getLineMainColor(), lineMain);
    EXPECT_COLOR_EQ(config.getLineSecondColor(), lineSecond);
    EXPECT_COLOR_EQ(config.getLineSelectionColor(), lineSelection);
}

// Тест сериализации
TEST(ObjectsConfigurationTest, Serialization) {
    ObjectsConfiguration original;

    // Установка нестандартных значений
    original.setDefaultNodeShape(ObjectsConfiguration::NodeShape::Triangle);
    original.setNodeSize(ObjectsConfiguration::NodeSize::Small);
    original.setLineThickness(4);
    original.setArrowStyle(ObjectsConfiguration::ArrowStyle::Diamond);

    original.setNodeMainColor(QColor(10, 20, 30));
    original.setNodeSecondColor(QColor(40, 50, 60));
    original.setNodeSelectionColor(QColor(70, 80, 90));

    original.setLabelTextColor(QColor(100, 110, 120));
    original.setLabelBackgroundColor(QColor(130, 140, 150));

    original.setLineMainColor(QColor(160, 170, 180));
    original.setLineSecondColor(QColor(190, 200, 210));
    original.setLineSelectionColor(QColor(220, 230, 240));

    // Сохраняем настройки
    QSettings settingsFile("test.ini", QSettings::IniFormat);
    original.addToSettingsFile(settingsFile);
    settingsFile.sync();

    // Загружаем в новый объект
    ObjectsConfiguration loaded;
    loaded.fromSettingsFile(settingsFile);

    // Сравниваем
    EXPECT_EQ(loaded.getDefaultNodeShape(), original.getDefaultNodeShape());
    EXPECT_EQ(loaded.getNodeSize(), original.getNodeSize());
    EXPECT_EQ(loaded.getLineThickness(), original.getLineThickness());
    EXPECT_EQ(loaded.getArrowStyle(), original.getArrowStyle());

    EXPECT_COLOR_EQ(loaded.getNodeMainColor(), original.getNodeMainColor());
    EXPECT_COLOR_EQ(loaded.getNodeSecondColor(), original.getNodeSecondColor());
    EXPECT_COLOR_EQ(loaded.getNodeSelectionColor(), original.getNodeSelectionColor());

    EXPECT_COLOR_EQ(loaded.getLabelTextColor(), original.getLabelTextColor());
    EXPECT_COLOR_EQ(loaded.getLabelBackgroundColor(), original.getLabelBackgroundColor());

    EXPECT_COLOR_EQ(loaded.getLineMainColor(), original.getLineMainColor());
    EXPECT_COLOR_EQ(loaded.getLineSecondColor(), original.getLineSecondColor());
    EXPECT_COLOR_EQ(loaded.getLineSelectionColor(), original.getLineSelectionColor());
}

int main(int argc, char** argv) {
    QCoreApplication app(argc, argv); // Инициализация для работы с QColor и QSettings
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
