#include <gtest/gtest.h>

#include <GraphObject/TestGenerators.h>

#include <QGuiApplication>
#include <QFile>

#include "../src/gse_format.h"
TEST(FormatSaving, GSE_Format) {
    int argc = 0;
    char** argv = nullptr;
    QGuiApplication app(argc, argv);

    auto gMaintaner = Graph::TestGenerators::createTestGraph();
    auto savedGraph = gMaintaner->getObject();

    Filework::GSE_Format saveFormat;

    auto graphCopy = savedGraph; // Для чистоты исследований (проверка бага на затирание данных)
    saveFormat.setGraphMaintaner(gMaintaner);

    QString testTargetPath = "/tmp/GraphEditorSaveTest.gse";

    // С расширением
    EXPECT_EQ(saveFormat.save(testTargetPath), true);

    EXPECT_EQ(graphCopy, savedGraph);

    auto gMaintanerLoaded = Graph::GraphMaintainer::createInstance();
    saveFormat.setGraphMaintaner(gMaintanerLoaded);
    EXPECT_EQ(saveFormat.load(testTargetPath), true);

    QFile::remove(testTargetPath);
    EXPECT_EQ(*gMaintanerLoaded, *gMaintaner);
}
