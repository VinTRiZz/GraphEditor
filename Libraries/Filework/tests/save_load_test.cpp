#include <gtest/gtest.h>

#include <GraphObject/TestGenerators.h>

#include <QGuiApplication>
#include <QFile>

#include "../src/savemaster.h"
TEST(FormatSaving, InterfaceSaveTest) {
    int argc = 0;
    char** argv = nullptr;
    QGuiApplication app(argc, argv);

    auto savedGraph = Graph::TestGenerators::createTestGraph();
    auto graphCopy = savedGraph; // Для чистоты исследований (проверка бага на затирание данных)

    SaveMaster saver;

    QString testTargetPathEmptyExtension = "/tmp/GraphEditorSaveTest";
    EXPECT_EQ(saver.save(testTargetPathEmptyExtension, &graphCopy), true);
    EXPECT_EQ(graphCopy, savedGraph);

    Graph::GraphObject loadedGraph;
    EXPECT_EQ(saver.load(saver.formatToDefaultPath(testTargetPathEmptyExtension), &loadedGraph), true);

    QFile::remove(saver.formatToDefaultPath(testTargetPathEmptyExtension));
    EXPECT_EQ(loadedGraph, savedGraph);
}
