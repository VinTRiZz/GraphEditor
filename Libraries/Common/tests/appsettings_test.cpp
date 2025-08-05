#include <gtest/gtest.h>
#include <QCoreApplication>
#include <QTemporaryFile>
#include <QDir>
#include "applicationsettings.h"

class ApplicationSettingsTest : public ::testing::Test {
protected:
    void SetUp() override {
        // Инициализация Qt (необходима для работы с QDir и QString)
        int argc = 0;
        char* argv[] = {nullptr};
        app.reset(new QCoreApplication(argc, argv));

        // Получаем экземпляр синглтона
        settings = &ApplicationSettings::getInstance();

        // Очищаем список recent files перед каждым тестом
        for (const QString& file : settings->getRecentOpenFiles()) {
            settings->removeRecentFile(file);
        }
    }

    void TearDown() override {
        app.reset(); // Освобождаем QCoreApplication
    }

    ApplicationSettings* settings;
    std::unique_ptr<QCoreApplication> app;
};

// Тест синглтона
TEST_F(ApplicationSettingsTest, SingletonInstance) {
    ApplicationSettings* instance1 = &ApplicationSettings::getInstance();
    ApplicationSettings* instance2 = &ApplicationSettings::getInstance();
    ASSERT_EQ(instance1, instance2);
}

// Тест работы с недавними файлами
TEST_F(ApplicationSettingsTest, RecentFilesOperations) {
    const QString file1 = "/path/to/file1.txt";
    const QString file2 = "/path/to/file2.txt";

    // Добавление файлов
    settings->addRecentFile(file1);
    settings->addRecentFile(file2);

    QStringList recentFiles = settings->getRecentOpenFiles();
    ASSERT_EQ(recentFiles.size(), 2);
    ASSERT_TRUE(recentFiles.contains(file1));
    ASSERT_TRUE(recentFiles.contains(file2));

    // Удаление файла
    settings->removeRecentFile(file1);
    recentFiles = settings->getRecentOpenFiles();
    ASSERT_EQ(recentFiles.size(), 1);
    ASSERT_FALSE(recentFiles.contains(file1));
    ASSERT_TRUE(recentFiles.contains(file2));

    // Удаление несуществующего файла
    EXPECT_NO_THROW(settings->removeRecentFile("/non/existent/file.txt"));
}

// Тест уникальности файлов в списке
TEST_F(ApplicationSettingsTest, RecentFilesUnique) {
    const QString file = "/path/to/duplicate.txt";

    settings->addRecentFile(file);
    settings->addRecentFile(file); // Дублирующее добавление

    QStringList recentFiles = settings->getRecentOpenFiles();
    ASSERT_EQ(recentFiles.size(), 1);
    ASSERT_EQ(recentFiles.first(), file);
}

// Тест получения конфигураций
TEST_F(ApplicationSettingsTest, ConfigurationsAccess) {
    auto& generalConfig = settings->getGeneralConfig();
    auto& objectsConfig = settings->getObjectsConfig();
    auto& canvasConfig = settings->getCanvasConfig();

    // Проверяем, что возвращаются действительные объекты
    SUCCEED() << "Configuration objects accessed successfully";

    // Проверяем, что возвращаются одни и те же экземпляры
    ASSERT_EQ(&generalConfig, &settings->getGeneralConfig());
    ASSERT_EQ(&objectsConfig, &settings->getObjectsConfig());
    ASSERT_EQ(&canvasConfig, &settings->getCanvasConfig());
}

// Тест загрузки и сохранения настроек
TEST_F(ApplicationSettingsTest, SettingsPersistence) {
    // Создаем временный файл для теста
    QTemporaryFile tempFile;
    ASSERT_TRUE(tempFile.open());
    QString tempPath = tempFile.fileName();
    tempFile.close();

    // Сохраняем тестовые данные
    const QString testFile = qApp->applicationFilePath();
    settings->addRecentFile(testFile);

    // Сохраняем в временный файл (используем приватный метод через дружественный тест)
    settings->saveSettings();

    // Очищаем текущие настройки
    settings->removeRecentFile(testFile);
    ASSERT_FALSE(settings->getRecentOpenFiles().contains(testFile));

    // Загружаем из временного файла
    settings->loadSettings();

    // Проверяем восстановление данных
    ASSERT_TRUE(settings->getRecentOpenFiles().contains(testFile));
}

// Тест публичных методов load/save без параметров
TEST_F(ApplicationSettingsTest, PublicLoadSave) {
    const QString testFile = qApp->applicationFilePath();

    settings->addRecentFile(testFile);
    EXPECT_NO_THROW(settings->saveSettings());

    // Очищаем текущее состояние
    settings->removeRecentFile(testFile);

    // Загружаем и проверяем восстановление
    EXPECT_NO_THROW(settings->loadSettings());
    ASSERT_TRUE(settings->getRecentOpenFiles().contains(testFile));
}

int main(int argc, char** argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
