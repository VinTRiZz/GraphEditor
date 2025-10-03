#include "directorymanager.h"

#include <gtest/gtest.h>

TEST(DirectoryManagerTest, SingletonWork) {
    DirectoryManager& instance1 = DirectoryManager::getInstance();
    DirectoryManager& instance2 = DirectoryManager::getInstance();

    EXPECT_EQ(&instance1, &instance2);
}

TEST(DirectoryManagerTest, NativePathsCheck) {
    DirectoryManager& dm = DirectoryManager::getInstance();

    QString rootPath =
        dm.getSystemDirectoryPath(DirectoryManager::DirectoryTypeSystem::Root);
    QString configPath = dm.getSystemDirectoryPath(
        DirectoryManager::DirectoryTypeSystem::Profiles);
    QString logsPath =
        dm.getSystemDirectoryPath(DirectoryManager::DirectoryTypeSystem::Logs);

    EXPECT_FALSE(rootPath.isEmpty());
    EXPECT_FALSE(configPath.isEmpty());
    EXPECT_FALSE(logsPath.isEmpty());

    QString logsPathNative = dm.getSystemDirectoryPath(
        DirectoryManager::DirectoryTypeSystem::Logs, true);
    QString logsPathUniversal = dm.getSystemDirectoryPath(
        DirectoryManager::DirectoryTypeSystem::Logs, false);

#ifdef Q_OS_WIN
    EXPECT_TRUE(logsPathNative.contains('\\'));
#else
    EXPECT_TRUE(logsPathNative.contains('/'));
#endif

    EXPECT_TRUE(logsPathUniversal.contains('/'));
}

TEST(DirectoryManagerTest, DirectoryExistanceCheck) {
    DirectoryManager& dm = DirectoryManager::getInstance();

    QDir rootDir =
        dm.getSystemDirectoryPath(DirectoryManager::DirectoryTypeSystem::Root);
    EXPECT_TRUE(rootDir.exists());

    QDir configDir = dm.getSystemDirectoryPath(
        DirectoryManager::DirectoryTypeSystem::Profiles);
    EXPECT_TRUE(configDir.exists());

    QDir logsDir =
        dm.getSystemDirectoryPath(DirectoryManager::DirectoryTypeSystem::Logs);
    EXPECT_TRUE(logsDir.exists());

    QDir backupDir = dm.getSystemDirectoryPath(
        DirectoryManager::DirectoryTypeSystem::Backup);
    EXPECT_TRUE(backupDir.exists());
}

TEST(DirectoryManagerTest, DirectoryIdentityCheck) {
    DirectoryManager& dm = DirectoryManager::getInstance();

    QDir configDir1 = dm.getSystemDirectoryPath(
        DirectoryManager::DirectoryTypeSystem::Profiles);
    QDir configDir2 = dm.getSystemDirectoryPath(
        DirectoryManager::DirectoryTypeSystem::Profiles);
    EXPECT_EQ(configDir1.absolutePath(), configDir2.absolutePath());

    QString configPath1 = dm.getSystemDirectoryPath(
        DirectoryManager::DirectoryTypeSystem::Profiles);
    QString configPath2 = dm.getSystemDirectoryPath(
        DirectoryManager::DirectoryTypeSystem::Profiles);
    EXPECT_EQ(configPath1, configPath2);
}
