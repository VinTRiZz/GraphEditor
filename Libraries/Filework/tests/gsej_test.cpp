#include <GraphObject/TestGenerators.h>
#include <gtest/gtest.h>

#include <QFile>
#include <QGuiApplication>

#include "../src/gsej_format.h"
TEST(FormatSaving, GSE_JSON_Encrypted_Format) {
    int argc = 0;
    char** argv = nullptr;
    QGuiApplication app(argc, argv);

    auto gMaintaner = Graph::TestGenerators::createTestGraph();
    auto savedGraph = gMaintaner->getObject();

    Filework::GSEJ_Format saveFormat;
    saveFormat.setEncryptionKey("Example key to save with");

    auto graphCopy = savedGraph;  // Для чистоты исследований (проверка бага на
                                  // затирание данных)
    saveFormat.setGraphMaintainer(gMaintaner);

    QString testTargetPath = "/tmp/GraphEditorSaveTest.gsej";
    EXPECT_TRUE(saveFormat.save(testTargetPath));
    EXPECT_EQ(graphCopy, savedGraph);

    auto gMaintanerLoaded = Graph::GraphMaintainer::createInstance();
    saveFormat.setGraphMaintainer(gMaintanerLoaded);
    EXPECT_TRUE(saveFormat.load(testTargetPath));

    QFile::remove(testTargetPath);
    EXPECT_EQ(*gMaintanerLoaded, *gMaintaner);
}
