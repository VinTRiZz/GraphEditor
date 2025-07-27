#include <gtest/gtest.h>

#include <GraphObject/TestGenerators.h>

#include <QGuiApplication>
#include <QFile>

#include "../src/gseje_format.h"
TEST(FormatSaving, GSE_JSON_Encrypted_Format) {
    int argc = 0;
    char** argv = nullptr;
    QGuiApplication app(argc, argv);

    auto gMaintaner = Graph::TestGenerators::createTestGraph();
    auto savedGraph = gMaintaner->getObject();

    Filework::GSEJE_Format saveFormat;
    saveFormat.setKey("Example key to save with");

    auto graphCopy = savedGraph; // Для чистоты исследований (проверка бага на затирание данных)
    saveFormat.setGraphMaintaner(gMaintaner);

    QString testTargetPath = "/tmp/GraphEditorSaveTest.gseje";

    // С расширением
    EXPECT_EQ(saveFormat.save(testTargetPath), true);

    EXPECT_EQ(graphCopy, savedGraph);

    auto gMaintanerLoaded = Graph::GraphMaintainer::createInstance();
    saveFormat.setGraphMaintaner(gMaintanerLoaded);
    EXPECT_EQ(saveFormat.load(testTargetPath), true);

    QFile::remove(testTargetPath);
    EXPECT_EQ(*gMaintanerLoaded, *gMaintaner);
}
