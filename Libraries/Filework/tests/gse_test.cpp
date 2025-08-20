#include <GraphObject/TestGenerators.h>
#include <gtest/gtest.h>

#include <QFile>
#include <QGuiApplication>

#include "../src/gse_format.h"
TEST(FormatSaving, GSE_Format) {
  int argc = 0;
  char **argv = nullptr;
  QGuiApplication app(argc, argv);

  auto gMaintaner = Graph::TestGenerators::createTestGraph();
  auto savedGraph = gMaintaner->getObject();

  Filework::GSE_Format saveFormat;

  auto graphCopy = savedGraph; // Для чистоты исследований (проверка бага на
                               // затирание данных)
  saveFormat.setGraphMaintainer(gMaintaner);

  QString testTargetPath = "/tmp/GraphEditorSaveTest.gse";

  // С расширением
  EXPECT_TRUE(saveFormat.save(testTargetPath));
  EXPECT_EQ(graphCopy, savedGraph);

  auto gMaintanerLoaded = Graph::GraphMaintainer::createInstance();
  saveFormat.setGraphMaintainer(gMaintanerLoaded);
  EXPECT_TRUE(saveFormat.load(testTargetPath));

  QFile::remove(testTargetPath);
  EXPECT_EQ(*gMaintanerLoaded, *gMaintaner);
}
