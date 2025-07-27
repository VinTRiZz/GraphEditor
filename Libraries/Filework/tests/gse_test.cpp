#include <gtest/gtest.h>

#include <GraphObject/TestGenerators.h>

#include <QGuiApplication>
#include <QFile>

#include "../src/gse_format.h"
TEST(FormatSaving, GSE_Format) {
    int argc = 0;
    char** argv = nullptr;
    QGuiApplication app(argc, argv);

    auto savedGraph = Graph::TestGenerators::createTestGraph();

    Filework::GSE_Format gseFormat;

    auto graphCopy = savedGraph; // Для чистоты исследований (проверка бага на затирание данных)
    gseFormat.setGraph(&graphCopy);

    QString testTargetPath = "/tmp/GraphEditorSaveTest.gse";

    // С расширением
    EXPECT_EQ(gseFormat.save(testTargetPath), true);

    EXPECT_EQ(graphCopy, savedGraph);

    Graph::GraphObject loadedGraph;
    gseFormat.setGraph(&loadedGraph);
    EXPECT_EQ(gseFormat.load(testTargetPath), true);

    QFile::remove(testTargetPath);
    EXPECT_EQ(loadedGraph, graphCopy);
}
