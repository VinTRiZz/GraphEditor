#include <GraphObject/TestGenerators.h>
#include <gtest/gtest.h>

#include <QFile>
#include <QGuiApplication>

#include "../src/gsj_format.h"
TEST(FormatSaving, GSE_JSON_Format) {
    int argc = 0;
    char** argv = nullptr;
    QGuiApplication app(argc, argv);

    auto gMaintaner = Graph::TestGenerators::createTestGraph();
    auto savedGraph = gMaintaner->getObject();

    Filework::GSJ_Format saveFormat;

    auto graphCopy = savedGraph;  // Для чистоты исследований (проверка бага на
                                  // затирание данных)
    saveFormat.setGraphMaintaner(gMaintaner);

    EXPECT_TRUE(saveFormat.isStructureValid(saveFormat.toDataJson()));

    QString testTargetPath = "/tmp/GraphEditorSaveTest.gsj";
    EXPECT_TRUE(saveFormat.save(testTargetPath));
    EXPECT_EQ(graphCopy, savedGraph);

    auto gMaintanerLoaded = Graph::GraphMaintainer::createInstance();
    saveFormat.setGraphMaintaner(gMaintanerLoaded);
    EXPECT_TRUE(saveFormat.load(testTargetPath));

    QFile::remove(testTargetPath);
    EXPECT_EQ(*gMaintanerLoaded, *gMaintaner);
}
