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
        dm.getDirectoryPath(DirectoryManager::DirectoryType::Root);
    QString configPath =
        dm.getDirectoryPath(DirectoryManager::DirectoryType::Config);
    QString logsPath =
        dm.getDirectoryPath(DirectoryManager::DirectoryType::Logs);

    EXPECT_FALSE(rootPath.isEmpty());
    EXPECT_FALSE(configPath.isEmpty());
    EXPECT_FALSE(logsPath.isEmpty());

    QString rootPathNative =
        dm.getDirectoryPath(DirectoryManager::DirectoryType::Root, true);
    QString rootPathUniversal =
        dm.getDirectoryPath(DirectoryManager::DirectoryType::Root, false);

#ifdef Q_OS_WIN
    EXPECT_TRUE(rootPathNative.contains('\\'));
#else
    EXPECT_TRUE(rootPathNative.contains('/'));
#endif

    EXPECT_TRUE(rootPathUniversal.contains('/'));
}

TEST(DirectoryManagerTest, DirectoryExistanceCheck) {
    DirectoryManager& dm = DirectoryManager::getInstance();

    QDir rootDir = dm.getDirectory(DirectoryManager::DirectoryType::Root);
    EXPECT_TRUE(rootDir.exists());

    QDir configDir = dm.getDirectory(DirectoryManager::DirectoryType::Config);
    EXPECT_TRUE(configDir.exists());

    QDir logsDir = dm.getDirectory(DirectoryManager::DirectoryType::Logs);
    EXPECT_TRUE(logsDir.exists());

    QDir docsDir = dm.getDirectory(DirectoryManager::DirectoryType::Documents);
    EXPECT_TRUE(docsDir.exists());

    QDir tempDir = dm.getDirectory(DirectoryManager::DirectoryType::Temporary);
    EXPECT_TRUE(tempDir.exists());

    QDir backupDir = dm.getDirectory(DirectoryManager::DirectoryType::Backup);
    EXPECT_TRUE(backupDir.exists());
}

TEST(DirectoryManagerTest, DirectoryIdentityCheck) {
    DirectoryManager& dm = DirectoryManager::getInstance();

    QDir configDir1 = dm.getDirectory(DirectoryManager::DirectoryType::Config);
    QDir configDir2 = dm.getDirectory(DirectoryManager::DirectoryType::Config);
    EXPECT_EQ(configDir1.absolutePath(), configDir2.absolutePath());

    QString configPath1 =
        dm.getDirectoryPath(DirectoryManager::DirectoryType::Config);
    QString configPath2 =
        dm.getDirectoryPath(DirectoryManager::DirectoryType::Config);
    EXPECT_EQ(configPath1, configPath2);
}
