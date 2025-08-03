#include <GraphObject/Object.h>
#include <gtest/gtest.h>

#include <QDateTime>

using namespace Graph;

class GraphObjectTest : public ::testing::Test {
protected:
    void SetUp() override {
        // Подготовка тестовых данных
        v1.id = 1;
        v1.shortName = "V1";
        v1.name = "Vertex 1";

        v2.id = 2;
        v2.shortName = "V2";
        v2.name = "Vertex 2";

        v3.id = 3;
        v3.shortName = "V3";
        v3.name = "Vertex 3";

        conn1.idFrom = 1;
        conn1.idTo = 2;
        conn1.name = "Edge1-2";

        conn2.idFrom = 2;
        conn2.idTo = 3;
        conn2.name = "Edge2-3";

        conn3.idFrom = 1;
        conn3.idTo = 3;
        conn3.name = "Edge1-3";
    }

    GraphObject graph;
    GVertex v1, v2, v3;
    GConnection conn1, conn2, conn3;
};

// Тесты вершин
TEST_F(GraphObjectTest, AddVertex) {
    graph.clearConnections();
    graph.clearVertices();

    EXPECT_TRUE(graph.addVertex(v1));
    EXPECT_TRUE(graph.addVertex(v2));
    EXPECT_FALSE(graph.addVertex(v1));  // Дубликат
}

TEST_F(GraphObjectTest, UpdateVertex) {
    graph.clearConnections();
    graph.clearVertices();

    graph.addVertex(v1);

    GVertex updated = v1;
    updated.name = "Updated Vertex";
    EXPECT_TRUE(graph.updateVertex(updated));

    auto vert = graph.getVertex(v1.id);
    ASSERT_TRUE(vert.has_value());
    EXPECT_EQ(vert->name, "Updated Vertex");

    GVertex newVert;
    newVert.id = 99;
    EXPECT_FALSE(graph.updateVertex(newVert));  // Несуществующая вершина
}

TEST_F(GraphObjectTest, GetVertex) {
    graph.clearConnections();
    graph.clearVertices();

    graph.addVertex(v1);
    graph.addVertex(v2);

    auto vert1 = graph.getVertex(1);
    ASSERT_TRUE(vert1.has_value());
    EXPECT_EQ(vert1->id, 1);

    auto vert3 = graph.getVertex(3);
    EXPECT_FALSE(vert3.has_value());
}

TEST_F(GraphObjectTest, GetAllVertices) {
    graph.clearConnections();
    graph.clearVertices();

    graph.addVertex(v1);
    graph.addVertex(v2);

    auto vertices = graph.getAllVertices();
    ASSERT_EQ(vertices.size(), 2);
    EXPECT_EQ(vertices.front().id, 1);
    EXPECT_EQ(vertices.back().id, 2);
}

TEST_F(GraphObjectTest, RemoveVertex) {
    graph.clearConnections();
    graph.clearVertices();

    graph.addVertex(v1);
    graph.addVertex(v2);
    graph.addConnection(conn1);

    graph.removeVertex(1);
    EXPECT_FALSE(graph.getVertex(1).has_value());
    EXPECT_EQ(graph.getVerticesCount(), 1);

    // Соединения должны быть удалены
    EXPECT_TRUE(graph.getConnectionsFromVertex(1).empty());
}

TEST_F(GraphObjectTest, ClearVertices) {
    graph.clearConnections();
    graph.clearVertices();

    graph.addVertex(v1);
    graph.addVertex(v2);
    graph.addConnection(conn1);

    graph.clearVertices();
    EXPECT_EQ(graph.getVerticesCount(), 0);
    EXPECT_TRUE(graph.getAllConnections().empty());
}

// Тесты соединений
TEST_F(GraphObjectTest, AddConnection) {
    graph.clearConnections();
    graph.clearVertices();

    graph.addVertex(v1);
    graph.addVertex(v2);

    EXPECT_TRUE(graph.addConnection(conn1));
    EXPECT_FALSE(graph.addConnection(conn1));  // Дубликат

    GConnection invalid;
    invalid.idFrom = 99;
    invalid.idTo = 1;
    EXPECT_FALSE(graph.addConnection(invalid));  // Несуществующие вершины
}

TEST_F(GraphObjectTest, GetConnectionsFromVertex) {
    graph.clearConnections();
    graph.clearVertices();

    graph.addVertex(v1);
    graph.addVertex(v2);
    graph.addVertex(v3);
    graph.addConnection(conn1);
    graph.addConnection(conn3);

    auto conns = graph.getConnectionsFromVertex(1);
    ASSERT_EQ(conns.size(), 2);
    EXPECT_EQ(conns[0].idTo, 2);
    EXPECT_EQ(conns[1].idTo, 3);

    EXPECT_TRUE(graph.getConnectionsFromVertex(3).empty());
}

TEST_F(GraphObjectTest, GetConnection) {
    graph.clearConnections();
    graph.clearVertices();

    graph.addVertex(v1);
    graph.addVertex(v2);
    graph.addConnection(conn1);

    auto conn = graph.getConnection(1, 2);
    ASSERT_TRUE(conn.has_value());
    EXPECT_EQ(conn->name, "Edge1-2");

    EXPECT_FALSE(graph.getConnection(2, 1).has_value());
}

TEST_F(GraphObjectTest, GetAllConnections) {
    graph.clearConnections();
    graph.clearVertices();

    graph.addVertex(v1);
    graph.addVertex(v2);
    graph.addVertex(v3);
    graph.addConnection(conn1);
    graph.addConnection(conn2);

    auto conns = graph.getAllConnections();
    ASSERT_EQ(conns.size(), 2);
    EXPECT_EQ(conns[0].idFrom, 1);
    EXPECT_EQ(conns[1].idFrom, 2);
}
