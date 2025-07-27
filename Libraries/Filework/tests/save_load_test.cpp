#include <gtest/gtest.h>

#include <GraphObject/TestGenerators.h>

#include <QGuiApplication>
#include <QFile>

#include "../src/savemaster.h"
TEST(FormatSaving, InterfaceSaveTest) {
    int argc = 0;
    char** argv = nullptr;
    QGuiApplication app(argc, argv);

    SaveMaster saver;

    auto gMaintaner = Graph::TestGenerators::createTestGraph();
    auto graphCopy = gMaintaner->getObject(); // Для чистоты исследований (проверка бага на затирание данных)

    QString testTargetPathEmptyExtension = "/tmp/GraphEditorSaveTest";
    EXPECT_EQ(saver.save(testTargetPathEmptyExtension, gMaintaner), true);
    EXPECT_EQ(graphCopy, gMaintaner->getObject());

    auto gMaintanerLoaded = Graph::GraphMaintainer::createInstance();
    EXPECT_EQ(saver.load(saver.formatToDefaultPath(testTargetPathEmptyExtension), gMaintanerLoaded), true);

    QFile::remove(saver.formatToDefaultPath(testTargetPathEmptyExtension));
    EXPECT_EQ(gMaintanerLoaded->getObject(), gMaintaner->getObject());
}
