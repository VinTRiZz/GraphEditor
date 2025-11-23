#include "sceneitemconverter.hpp"

#include <GraphObject/TestGenerators.h>
#include <gtest/gtest.h>

#include <QApplication>

#include <GraphItems/GraphItemsConstants.h>

class SceneFieldItemTest : public ::testing::Test {
protected:
    // For imagination
};

TEST_F(SceneFieldItemTest, GraphConversion) {
    auto testMaintainer = Graph::TestGenerators::createTestGraph();
    auto& testGraph = testMaintainer->getObject();

    std::size_t itemCount{0};
    itemCount += testGraph.getAllVertices().size();
    itemCount += testGraph.getAllConnections().size();

    auto items = Graph::SceneItemConverter::fromGraph(testGraph);
    EXPECT_EQ(items.size(), itemCount);  // Проверка количества элементов

    auto convertedGraph = Graph::SceneItemConverter::toGraph(items);
    EXPECT_EQ(testGraph, convertedGraph);

    // Очистка элементов
    for (auto* item : items) {
        delete item;
    }
}

TEST_F(SceneFieldItemTest, MaintainerConversion) {
    auto testMaintainer = Graph::TestGenerators::createTestGraph();

    auto items = Graph::SceneItemConverter::fromMaintainer(testMaintainer);

    auto pMaintainer = Graph::GraphMaintainer::createInstance();
    Graph::SceneItemConverter::toMaintainer(pMaintainer, items);

    // Тут нужно сравнение будущих элементов в мейнтейнере
    EXPECT_EQ(testMaintainer->getObject(), pMaintainer->getObject());

    // Очистка элементов
    for (auto* item : items) {
        delete item;
    }
}

int argc{0};
QApplication app(argc, nullptr);
