#include <gtest/gtest.h>

#include <QCoreApplication>
#include <QDebug>
#include <QDir>
#include <QTemporaryFile>

#include "applicationsettings.h"

class ApplicationSettingsTest : public ::testing::Test {
protected:
    static void SetUpTestSuite() {
        auto& inst = ApplicationSettings::getInstance();

        auto& instPath = inst.APPLICATION_SETTINGS_FILE_PATH;
        const_cast<QString&>(instPath) = "test.ini";
    }

    static void TearDownTestSuite() {
        auto& inst = ApplicationSettings::getInstance();
        QFile::remove(inst.APPLICATION_SETTINGS_FILE_PATH);
    }
};

// Тест работы с недавними файлами
TEST_F(ApplicationSettingsTest, HistorySaveAccuracy) {
    QFile tstfile1(QDir::tempPath() + QDir::separator() + "test1.txt");
    tstfile1.open(QIODevice::Truncate | QIODevice::WriteOnly);
    tstfile1.close();

    QFile tstfile2(QDir::tempPath() + QDir::separator() + "test2.txt");
    tstfile2.open(QIODevice::Truncate | QIODevice::WriteOnly);
    tstfile2.close();

    const QString file1 = tstfile1.fileName();
    const QString file2 = tstfile2.fileName();

    auto& settings = ApplicationSettings::getInstance();

    // Добавление файлов
    settings.addRecentFile(file1);
    settings.addRecentFile(file2);

    QStringList recentFiles = settings.getRecentOpenFiles();
    ASSERT_EQ(recentFiles.size(), 2);
    ASSERT_TRUE(recentFiles.contains(file1));
    ASSERT_TRUE(recentFiles.contains(file2));

    // Удаление файла
    settings.removeRecentFile(file1);
    recentFiles = settings.getRecentOpenFiles();
    ASSERT_EQ(recentFiles.size(), 1);
    ASSERT_FALSE(recentFiles.contains(file1));
    ASSERT_TRUE(recentFiles.contains(file2));

    // Удаление несуществующего файла
    EXPECT_NO_THROW(settings.removeRecentFile("/aaaaaaaaaaaa/aaaaaaaaaaaaaaaaaaaaa/aaaaaaaaaaaa/file.txt"));
}

// Тест уникальности файлов в списке
TEST_F(ApplicationSettingsTest, RecentFilesUnique) {
    auto& settings = ApplicationSettings::getInstance();
    settings.clearRecentFiles();

    const QString file = qApp->applicationFilePath();

    settings.addRecentFile(file);
    settings.addRecentFile(file);  // Дублирующее добавление

    QStringList recentFiles = settings.getRecentOpenFiles();
    ASSERT_EQ(recentFiles.size(), 1);
    ASSERT_EQ(recentFiles.first(), file);
}

// Тест получения конфигураций
TEST_F(ApplicationSettingsTest, ConfigurationsAccess) {
    auto& settings = ApplicationSettings::getInstance();

    auto& generalConfig = settings.getGeneralConfig();
    auto& objectsConfig = settings.getObjectsConfig();
    auto& canvasConfig = settings.getCanvasConfig();

    // Проверяем, что возвращаются действительные объекты
    SUCCEED() << "Configuration objects accessed successfully";

    // Проверяем, что возвращаются одни и те же экземпляры
    ASSERT_EQ(&generalConfig, &settings.getGeneralConfig());
    ASSERT_EQ(&objectsConfig, &settings.getObjectsConfig());
    ASSERT_EQ(&canvasConfig, &settings.getCanvasConfig());
}

// Тест загрузки и сохранения настроек
TEST_F(ApplicationSettingsTest, SettingsPersistence) {
    auto& settings = ApplicationSettings::getInstance();
    QString testSettingsFile = "persistancetest.ini";

    // Создаем временный файл для теста
    QTemporaryFile tempFile;
    ASSERT_TRUE(tempFile.open());
    QString tempPath = tempFile.fileName();
    tempFile.close();

    // Сохраняем тестовые данные
    const QString testFile = qApp->applicationFilePath();
    settings.addRecentFile(testFile);

    // Сохраняем в временный файл (используем приватный метод через
    // дружественный тест)
    settings.saveSettings(testSettingsFile);

    // Очищаем текущие настройки
    settings.removeRecentFile(testFile);
    ASSERT_FALSE(settings.getRecentOpenFiles().contains(testFile));

    // Загружаем из временного файла
    settings.loadSettings(testSettingsFile);

    // Проверяем восстановление данных
    ASSERT_TRUE(settings.getRecentOpenFiles().contains(testFile));
}

// Тест публичных методов load/save
TEST_F(ApplicationSettingsTest, SaveLoad) {
    auto& settings = ApplicationSettings::getInstance();
    QString testSettingsFile = "saveload.ini";

    const QString testFile = qApp->applicationFilePath();

    settings.addRecentFile(testFile);
    EXPECT_NO_THROW(settings.saveSettings(testSettingsFile));

    // Очищаем текущее состояние
    settings.removeRecentFile(testFile);

    // Загружаем и проверяем восстановление
    EXPECT_NO_THROW(settings.loadSettings(testSettingsFile));
    ASSERT_TRUE(settings.getRecentOpenFiles().contains(testFile));
}
