#include "sceneitemconverter.h"

#include <GraphObject/TestGenerators.h>

#include <gtest/gtest.h>

#include <QGuiApplication>

/*
    Graph::GVertex vertex_;
    Graph::GConnection connection_;
    Graph::GraphObject graph_;
    Graph::PMaintainer pMaintainer_;
  */

TEST(SceneFieldItemTest, VertexConversion) {
    int argc = 0;
    char **argv = nullptr;
    QGuiApplication app(argc, argv);

    auto vertex = Graph::TestGenerators::randomVertex();
    LOG_DEBUG_SYNC("Test 1");

    auto* vertexObj = Graph::SceneItemConverter::fromVertex(vertex);
    ASSERT_NE(vertexObj, nullptr);

    LOG_DEBUG_SYNC("Test 2");

    Graph::GVertex convertedVertex = Graph::SceneItemConverter::toVertex(vertexObj);
    EXPECT_EQ(vertex, convertedVertex);

    LOG_DEBUG_SYNC("Test 3");

    delete vertexObj; // Очистка ресурсов
}

TEST(SceneFieldItemTest, ConnectionConversion) {
    int argc = 0;
    char **argv = nullptr;
    QGuiApplication app(argc, argv);

    auto connection = Graph::TestGenerators::randomConnection({1, 2, 3});

    auto* conObj = Graph::SceneItemConverter::fromConnection(connection);
    ASSERT_NE(conObj, nullptr);

    auto convertedCon = Graph::SceneItemConverter::toConnection(conObj);
    EXPECT_EQ(connection, convertedCon);

    delete conObj; // Очистка ресурсов
}

TEST(SceneFieldItemTest, GraphConversion) {
    int argc = 0;
    char **argv = nullptr;
    QGuiApplication app(argc, argv);

    auto testMaintainer = Graph::TestGenerators::createTestGraph();

    auto testGraph = testMaintainer->getObject();

    auto items = Graph::SceneItemConverter::fromGraph(testGraph);
    EXPECT_EQ(items.size(), 2); // Проверка количества элементов

    Graph::GraphObject convertedGraph = Graph::SceneItemConverter::toGraph(items);
    EXPECT_EQ(testGraph, convertedGraph);

    // Очистка элементов
    for (auto* item : items) delete item;
}

TEST(SceneFieldItemTest, MaintainerConversion) {
    int argc = 0;
    char **argv = nullptr;
    QGuiApplication app(argc, argv);

    auto testMaintainer = Graph::TestGenerators::createTestGraph();

    auto items = Graph::SceneItemConverter::fromMaintainer(testMaintainer);
    EXPECT_EQ(items.size(), 2);

    auto pMaintainer = Graph::GraphMaintainer::createInstance();
    Graph::SceneItemConverter::toMaintainer(pMaintainer, items);

    // Сравнение исходного и преобразованного Maintainer
    EXPECT_EQ(*testMaintainer, *pMaintainer);

    for (auto* item : items) delete item;
}
