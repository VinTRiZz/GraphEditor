#include "sceneitemconverter.h"

#include <GraphObject/TestGenerators.h>
#include <gtest/gtest.h>

#include <QApplication>

class SceneFieldItemTest : public ::testing::Test {
protected:
    // For imagination
};

TEST_F(SceneFieldItemTest, VertexConversion) {
    auto vertex = Graph::TestGenerators::randomVertex();

    auto* vertexObj = Graph::SceneItemConverter::fromVertex(vertex);
    ASSERT_NE(vertexObj, nullptr);

    Graph::GVertex convertedVertex =
        Graph::SceneItemConverter::toVertex(vertexObj);

    EXPECT_EQ(vertex, convertedVertex);

    delete vertexObj;  // Очистка ресурсов
}

TEST_F(SceneFieldItemTest, ConnectionConversion) {
    auto connection = Graph::TestGenerators::randomConnection({1, 2, 3});

    auto* conObj = Graph::SceneItemConverter::fromConnection(connection);
    ASSERT_NE(conObj, nullptr);

    auto convertedCon = Graph::SceneItemConverter::toConnection(conObj);

    // Не должно быть преобразований, т.к. невалидны vertexFrom и vertexTo
    EXPECT_NE(connection, convertedCon);

    delete conObj;  // Очистка ресурсов
}

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
        if (item->getType() == ObjectViewConstants::OBJECTTYPE_VERTEX) {
            delete item;
        }
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
        if (item->getType() == ObjectViewConstants::OBJECTTYPE_VERTEX) {
            delete item;
        }
    }
}

int argc{0};
QApplication app(argc, nullptr);
